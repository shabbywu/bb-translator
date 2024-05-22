#pragma once
#include "../ui/AppState.h"
#include <filesystem>

// git_force_fetch: 更新汉化项目
bool git_force_update(AppState *state, std::string gh);
