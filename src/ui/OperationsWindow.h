#pragma once
#include "AppState.h"

class OperationsWindow {
public:
  OperationsWindow(AppState *state) {
    this->state = state;
    gameDir = state->gameDir.string();
    destDir = state->i18nJSONDir.string();
  };
  void gui();

  AppState *state;
  std::string gameDir;
  std::string destDir;
};
