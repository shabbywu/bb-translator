#include "OperationsWindow.h"
#include "../bb-translator/binding.h"
#include "../git/clone.h"
#include "../git/head.h"
#include "../git/pull.h"
#include "imgui_utilities/ImGuiExt.h"
#include "imgui_utilities/imFileDialog.h"
#include <chrono>
#include <frozen/unordered_map.h>
#include <hello_imgui/hello_imgui.h>
#include <memory>
#include <thread>

using namespace std::chrono_literals;

static enum GitTaskStatus
{
    Pending,
    Running,
};

static enum I18NProjectStatus
{
    Unknown,
    Missing,
    Existed,
};

static constexpr enum MessageID
{
    // Message Group for Git
    MsgGitCloneSuccessed,
    MsgGitCloneFailed,
    MsgGitFetchSuccessed,
    MsgGitFetchFailed,

    // Message Group for bb-translator
    MsgSelectGameFolderButton,
    MsgSelectGameFolder,
    MsgGameFolderPrefix,

    MsgMissingI18nProjectError,
    MsgInitI18nProjectButton,
    MsgUpdateI18nProjectButton,
    MsgI18nProjectButtonTooltip,
    MsgI18nProjectVersionTmpl,

    MsgSelectI18nJsonFolderButton,
    MsgSelectI18nJsonFolder,
    MsgI18nJsonFolderPrefix,

    MsgDoTranslateButton,
    MsgDoTranslateButtonTooltip,
    //
    MsgTaskRunningError,
    // trick to get enum size
    MsgEnumTotalSize,
};

#define _(MessageID) i18nMessage.at(state->lang).at(MessageID)

static constexpr frozen::unordered_map<LangType, frozen::unordered_map<MessageID, const char *, MsgEnumTotalSize>, 2>
    i18nMessage = {{
                       LangCN,
                       {
                           {MsgGitCloneSuccessed, "[*] 克隆仓库成功!"},
                           {
                               MsgGitCloneFailed,
                               "[x] 克隆仓库失败!",
                           },
                           {
                               MsgGitFetchSuccessed,
                               "[*] 更新仓库成功!",
                           },
                           {
                               MsgGitFetchFailed,
                               "[x] 更新仓库失败!!",
                           },
                           {
                               MsgSelectGameFolderButton,
                               ICON_FA_FILE " 选择游戏根目录",
                           },
                           {
                               MsgSelectGameFolder,
                               "选择游戏根目录",
                           },
                           {
                               MsgGameFolderPrefix,
                               "游戏根目录: ",
                           },
                           {
                               MsgMissingI18nProjectError,
                               "请先初始化翻译项目！",
                           },
                           {
                               MsgInitI18nProjectButton,
                               ICON_FA_FILE " 初始化翻译项目",
                           },
                           {
                               MsgUpdateI18nProjectButton,
                               ICON_FA_FILE " 更新翻译项目",
                           },
                           {
                               MsgI18nProjectButtonTooltip,
                               "如初始化或更新翻译项目失败, 请尝试切换 Git 仓库源。",
                           },
                           {MsgI18nProjectVersionTmpl, "文本版本: %.20s"},
                           {MsgSelectI18nJsonFolderButton, ICON_FA_FILE " Select i18n json directory"},
                           {
                               MsgSelectI18nJsonFolder,
                               "Select i18n json directory",
                           },
                           {MsgI18nJsonFolderPrefix, "文本目录: "},
                           {
                               MsgDoTranslateButton,
                               ICON_FA_PLAY " 翻译文本",
                           },
                           {MsgDoTranslateButtonTooltip, "请先设置游戏根目录和文本目录"},
                           {
                               MsgTaskRunningError,
                               "请等待上一个任务执行完毕",
                           },
                       },
                   },
                   {LangEN,
                    {
                        {MsgGitCloneSuccessed, "[*] 克隆仓库成功!"},
                        {
                            MsgGitCloneFailed,
                            "[x] 克隆仓库失败!",
                        },
                        {
                            MsgGitFetchSuccessed,
                            "[*] 更新仓库成功!",
                        },
                        {
                            MsgGitFetchFailed,
                            "[x] 更新仓库失败!!",
                        },
                        {
                            MsgSelectGameFolderButton,
                            ICON_FA_FILE " Select game root directory",
                        },
                        {
                            MsgSelectGameFolder,
                            "Select game root directory",
                        },
                        {
                            MsgGameFolderPrefix,
                            "Game Root Directory: ",
                        },
                        {
                            MsgMissingI18nProjectError,
                            "请先初始化翻译项目！",
                        },
                        {
                            MsgInitI18nProjectButton,
                            ICON_FA_FILE " 初始化翻译项目",
                        },
                        {
                            MsgUpdateI18nProjectButton,
                            ICON_FA_FILE " 更新翻译项目",
                        },
                        {
                            MsgI18nProjectButtonTooltip,
                            "如初始化或更新翻译项目失败, 请尝试切换 Git 仓库源。",
                        },
                        {MsgI18nProjectVersionTmpl, "文本版本: %.20s"},
                        {MsgSelectI18nJsonFolderButton, ICON_FA_FILE " Select i18n json directory"},
                        {
                            MsgSelectI18nJsonFolder,
                            "Select i18n json directory",
                        },
                        {MsgI18nJsonFolderPrefix, "i18n json Directory: "},
                        {
                            MsgDoTranslateButton,
                            ICON_FA_PLAY " Tranlate text",
                        },
                        {MsgDoTranslateButtonTooltip, "Please set the game root directory "
                                                      "and i18n json directory first"},
                        {
                            MsgTaskRunningError,
                            "请等待上一个任务执行完毕",
                        },
                    }}};

