#pragma once
#include <chrono>
#include <iostream>

namespace debug
{
struct time_guard
{
    std::string scope;
    time_guard(std::string scope = "")
    {
        start = std::chrono::steady_clock::now();
        this->scope = scope;
    }
    ~time_guard()
    {
        if (!scope.empty())
        {
            std::cout << "[" << scope << "]";
        }
        std::cout
            << "Cost Time: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count()
            << "[ms]\n";
    }
    std::chrono::steady_clock::time_point start;
};
} // namespace debug
