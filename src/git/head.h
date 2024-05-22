#pragma once
#include "../ui/AppState.h"
#include <filesystem>
#include <string>

// git_head_digest: 获取 git head 的数字摘要
std::string git_head_digest(AppState *state);
