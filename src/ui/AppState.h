#pragma once
#include <filesystem>
#include <functional>
#include <iostream>

enum LangType {
  LangCN,
  LangEN,
};

class AppState {
public:
  std::filesystem::path assetsDir;
  std::filesystem::path pythonRootDir;

  std::filesystem::path gameDir;
  std::filesystem::path i18nProjectDir;
  std::filesystem::path i18nJSONDir;
  std::string i18nProjectGitUrl =
      "https://ghproxy.shabbywu.cn/github.com/shabbywu/Battle-Brothers-CN.git";
  LangType lang = LangCN;

  std::function<void(std::string)> addLog;

  bool gracefulExit;
  bool appShallExit;

  AppState() {
    addLog = [](std::string message) { std::cout << message << std::endl; };
  };
};
