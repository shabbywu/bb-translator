#pragma once
#include "Sources.h"
#include <unordered_map>

namespace SourceParse
{
AnnotatedSource ReadImGuiDemoCode();
AnnotatedSource ReadImGuiDemoCodePython();
std::unordered_map<std::string, SourceCode> FindExampleAppsCode();
} // namespace SourceParse
