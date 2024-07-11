#include <argparse/argparse.hpp>
#include <chrono>
#include <frozen/unordered_map.h>
#include <fstream>
#include <iostream>
#include <thread>

#include "hello_imgui/hello_imgui.h"
#include "hello_imgui/hello_imgui_include_opengl.h"
#include "ui/imgui_utilities/FontLoader.h"
#include "ui/imgui_utilities/HyperlinkHelper.h"
#include "ui/imgui_utilities/MarkdownHelper.h"
#include "ui/imgui_utilities/imFileDialog.h"

#include "assets/backgrounds/loading_screen_11.h"
#include "assets/fonts/font.h"
#include "assets/fonts/fontawesome-webfont.h"
#include "bb-translator/binding.h"
#include "git/init.h"
#include "ui/AboutWindow.h"
#include "ui/AppState.h"
#include "ui/ConsoleWindow.h"
#include "ui/OperationsWindow.h"
#include "ui/SettingsWindow.h"
#include "ui/UsageWindow.h"
#include "utf8.h"

using namespace std::chrono_literals;
HelloImGui::RunnerParams runnerParams;
AppState state;

#ifndef BB_TRANSLATOR_VERSION
#define BB_TRANSLATOR_VERSION "develop"
#endif

static constexpr enum MessageID
{
    MsgWindowTitle,
    // Menu - Link
    MsgMenuLinkTitle,
    MsgMenuLinkCNProject,
    MsgMenuLinkParatranz,
    MsgMenuLinkRelease,
    // Menu - About
    MsgMenuAboutTitle,
    MsgMenuAboutTool,
    MsgMenuAboutUsage,
    MsgMenuAboutAuthor,
    // Menu - Settings
    MsgMenuSettingTitle,
    MsgMenuSettingLocale,
    MsgMenuSettingAdvance,
    // 状态栏
    MsgStatusBar,
    // 控制台
    MsgOpenTerminal,
    // trick to get enum size
    MsgEnumTotalSize,
};

#define _(MessageID) i18nMessage.at(state.lang).at(MessageID)

static constexpr frozen::unordered_map<LangType, frozen::unordered_map<MessageID, const char *, MsgEnumTotalSize>, 2>
    i18nMessage{{LangCN,
                 {
                     {MsgWindowTitle, "战场兄弟文本翻译器"},
                     // Menu - Link
                     {MsgMenuLinkTitle, "链接"},
                     {MsgMenuLinkCNProject, "汉化项目主仓库"},
                     {MsgMenuLinkParatranz, "汉化项目协同网站"},
                     {MsgMenuLinkRelease, "战场兄弟汉化发布"},
                     // Menu - About
                     {MsgMenuAboutTitle, "关于"},
                     {MsgMenuAboutTool, "关于本工具"},
                     {MsgMenuAboutUsage, "使用说明"},
                     {MsgMenuAboutAuthor, "作者"},
                     // Menu - Settings
                     {MsgMenuSettingTitle, "选项"},
                     {MsgMenuSettingLocale, "EN/中文"},
                     {MsgMenuSettingAdvance, "高级"},
                     {
                         MsgStatusBar,
                         "战场兄弟文本翻译器 - "
                         "[shabbywu](https://github.com/shabbywu/) - "
                         "[参与汉化](https://paratranz.cn/projects/7032)"
                         " - 版本: " BB_TRANSLATOR_VERSION,
                     },
                     //
                     {MsgOpenTerminal, "打开终端"},
                 }},
                {LangEN,
                 {
                     {MsgWindowTitle, "Battle Brothers Translator"},
                     // Menu - Link
                     {MsgMenuLinkTitle, "Links"},
                     {MsgMenuLinkCNProject, "Battle-Brothers-CN Project"},
                     {MsgMenuLinkParatranz, "Chinese Collaborative Website"},
                     {MsgMenuLinkRelease, "Chinese Transaltion Release"},
                     // Menu - About
                     {MsgMenuAboutTitle, "About"},
                     {MsgMenuAboutTool, "About this tool"},
                     {MsgMenuAboutUsage, "Usage"},
                     {MsgMenuAboutAuthor, "Author"},
                     // Menu - Settings
                     {MsgMenuSettingTitle, "Settings"},
                     {MsgMenuSettingLocale, "EN/中文"},
                     {MsgMenuSettingAdvance, "Advance"},
                     {MsgStatusBar, "Battle Brothers Translator - "
                                    "[shabbywu](https://github.com/shabbywu/) - "
                                    "[Join](https://paratranz.cn/projects/7032)"
                                    " - VERSION: " BB_TRANSLATOR_VERSION},
                     {MsgOpenTerminal, "Open terminal"},
                 }}};

