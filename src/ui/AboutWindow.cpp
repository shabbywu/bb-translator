#include "AboutWindow.h"
#include "hello_imgui/hello_imgui.h"
#include "imgui_utilities/HyperlinkHelper.h"
#include "imgui_utilities/ImGuiExt.h"
#include "imgui_utilities/MarkdownHelper.h"
#include <frozen/unordered_map.h>

static constexpr enum MessageID {
  MsgHelp,
  MsgUsageButton,
  MsgGithubStarButton,
  // trick to get enum size
  MsgEnumTotalSize,
};

#define _(MessageID) i18nMessage.at(state.lang).at(MessageID)

static constexpr frozen::unordered_map<
    LangType, frozen::unordered_map<MessageID, const char *, MsgEnumTotalSize>,
    2>
    i18nMessage = {{LangCN,
                    {
                        {MsgHelp, R"(
本工具由战场兄弟中文本地化团队开发。
汉化贡献者:
- shabbywu
- Rayforward
- FaelGnahz
- DarkSjm
- 0v0
- 野蛮人神选者

[如对本工具有任何建议, 欢迎反馈](https://github.com/shabbywu/bb-translator/issues)
)"},
                        {MsgUsageButton, ICON_FA_INFO " 查看使用说明"},
                        {MsgGithubStarButton, ICON_FA_THUMBS_UP " Github 关注"},
                    }},
                   {LangEN,
                    {
                        {MsgHelp, R"(
This tool is developed by the Battlefield Brothers Chinese localization team.

[FeedBack](https://github.com/shabbywu/bb-translator/issues)

)"},
                        {MsgUsageButton, ICON_FA_INFO " Usage"},
                        {MsgGithubStarButton, ICON_FA_THUMBS_UP " Github Star"},
                    }}};

void AboutWindow::gui(AppState &state) {
  MarkdownHelper::Markdown(_(MsgHelp));
  if (ImGui::Button(_(MsgUsageButton))) {
    HelloImGui::DockableWindow *usageWindow =
        runnerParams->dockingParams.dockableWindowOfName("使用说明");
    usageWindow->isVisible = true;
  }
  ImGuiExt::SameLine_IfPossible();
  if (ImGui::Button(_(MsgGithubStarButton))) {
    HyperlinkHelper::OpenUrl("https://github.com/shabbywu/bb-translator");
  }
}
