#pragma once
#include <filesystem>
#include <functional>
#include <iostream>

enum LangType
{
    LangCN,
    LangEN,
};

// trim from start (in place)
inline void ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
}

// trim from end (in place)
inline void rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
}

// trim from both ends (in place)
inline void trim(std::string &s)
{
    rtrim(s);
    ltrim(s);
}

// trim from both ends (copying)
inline std::string trim_copy(std::string s)
{
    trim(s);
    return s;
}

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
