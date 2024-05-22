#include "UsageWindow.h"
#include "imgui_utilities/MarkdownHelper.h"
#include <frozen/unordered_map.h>
#include <hello_imgui/hello_imgui.h>

static constexpr enum MessageID {
  MsgUsage,
  // trick to get enum size
  MsgEnumTotalSize,
};

#define _(MessageID) i18nMessage.at(state.lang).at(MessageID)

static constexpr frozen::unordered_map<
    LangType, frozen::unordered_map<MessageID, const char *, MsgEnumTotalSize>,
    2>
    i18nMessage = {
        {LangCN,
         {{MsgUsage,
           R"(# 使用说明
1. 设置游戏根目录 (通常是 X:\Program Files (x86)\Steam\steamapps\common\Battle Brothers\ 目录)
2. 初始化翻译项目或按需更新翻译项目 (翻译项目会不定时更新翻译进度)
3. 点击「翻译文本」

___
[)" ICON_FA_BELL
           R"(如遇到使用问题, 欢迎反馈!](https://github.com/shabbywu/bb-translator/issues) | [)" ICON_FA_THUMBS_UP
           R"( Github 关注](https://github.com/shabbywu/bb-translator)
)"}}},
        {LangEN,
         {{MsgUsage,
           R"(# Usage
1. Set the game root directory (usually the X:\Program Files (x86)\Steam\steamapps\common\Battle Brothers\ directory)
2. Set the output path for text extraction (for example, X:\Program Files (x86)\Steam\steamapps\common\Battle Brothers\localization\)
3. Click "Extract text"
___
[)" ICON_FA_BELL
           R"(Feedback](https://github.com/shabbywu/bb-translator/issues) | [)" ICON_FA_THUMBS_UP
           R"( Github Star](https://github.com/shabbywu/bb-translator)
)"}}}};

void UsageWindow::gui(AppState &state) {
  MarkdownHelper::Markdown(_(MsgUsage));
}
