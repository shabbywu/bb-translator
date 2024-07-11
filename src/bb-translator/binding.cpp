#include <Python.h>
#include <chrono>
#include <fstream>
#include <hello_imgui/hello_imgui.h>
#include <iostream>
#include <mutex>
#include <pybind11/embed.h>
#include <thread>
#include <vector>
#include <zip.h>

#include "binding.h"
#include "../utf8.h"
#include <bundle/base_library.h>
#include <bundle/bb_translator.h>
#include <bundle/purepython.h>
#include <bundle/python.h>

using namespace std::chrono_literals;

namespace py = pybind11;
namespace memory_importer
{
void register_memory_importer(py::module_ &m);
namespace physfs
{
void register_physfs(py::module_ &m);
}
} // namespace memory_importer
void register_nutcracker(py::module_ &m);

PYBIND11_EMBEDDED_MODULE(memory_importer, m)
{
    memory_importer::register_memory_importer(m);
    py::module mPhysfs = m.def_submodule("physfs", "physfs lib");
    memory_importer::physfs::register_physfs(mPhysfs);
}

PYBIND11_EMBEDDED_MODULE(_nutcracker, m)
{
    register_nutcracker(m);
    m.attr("__version__") = "embed";
    m.attr("__author__") = "shabbywu<shabbywu@qq.com>";
}

std::thread daemon_thread;
std::unique_ptr<py::scoped_interpreter> guard;
std::unique_ptr<py::gil_scoped_release> release;
std::mutex mtx;

void daemon_worker_thread(AppState *state)
{
    state->addLog("[*] worker daemon started");
    while (!state->appShallExit)
    {
        if (mtx.try_lock())
        {
            py::gil_scoped_acquire acquire;
            py::dict locals;
            locals["lang"] = state->lang == LangCN ? "cn" : "en";
            try
            {
                py::exec(R"(
                    from pathlib import Path
                    from bb_translator.entrance import do_translate
                    if queues:
                        # set_lang(lang)
                        game_path, json_path  = queues.pop(0)
                        do_translate(Path(game_path), Path(json_path), addLog)
                )",
                         py::globals(), locals);
            }
            catch (const py::error_already_set &e)
            {
                state->addLog(e.what());
            }
            mtx.unlock();
        }
        std::this_thread::sleep_for(100ms);
    }
    state->addLog("[*] worker daemon exited");
    state->gracefulExit = true;
}

void setup_python(AppState *state)
{
    {
        putenv("PYTHONIOENCODING=utf-8");
        // 设置目录
        const auto pythonRootDir = (state->pythonRootDir).wstring();
        if (!std::filesystem::exists(state->pythonRootDir))
        {
            std::filesystem::create_directories(state->pythonRootDir);
        }

        auto &pybaseLibrary = bin2cpp::getBase_libraryZipFile();
        if (!std::filesystem::exists(state->pythonRootDir / pybaseLibrary.getFileName()))
        {
            auto pybaseLibraryPath = state->pythonRootDir / pybaseLibrary.getFileName();
            std::ofstream f(pybaseLibraryPath, std::ios::out | std::ios::binary | std::ios::trunc);
            if (f.fail())
                return;
            f.write((const char *)pybaseLibrary.getBuffer(), pybaseLibrary.getSize());
            f.close();
        }

        if (!std::filesystem::exists(state->pythonRootDir / ".binary.unzip.v1"))
        {
            auto &pythonZip = bin2cpp::getPythonZipFile();
            auto path = state->pythonRootDir.string();
            if (auto error_code =
                    zip_stream_extract(pythonZip.getBuffer(), pythonZip.getSize(), path.data(), nullptr, nullptr);
                error_code < 0)
            {
                std::cout << "failed to extract binary: " << zip_strerror(error_code) << std::endl;
            }
            else
            {
                std::ofstream f(state->pythonRootDir / ".binary.unzip.v1",
                                std::ios::out | std::ios::binary | std::ios::trunc);
                f.close();
            }
        }

#ifdef _WIN32
        auto pythonHome = pythonRootDir + L"\\";
        auto pythonPath = pythonRootDir + L"\\base_library.zip;" + pythonRootDir + L";";
#else
        auto pythonHome = pythonRootDir + L"/";
        auto pythonPath =
            pythonRootDir + L"/base_library.zip:" + pythonRootDir + L":" + pythonRootDir + L"/lib-dynload:";
#endif

        Py_SetProgramName(L"bb-translator");
        Py_SetPath(pythonPath.c_str());

        guard = std::make_unique<py::scoped_interpreter>();
        // ensure python object will release before gil_scoped_release
        {
            auto physfs = py::module_::import("memory_importer.physfs");
            physfs.attr("init")();

            auto &purePython = bin2cpp::getPurepythonZipFile();
            auto purePython_buffer = py::memoryview::from_memory(purePython.getBuffer(), purePython.getSize());
            physfs.attr("mount_memory")(purePython_buffer, purePython.getFileName(), "/");

            auto &core = bin2cpp::getBb_translatorZipFile();
            auto core_buffer = py::memoryview::from_memory(core.getBuffer(), core.getSize());
            physfs.attr("mount_memory")(core_buffer, core.getFileName(), "/");

            // install memory importer as package finder
            auto sys = py::module_::import("sys");
            auto obj = py::module_::import("memory_importer").attr("PhysfsImporter")();
            // obj.attr("__setattr__")("debug", true);
            sys.attr("meta_path").attr("append")(obj);

            // register method
            auto globals = py::globals();
            globals["addLog"] =
                py::cpp_function([state](py::object message) { state->addLog(std::string(py::str(message))); });
            globals["queues"] = py::list();
        }
        // release gil for multi-threading
        release = std::make_unique<py::gil_scoped_release>();
    }
}

// start python interpreter and daemon worker
void start_python_daemon(AppState *state)
{
    daemon_thread = std::thread(daemon_worker_thread, state);
}

void shutdown_python_daemon(AppState *state)
{
    daemon_thread.detach();
    while (!state->gracefulExit)
    {
        std::this_thread::sleep_for(100ms);
    }
    release = nullptr;
    guard = nullptr;
}

void dispatch_translator(AppState *state)
{
    if (state->appShallExit)
        return;
    try
    {
        if (mtx.try_lock())
        {
            py::gil_scoped_acquire acquire;
            py::dict locals;
            locals["task"] = py::make_tuple(state->gameDir.string(), state->i18nJSONDir.string());
            py::exec(R"(
                if queues:
                    addLog("上一个任务尚未完成...")
                else:
                    queues.append(task)
            )",
                     py::globals(), locals);
            mtx.unlock();
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

void sync_translate(AppState *state)
{
    {
        py::gil_scoped_acquire acquire;
        py::dict locals;
        locals["game_path"] = state->gameDir.string();
        locals["json_path"] = state->i18nJSONDir.string();
        py::exec(R"(
            from pathlib import Path
            from bb_translator.entrance import do_translate

            do_translate(Path(game_path), Path(json_path), addLog)
        )",
                 py::globals(), locals);
    }
}