int gui_main()
{
    {
        auto current_path = std::filesystem::current_path();
        if (std::filesystem::exists(current_path / "data") && std::filesystem::exists(current_path / "win32"))
        {
            state.gameDir = current_path;
            auto i18nProjectDir = current_path / "Battle-Brothers-CN";
            state.i18nProjectDir = i18nProjectDir;
            auto i18nJSONDir = i18nProjectDir / "zh_CN.UTF-8" / "json";
            if (std::filesystem::exists(i18nJSONDir))
                state.i18nJSONDir = i18nJSONDir;
        }
    }

    AboutWindow aboutWindow(&runnerParams);
    UsageWindow usageWindow;
    OperationsWindow OperationsWindow(&state);
    ConsoleWindow consoleWindow(&state);
    SettingsWindow settingWindow;

    // App window params
    runnerParams.appWindowParams.windowTitle = _(MsgWindowTitle);
    runnerParams.appWindowParams.windowGeometry.size = {800, 640};

    // ini file params
    runnerParams.iniFilename = "BattleBrothersTranslator.ini";
    runnerParams.iniFolderType = HelloImGui::IniFolderType::AppExecutableFolder;

    //
    // Below, we will define all our application parameters and callbacks
    // before starting it.
    //
    runnerParams.imGuiWindowParams.tweakedTheme.Theme = ImGuiTheme::ImGuiTheme_ImGuiColorsClassic;
    runnerParams.imGuiWindowParams.tweakedTheme.Tweaks.AlphaMultiplier = 0.75f;

    // ImGui window params
    runnerParams.imGuiWindowParams.defaultImGuiWindowType =
        HelloImGui::DefaultImGuiWindowType::ProvideFullScreenDockSpace;
    runnerParams.imGuiWindowParams.showMenu_View = false;
    runnerParams.imGuiWindowParams.showMenuBar = true;
    runnerParams.imGuiWindowParams.showStatusBar = true;
    runnerParams.imGuiWindowParams.showStatus_Fps = false;
    runnerParams.imGuiWindowParams.enableViewports = true;

    runnerParams.dockingParams.dockingSplits = {
        {"MainDockSpace", "ConsoleSpace", ImGuiDir_Down, 0.4f,
         ImGuiDockNodeFlags_AutoHideTabBar | ImGuiDockNodeFlags_NoUndocking | ImGuiDockNodeFlags_NoDockingSplit},
        {"MainDockSpace", "SideCarSpace", ImGuiDir_Right, 0.60f, ImGuiDockNodeFlags_NoUndocking},
    };
    runnerParams.dockingParams.mainDockSpaceNodeFlags = ImGuiWindowFlags_DockNodeHost;

    //
    // Dockable windows definitions
    //
    {
        HelloImGui::DockableWindow dock_tools;
        {
            dock_tools.label = "操作面板";
            dock_tools.dockSpaceName = "MainDockSpace";
            dock_tools.isVisible = true;
            dock_tools.includeInViewMenu = false;
            dock_tools.canBeClosed = false;
            dock_tools.rememberIsVisible = false;
            dock_tools.GuiFunction = [&OperationsWindow] { OperationsWindow.gui(); };
            dock_tools.callBeginEnd = true;
        };

        HelloImGui::DockableWindow dock_console;
        {
            dock_console.label = "终端";
            dock_console.dockSpaceName = "ConsoleSpace";
            dock_console.isVisible = true;
            dock_console.includeInViewMenu = false;
            dock_console.canBeClosed = false;
            dock_console.rememberIsVisible = false;
            dock_console.GuiFunction = [&consoleWindow, &dock_console] {
                consoleWindow.gui(state.lang, &dock_console.isVisible);
            };
            dock_console.callBeginEnd = true;
        };

        HelloImGui::DockableWindow dock_usage;
        {
            dock_usage.label = "使用说明";
            dock_usage.dockSpaceName = "SideCarSpace";
            dock_usage.isVisible = false;
            dock_usage.includeInViewMenu = false;
            dock_usage.GuiFunction = [&usageWindow] { usageWindow.gui(state); };
            dock_usage.callBeginEnd = true;
        };

        HelloImGui::DockableWindow dock_about;
        {
            dock_about.label = "关于";
            dock_about.dockSpaceName = "SideCarSpace";
            dock_about.isVisible = true;
            dock_about.includeInViewMenu = false;
            dock_about.GuiFunction = [&aboutWindow] { aboutWindow.gui(state); };
            dock_about.callBeginEnd = true;
        };

        HelloImGui::DockableWindow dock_setting;
        {
            dock_setting.label = "选项";
            dock_setting.dockSpaceName = "SideCarSpace";
            dock_setting.isVisible = false;
            dock_setting.includeInViewMenu = false;
            dock_setting.GuiFunction = [&settingWindow] { settingWindow.gui(state); };
            dock_setting.callBeginEnd = true;
            dock_setting.rememberIsVisible = false;
        }

        //
        // Set our app dockable windows list
        //
        runnerParams.dockingParams.dockableWindows = {
            dock_tools, dock_console, dock_usage, dock_about, dock_setting,
        };
    }

    runnerParams.dockingParams.mainDockSpaceNodeFlags =
        ImGuiDockNodeFlags_NoDockingSplit | ImGuiDockNodeFlags_AutoHideTabBar;

    // Set the custom fonts
    runnerParams.callbacks.LoadAdditionalFonts = []() {
        FontLoader::LoadDefaultFont();
        MarkdownHelper::LoadFonts();
    };

    // Set the app menu
    runnerParams.callbacks.ShowAppMenuItems = [] {
        HelloImGui::DockableWindow *consoleWindow = runnerParams.dockingParams.dockableWindowOfName("终端");
        if (!consoleWindow->isVisible)
        {
            if (ImGui::MenuItem(_(MsgOpenTerminal)))
                consoleWindow->isVisible = true;
        }
    };

    // set menus
    runnerParams.callbacks.ShowMenus = [] {
        HelloImGui::DockableWindow *aboutWindow = runnerParams.dockingParams.dockableWindowOfName("关于");

        HelloImGui::DockableWindow *usageWindow = runnerParams.dockingParams.dockableWindowOfName("使用说明");

        if (ImGui::BeginMenu(_(MsgMenuLinkTitle)))
        {
            ImGui::TextDisabled(_(MsgMenuLinkTitle));
            if (ImGui::MenuItem(_(MsgMenuLinkCNProject)))
                HyperlinkHelper::OpenUrl("https://github.com/shabbywu/Battle-Brothers-CN");
            if (ImGui::MenuItem(_(MsgMenuLinkParatranz)))
                HyperlinkHelper::OpenUrl("https://paratranz.cn/projects/7032");
            if (ImGui::MenuItem(_(MsgMenuLinkRelease)))
                HyperlinkHelper::OpenUrl("https://battle-brothers-cn.shabbywu.cn/");
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(_(MsgMenuAboutTitle)))
        {
            if (ImGui::MenuItem(_(MsgMenuAboutTool)))
                aboutWindow->isVisible = true;
            if (ImGui::MenuItem(_(MsgMenuAboutUsage)))
                usageWindow->isVisible = true;
            if (ImGui::MenuItem(_(MsgMenuAboutAuthor)))
                HyperlinkHelper::OpenUrl("https://github.com/shabbywu/");
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(_(MsgMenuSettingTitle)))
        {
            if (ImGui::MenuItem(_(MsgMenuSettingLocale)))
            {
                if (state.lang == LangEN)
                {
                    state.lang = LangCN;
                }
                else
                {
                    state.lang = LangEN;
                }
                runnerParams.appWindowParams.windowTitle = _(MsgWindowTitle);
            }
            if (ImGui::MenuItem(_(MsgMenuSettingAdvance)))
            {
                HelloImGui::DockableWindow *settingWindow = runnerParams.dockingParams.dockableWindowOfName("选项");
                settingWindow->isVisible = true;
            }
            ImGui::EndMenu();
        }
    };

    // Add some widgets in the status bar
    runnerParams.callbacks.ShowStatus = [] { MarkdownHelper::Markdown(_(MsgStatusBar)); };

    // disable dark style
    runnerParams.callbacks.SetupImGuiStyle = HelloImGui::EmptyVoidFunction;

    // setup background image
    runnerParams.callbacks.BeforeImGuiRender = [] {
        auto viewport = ImGui::GetMainViewport();
        auto pos = viewport->Pos;
        auto size = viewport->Size;
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::SetNextWindowPos(ImVec2(pos[0] - 10, pos[1] - 10));
        ImGui::SetNextWindowSize(ImVec2(size[0] + 20, size[1] + 20));
        ImGui::Begin("Background", NULL,
                     ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus |
                         ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);
        HelloImGui::ImageFromAsset("background.jpeg", ImVec2(size[0] + 20, size[1] + 20));
        ImGui::End();
        // glClear(GL_COLOR_BUFFER_BIT);
    };

    // start python daemon after init
    runnerParams.callbacks.PostInit_AddPlatformBackendCallbacks = []() {
        std::filesystem::path userConfigFolder =
            HelloImGui::IniFolderLocation(HelloImGui::IniFolderType::AppUserConfigFolder);

        if (userConfigFolder.string() != acp_to_utf8(userConfigFolder.string()))
        {
            userConfigFolder = std::filesystem::current_path() / ".temp";
            std::filesystem::create_directories(userConfigFolder);

            std::ofstream f(userConfigFolder / "README.txt", std::ios::out | std::ios::binary | std::ios::trunc);

            f << "该目录存放的是临时文件, 仅当你的用户名有特殊字符时会创建该目录, 该目录可随意删除\r\n";
            f << utf8_to_acp(
                std::string("该目录存放的是临时文件, 仅当你的用户名有特殊字符时会创建该目录, 该目录可随意删除\r\n"));
            f.close();
        }

        auto appFolder = userConfigFolder / "bb-translator";
        state.assetsDir = appFolder / "assets";
        state.pythonRootDir = appFolder / "binary";

        {
            auto fontsFolder = state.assetsDir / "fonts";
            if (!std::filesystem::exists(fontsFolder))
            {
                std::filesystem::create_directories(fontsFolder);
                {
                    auto &font = bin2cpp::getFontTtfFile();
                    auto fontFilename = fontsFolder / font.getFileName();
                    std::ofstream f(fontFilename, std::ios::out | std::ios::binary | std::ios::trunc);
                    if (f.fail())
                        return;
                    f.write((const char *)font.getBuffer(), font.getSize());
                    f.close();
                }

                {
                    auto &font = bin2cpp::getFontawesomewebfontTtfFile();
                    auto fontFilename = fontsFolder / font.getFileName();
                    std::ofstream f(fontFilename, std::ios::out | std::ios::binary | std::ios::trunc);
                    if (f.fail())
                        return;
                    f.write((const char *)font.getBuffer(), font.getSize());
                    f.close();
                }
            }

            auto backgroundFilename = state.assetsDir / "background.jpeg";
            if (!std::filesystem::exists(backgroundFilename))
            {
                auto &file = bin2cpp::getLoading_screen_11JpegFile();
                std::ofstream f(backgroundFilename, std::ios::out | std::ios::binary | std::ios::trunc);
                if (f.fail())
                    return;
                f.write((const char *)file.getBuffer(), file.getSize());
                f.close();
            }
            git_init();
        }

        HelloImGui::SetAssetsFolder(acp_to_utf8(state.assetsDir.string()));
        setup_python(&state);
    };

    runnerParams.callbacks.PostInit = []() {
        if (HelloImGui::HasIniSettings(runnerParams))
        {
            if (trim_copy(HelloImGui::LoadUserPref("bb-translator.lang")) != "cn")
                state.lang = LangEN;
            else
                state.lang = LangCN;
            if (auto i18nProjectGitUrl = trim_copy(HelloImGui::LoadUserPref("bb-translator.git-url"));
                i18nProjectGitUrl != "")
            {
                state.i18nProjectGitUrl = i18nProjectGitUrl;
            }
            if (auto httpProxyUrl = trim_copy(HelloImGui::LoadUserPref("bb-translator.http-proxy")); httpProxyUrl != "")
            {
                memcpy(state.httpProxyUrl, httpProxyUrl.c_str(), httpProxyUrl.size());
            }
        }
        start_python_daemon(&state);
    };

    // notice python daemon to stop
    runnerParams.callbacks.BeforeExit = []() {
        state.appShallExit = true;
        state.addLog = [](std::string message) { std::cout << message << std::endl; };
    };
    runnerParams.callbacks.BeforeExit_PostCleanup = []() {
        shutdown_python_daemon(&state);

        HelloImGui::SaveUserPref("bb-translator.lang", state.lang == LangCN ? "cn" : "en");
        HelloImGui::SaveUserPref("bb-translator.git-url", state.i18nProjectGitUrl);
        auto httpProxyUrl = trim_copy(state.httpProxyUrl);
        HelloImGui::SaveUserPref("bb-translator.http-proxy", httpProxyUrl);
    };

    ifd::FileDialog::Instance().CreateTexture = [](uint8_t *data, int w, int h, char fmt) -> void * {
        GLuint tex;

        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, (fmt == 0) ? GL_BGRA : GL_RGBA, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        glBindTexture(GL_TEXTURE_2D, 0);

        return (void *)tex;
    };

    ifd::FileDialog::Instance().DeleteTexture = [](void *tex) {
        GLuint texID = (GLuint)(size_t)(tex);
        glDeleteTextures(1, &texID);
    };

    HelloImGui::Run(runnerParams);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        return gui_main();
    }

    argparse::ArgumentParser program("bb-translator");
    program.add_argument("-v", "--version").help("show version").default_value(false).implicit_value(true);

    program.add_argument("--game-dir").help("specify the game root directory").default_value("");

    program.add_argument("--i18n-json-dir").help("specify the i18n json directory").default_value("");

    try
    {
        program.parse_args(argc, argv);
    }
    catch (const std::exception &err)
    {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }
    if (program["--version"] == true)
    {
        std::cout << "bb-translator version " << BB_TRANSLATOR_VERSION << std::endl;
        return 0;
    }

    std::filesystem::path gameDir = program.get<std::string>("--game-dir");
    std::filesystem::path i18nJsonDir = program.get<std::string>("--i18n-json-dir");

    if (gameDir == "")
    {
        std::cerr << "must specify the game root directory" << std::endl;
        std::cerr << program;
        return 1;
    }
    state.gameDir = gameDir;
    state.i18nJSONDir = i18nJsonDir;
    if (i18nJsonDir == "")
    {
        auto i18nProjectDir = gameDir / "Battle-Brothers-CN";
        if (std::filesystem::exists(i18nProjectDir))
            state.i18nProjectDir = i18nProjectDir;
        auto i18nJSONDir = i18nProjectDir / "zh_CN.UTF-8" / "json";
        if (std::filesystem::exists(i18nJSONDir))
            state.i18nJSONDir = i18nJSONDir;
    }
    if (state.i18nJSONDir == "")
    {
        std::cerr << "must specify the i18n json directory" << std::endl;
        std::cerr << program;
        return 1;
    }

    // setup python
    {
        std::filesystem::path userConfigFolder =
            HelloImGui::IniFolderLocation(HelloImGui::IniFolderType::AppUserConfigFolder);
        auto appFolder = userConfigFolder / "bb-translator";
        state.assetsDir = appFolder / "assets";
        state.pythonRootDir = appFolder / "binary";
        setup_python(&state);
    }
    // do translate
    sync_translate(&state);
    return 0;
}
