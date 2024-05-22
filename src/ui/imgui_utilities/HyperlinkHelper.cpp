#include <fplus/fplus.hpp>

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#elif defined(_WIN32)
#include <Shellapi.h>
#include <windows.h>
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#include <cstdlib>
#endif

#include "HyperlinkHelper.h"

namespace HyperlinkHelper {
void OpenUrl(const std::string &url) {
  bool isAbsoluteUrl = fplus::is_prefix_of(std::string("http"), url);
  if (!isAbsoluteUrl)
    return;
#if defined(__EMSCRIPTEN__)
  std::string js_command = "window.open(\"" + url + "\");";
  emscripten_run_script(js_command.c_str());
#elif defined(_WIN32)
  ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
#elif defined(TARGET_OS_MAC)
  std::string cmd = std::string("open ") + url.c_str();
  system(cmd.c_str());
#endif
}

} // namespace HyperlinkHelper
