#pragma once
#include "AppState.h"
#include <hello_imgui/hello_imgui.h>

class AboutWindow {
public:
  HelloImGui::RunnerParams *runnerParams;
  AboutWindow(HelloImGui::RunnerParams *runnerParams) {
    this->runnerParams = runnerParams;
  }
  void gui(AppState &);
};
