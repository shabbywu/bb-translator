#pragma once
#include "../ui/AppState.h"
#include <filesystem>

// git_clone_repo: 克隆汉化项目到游戏目录
bool git_clone_repo(AppState *state, std::string gh);
