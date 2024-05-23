#include "SettingsWindow.h"
#include <frozen/unordered_map.h>
#include <imgui.h>
#include <hello_imgui/hello_imgui.h>

static constexpr enum MessageID
{
    MsgGitAdvanceSettingTitle,
    // Git Repo
    MsgGitRepoChoiceTitle,
    MsgGitRepoChoiceDirect,
    MsgGitRepoChoice__ghproxy,
    MsgGitRepoChoice__gitclone,
    MsgGitRepoChoice__tencentcloud,
    // HttpProxy
    MsgGitHTTPProxy,
    MsgGitHTTPProxyHelpTooltip,
    // trick to get enum size
    MsgEnumTotalSize,
};

#define _(MessageID) i18nMessage.at(state.lang).at(MessageID)

static constexpr frozen::unordered_map<LangType, frozen::unordered_map<MessageID, const char *, MsgEnumTotalSize>, 2>
    i18nMessage = {
        {LangCN,
         {
             {MsgGitAdvanceSettingTitle, "Git 高级配置"},
             // 仓库源配置
             {MsgGitRepoChoiceTitle, "Git 仓库源"},
             {MsgGitRepoChoiceDirect, "Github 直连"},
             {MsgGitRepoChoice__ghproxy, "ghproxy 代理源"},
             {MsgGitRepoChoice__gitclone, "gitclone 代理源"},
             {MsgGitRepoChoice__tencentcloud, "边缘加速代理源"},
             // HTTP Proxy
             {MsgGitHTTPProxy, "HTTP 代理(?)"},
             {MsgGitHTTPProxyHelpTooltip, R"(使用 HTTP 代理服务器连接 Git 仓库源。配置示例: `http://127.0.0.1:8080`

填写该配置前，请确保你清楚什么是 HTTP 代理。)"},
         }},
        {LangEN,
         {
             {MsgGitAdvanceSettingTitle, "Git AdvanceConfig"},
             // 仓库源配置
             {MsgGitRepoChoiceTitle, "Git Repo"},
             {MsgGitRepoChoiceDirect, "Github"},
             {MsgGitRepoChoice__ghproxy, "ghproxy proxy"},
             {MsgGitRepoChoice__gitclone, "gitclone proxy"},
             {MsgGitRepoChoice__tencentcloud, "edgeone proxy"},
             // Http Proxy
             {MsgGitHTTPProxy, "HTTP Proxy(?)"},
             {MsgGitHTTPProxyHelpTooltip, R"(Use HTTP Proxy Server to connect git repo. Example: `http://127.0.0.1:8080`

Please ensure you know WHAT is HTTP Proxy before you fill anything in here.)"},
         }}};

static constexpr frozen::unordered_map<MessageID, const char *, 4> i18nProjectGitUrls = {
    {MsgGitRepoChoiceDirect, "https://github.com/shabbywu/Battle-Brothers-CN.git"},
    {MsgGitRepoChoice__ghproxy, "https://mirror.ghproxy.com/https://github.com/shabbywu/Battle-Brothers-CN.git"},
    {MsgGitRepoChoice__gitclone, "https://gitclone.com/github.com/shabbywu/Battle-Brothers-CN.git"},
    {MsgGitRepoChoice__tencentcloud, "https://ghproxy.shabbywu.cn/github.com/shabbywu/Battle-Brothers-CN.git"},
};

void SettingsWindow::gui(AppState &state)
{
    static MessageID i18nProjectChoiceSelected = std::find_if(i18nProjectGitUrls.begin(), i18nProjectGitUrls.end(),
                                                              [&state](std::pair<MessageID, const char *> pair) {
                                                                  return state.i18nProjectGitUrl == pair.second;
                                                              })
                                                     ->first;
    static const char *i18nProjectPreview = _(i18nProjectChoiceSelected);
    static ImGuiComboFlags flags = 0;

    ImGui::SeparatorText(_(MsgGitAdvanceSettingTitle));
    if (ImGui::BeginCombo(_(MsgGitRepoChoiceTitle), i18nProjectPreview, flags))
    {
        for (auto &[key, value] : i18nProjectGitUrls)
        {
            auto isSelected = key == i18nProjectChoiceSelected;
            if (ImGui::Selectable(_(key), isSelected))
            {
                i18nProjectChoiceSelected = key;
                i18nProjectPreview = _(i18nProjectChoiceSelected);
                state.i18nProjectGitUrl = i18nProjectGitUrls.at(i18nProjectChoiceSelected);
            }
            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (key == i18nProjectChoiceSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::InputText(_(MsgGitHTTPProxy), state.httpProxyUrl, 256, 0);
    ImGui::SetItemTooltip(_(MsgGitHTTPProxyHelpTooltip));
};
