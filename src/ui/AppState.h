#pragma once
#include <filesystem>
#include <functional>
#include <iostream>

enum LangType
{
    LangCN,
    LangEN,
};

class AppState
{
  public:
    std::filesystem::path assetsDir;
    std::filesystem::path pythonRootDir;

    std::filesystem::path gameDir;
    std::filesystem::path i18nProjectDir;
    std::filesystem::path i18nJSONDir;

    // Git Clone
    std::string i18nProjectGitUrl = "https://github.com/shabbywu/Battle-Brothers-CN.git";
    char httpProxyUrl[256] = {0};

    LangType lang = LangCN;

    std::function<void(std::string)> addLog;

    bool gracefulExit;
    bool appShallExit;

    AppState()
    {
        addLog = [](std::string message) { std::cout << message << std::endl; };
    };
};
