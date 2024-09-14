#include <string>
#include <windows.h>

std::string acp_to_utf8(std::string const codepage_str)
{
    auto ACP = GetACP();
    if (ACP == CP_UTF8)
    {
        return codepage_str;
    }

    int size = MultiByteToWideChar(CP_ACP, MB_COMPOSITE, codepage_str.c_str(), codepage_str.length(), nullptr, 0);
    std::wstring utf16_str(size, '\0');
    MultiByteToWideChar(CP_ACP, MB_COMPOSITE, codepage_str.c_str(), codepage_str.length(), &utf16_str[0], size);

    int utf8_size =
        WideCharToMultiByte(CP_UTF8, 0, utf16_str.c_str(), utf16_str.length(), nullptr, 0, nullptr, nullptr);
    std::string utf8_str(utf8_size, '\0');
    WideCharToMultiByte(CP_UTF8, 0, utf16_str.c_str(), utf16_str.length(), &utf8_str[0], utf8_size, nullptr, nullptr);

    return utf8_str;
}

std::string utf8_to_acp(std::string const src)
{
    auto ACP = GetACP();
    std::wstring temp;
    int const length = MultiByteToWideChar(CP_UTF8, 0, src.c_str(), src.length(), nullptr, 0);

    wchar_t *wideBuffer = new wchar_t[length];
    MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, wideBuffer, length);

    char *destBuffer = new char[length * 2];
    int const result = WideCharToMultiByte(ACP, 0, wideBuffer, -1, destBuffer, length * 2, nullptr, nullptr);
    return std::string(destBuffer);
}
