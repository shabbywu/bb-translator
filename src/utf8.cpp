#include "utf8.h"

#ifdef _WIN32
#include <windows.h>

namespace
{
std::wstring multibyte_to_wide(UINT code_page, DWORD flags, std::string const &src)
{
    if (src.empty())
    {
        return {};
    }

    int const size = MultiByteToWideChar(code_page, flags, src.data(), static_cast<int>(src.size()), nullptr, 0);
    if (size <= 0)
    {
        return {};
    }

    std::wstring result(size, L'\0');
    MultiByteToWideChar(code_page, flags, src.data(), static_cast<int>(src.size()), result.data(), size);
    return result;
}

std::string wide_to_multibyte(UINT code_page, std::wstring const &src)
{
    if (src.empty())
    {
        return {};
    }

    int const size =
        WideCharToMultiByte(code_page, 0, src.data(), static_cast<int>(src.size()), nullptr, 0, nullptr, nullptr);
    if (size <= 0)
    {
        return {};
    }

    std::string result(size, '\0');
    WideCharToMultiByte(code_page, 0, src.data(), static_cast<int>(src.size()), result.data(), size, nullptr, nullptr);
    return result;
}
} // namespace
#endif

std::string acp_to_utf8(std::string const codepage_str)
{
#ifdef _WIN32
    auto ACP = GetACP();
    if (ACP == CP_UTF8 || codepage_str.empty())
    {
        return codepage_str;
    }

    auto utf16_str = multibyte_to_wide(CP_ACP, MB_COMPOSITE, codepage_str);
    auto utf8_str = wide_to_multibyte(CP_UTF8, utf16_str);
    return utf8_str.empty() ? codepage_str : utf8_str;
#else
    return codepage_str;
#endif
}

std::string utf8_to_acp(std::string const src)
{
#ifdef _WIN32
    auto ACP = GetACP();
    if (ACP == CP_UTF8 || src.empty())
    {
        return src;
    }

    auto utf16_str = multibyte_to_wide(CP_UTF8, 0, src);
    auto acp_str = wide_to_multibyte(ACP, utf16_str);
    return acp_str.empty() ? src : acp_str;
#else
    return src;
#endif
}
