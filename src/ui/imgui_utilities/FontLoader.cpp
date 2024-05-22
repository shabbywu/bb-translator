#include "FontLoader.h"

namespace FontLoader {
static ImFont *gDefaultFont = nullptr;

void LoadDefaultFont() {
  float fontSize = 15.f;
  std::string fontFilename = "fonts/font.ttf";
  gDefaultFont = HelloImGui::LoadFontTTF_WithFontAwesomeIcons(fontFilename,
                                                              fontSize, true);
}

ImFont *GetDefaultFont() { return gDefaultFont; }
} // namespace FontLoader