void OperationsWindow::gui()
{
    static GitTaskStatus gitTaskStatus = GitTaskStatus::Pending;
    static I18NProjectStatus i18nProjectStatus =
        state->i18nJSONDir == "" ? (state->gameDir == "" ? I18NProjectStatus::Unknown : I18NProjectStatus::Missing)
                                 : I18NProjectStatus::Existed;
    static std::string i18nProjectVersion = state->i18nJSONDir == "" ? "" : git_head_digest(state);

    auto cloneCNProject = [this]() {
        if (git_clone_repo(state))
        {
            state->addLog(_(MsgGitCloneSuccessed));
            state->i18nJSONDir = state->i18nProjectDir / "zh_CN.UTF-8" / "json";
            i18nProjectVersion = git_head_digest(state);
            i18nProjectStatus = I18NProjectStatus::Existed;
        }
        else
        {
            state->addLog(_(MsgGitCloneFailed));
        }
        gitTaskStatus = GitTaskStatus::Pending;
    };

    auto fetchCNProject = [this]() {
        if (git_force_update(state))
        {
            state->addLog(_(MsgGitFetchSuccessed));
            i18nProjectVersion = git_head_digest(state);
        }
        else
        {
            state->addLog(_(MsgGitFetchFailed));
        }
        gitTaskStatus = GitTaskStatus::Pending;
    };

    if (ImGui::Button(_(MsgSelectGameFolderButton), ImVec2(0, 40)))
        ifd::FileDialog::Instance().Open("SelectGameFolder", _(MsgSelectGameFolder), "");

    if (state->gameDir.string().length() > 0)
    {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), _(MsgGameFolderPrefix));
        ImGuiExt::SameLine_IfPossible(10);
        ImGui::TextWrapped(state->gameDir.string().c_str());
    }

    if (i18nProjectStatus == I18NProjectStatus::Missing)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), _(MsgMissingI18nProjectError));
    }

    if (state->lang == LangCN)
    {
        // 初始化中文本地化项目的按钮
        ImGui::BeginDisabled(i18nProjectStatus == I18NProjectStatus::Unknown);
        if (i18nProjectStatus != I18NProjectStatus::Existed)
        {
            if (ImGui::Button(_(MsgInitI18nProjectButton), ImVec2(0, 40)))
            {
                if (gitTaskStatus == GitTaskStatus::Pending)
                {
                    gitTaskStatus = GitTaskStatus::Running;
                    std::thread(cloneCNProject).detach();
                }
                else
                {
                    state->addLog(_(MsgTaskRunningError));
                }
            }
            ImGui::SetItemTooltip(_(MsgI18nProjectButtonTooltip));
        }
        else
        {
            if (ImGui::Button(_(MsgUpdateI18nProjectButton), ImVec2(0, 40)))
            {
                if (gitTaskStatus == GitTaskStatus::Pending)
                {
                    gitTaskStatus = GitTaskStatus::Running;
                    std::thread(fetchCNProject).detach();
                }
                else
                {
                    state->addLog(_(MsgTaskRunningError));
                }
            }
            ImGui::SetItemTooltip(_(MsgI18nProjectButtonTooltip));
            ImGui::TextColored(ImVec4(0.6875f, 0.621f, 0.539f, 1.0f), _(MsgI18nProjectVersionTmpl),
                               i18nProjectVersion.c_str());
        }
        ImGui::EndDisabled();
    }
    else
    {
        // 选择外语本地化项目的按钮
        if (ImGui::Button(_(MsgSelectI18nJsonFolderButton), ImVec2(0, 40)))
            ifd::FileDialog::Instance().Open("SelectI18nJsonFolder", _(MsgSelectI18nJsonFolder), "");
    }

    if (state->i18nJSONDir.string().length() > 0)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), _(MsgI18nJsonFolderPrefix));
        ImGuiExt::SameLine_IfPossible(10);
        ImGui::TextWrapped(state->i18nJSONDir.string().c_str());
    }

    // 渲染 “翻译文本” 按钮
    {
        bool enabled = state->gameDir.string().length() > 0 && state->i18nJSONDir.string().length() > 0;
        if (ImGuiExt::Button_WithEnabledFlag(_(MsgDoTranslateButton), enabled, _(MsgDoTranslateButtonTooltip), false))
        {
            dispatch_translator(state);
        }
    }

    // callback for FileDialog
    if (ifd::FileDialog::Instance().IsDone("SelectGameFolder"))
    {
        if (ifd::FileDialog::Instance().HasResult())
        {
            state->gameDir = ifd::FileDialog::Instance().GetResult();
            state->i18nProjectDir = state->gameDir / "Battle-Brothers-CN";
            state->i18nJSONDir = state->i18nProjectDir / "zh_CN.UTF-8" / "json";

            if (!std::filesystem::exists(state->i18nJSONDir))
            {
                state->i18nJSONDir = "";
                i18nProjectStatus = I18NProjectStatus::Missing;
            }
            else
            {
                i18nProjectStatus = I18NProjectStatus::Existed;
                i18nProjectVersion = git_head_digest(state);
            }
        }
        ifd::FileDialog::Instance().Close();
    }

    if (ifd::FileDialog::Instance().IsDone("SelectI18nJsonFolder"))
    {
        if (ifd::FileDialog::Instance().HasResult())
        {
            state->i18nJSONDir = ifd::FileDialog::Instance().GetResult();
        }
        ifd::FileDialog::Instance().Close();
    }
}
