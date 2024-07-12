#include "clone.h"
#include "common.h"
#include <functional>
#include <git2.h>

bool git_clone_repo(AppState *state)
{
    g_state = state;
    progress_data pd = {{0}};

    git_repository *cloned_repo;
    git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
    checkout_opts.checkout_strategy = GIT_CHECKOUT_FORCE;
    checkout_opts.progress_cb = checkout_progress;
    checkout_opts.progress_payload = &pd;

    git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;
    clone_opts.checkout_opts = checkout_opts;
    clone_opts.checkout_branch = "main";

    clone_opts.fetch_opts.callbacks.sideband_progress = sideband_progress;
    clone_opts.fetch_opts.callbacks.transfer_progress = fetch_progress;
    clone_opts.fetch_opts.callbacks.payload = &pd;
    clone_opts.fetch_opts.proxy_opts = GIT_PROXY_OPTIONS_INIT;
    if (strlen(state->httpProxyUrl) > 0)
    {
        clone_opts.fetch_opts.proxy_opts.type = GIT_PROXY_SPECIFIED;
        clone_opts.fetch_opts.proxy_opts.url = state->httpProxyUrl;
        state->addLog(std::format("clone with http proxy: {}", state->httpProxyUrl));
    }

    git_repository_state_t repository_state;
    git_reference *ref = NULL, *branch = NULL;
    git_commit *target_commit = NULL;

    g_state->addLog("正在克隆仓库...");
    int error = git_clone(&cloned_repo, g_state->i18nProjectGitUrl.c_str(), g_state->i18nProjectDir.string().c_str(),
                          &clone_opts);
    if (error != 0)
        print_git_error(error);

    if (cloned_repo)
    {
        git_reference *head;
        error = git_repository_head(&head, cloned_repo);
        if (error)
            print_git_error(error);
        else
        {
            char head_digist[GIT_OID_SHA1_HEXSIZE + 1];
            auto oid = git_reference_target(head);
            git_oid_fmt(head_digist, oid);
            head_digist[GIT_OID_SHA1_HEXSIZE] = '\0';
            g_state->addLog(std::format("当前汉化版本: {}", head_digist));
            git_reference_free(head);
        }
        git_repository_free(cloned_repo);
    }
    return error == 0;
}
