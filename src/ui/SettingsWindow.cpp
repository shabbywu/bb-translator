#include "SettingsWindow.h"
#include <frozen/unordered_map.h>

static constexpr enum MessageID
{
    MsgGitSettingTitle,
    // trick to get enum size
    MsgEnumTotalSize,
};

#define _(MessageID) i18nMessage.at(state->lang).at(MessageID)

static constexpr frozen::unordered_map<LangType, frozen::unordered_map<MessageID, const char *, MsgEnumTotalSize>, 2>
    i18nMessage = {{LangCN, {{MsgGitSettingTitle, "Git源"}}}, {LangEN, {{MsgGitSettingTitle, "Git源"}}}};

void SettingsWindow::gui(AppState &state) {

};
