#pragma once
#include "../ui/AppState.h"
#include <filesystem>

void setup_python(AppState *state);
void start_python_daemon(AppState *state);
void dispatch_translator(AppState *state);
void shutdown_python_daemon(AppState *state);

void sync_translate(AppState *state);
