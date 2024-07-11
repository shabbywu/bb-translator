#include <string>
#include <windows.h>


std::string acp_to_utf8(std::string const src)
{
    auto ACP = GetACP();
    if (ACP == CP_UTF8) {
        return src;
    }

    std::wstring temp;
    int const length = MultiByteToWideChar(
        ACP, 0,
        src.c_str(), -1,
        nullptr, 0);

    wchar_t* wideBuffer = new wchar_t[length];
    MultiByteToWideChar(
        ACP, 0,
        src.c_str(), -1,
        wideBuffer, length);

    char* destBuffer = new char[length*2];
    int const result = WideCharToMultiByte(
        CP_UTF8, 0,
        wideBuffer, -1,
        destBuffer, length *2,
        nullptr, nullptr);
    return std::string(destBuffer);
}


std::string utf8_to_acp(std::string const src) {
    auto ACP = GetACP();
    std::wstring temp;
    int const length = MultiByteToWideChar(
        CP_UTF8, 0,
        src.c_str(), -1,
        nullptr, 0);

    wchar_t* wideBuffer = new wchar_t[length];
    MultiByteToWideChar(
        CP_UTF8, 0,
        src.c_str(), -1,
        wideBuffer, length);

    char* destBuffer = new char[length*2];
    int const result = WideCharToMultiByte(
        ACP, 0,
        wideBuffer, -1,
        destBuffer, length *2,
        nullptr, nullptr);
    return std::string(destBuffer);
}
