#include "pull.h"
#include "common.h"
#include <format>
#include <functional>
#include <git2.h>

/**
 * This function gets called for each remote-tracking branch that gets
 * updated. The message we output depends on whether it's a new one or
 * an update.
 */
static int update_cb(const char *refname, const git_oid *a, const git_oid *b, void *data)
{
    char a_str[GIT_OID_SHA1_HEXSIZE + 1], b_str[GIT_OID_SHA1_HEXSIZE + 1];
    (void)data;

    git_oid_fmt(b_str, b);
    b_str[GIT_OID_SHA1_HEXSIZE] = '\0';

    if (git_oid_is_zero(a))
    {
        g_state->addLog(std::format("[new]     {} {}", b_str, refname));
    }
    else
    {
        git_oid_fmt(a_str, a);
        a_str[GIT_OID_SHA1_HEXSIZE] = '\0';
        g_state->addLog(std::format("[updated] {}..{} {}", a_str, b_str, refname));
    }
    return 0;
}

static int force_checkout(git_repository *repo, git_remote *remote)
{
    auto remote_name = git_remote_name(remote);
    auto reference_name = std::format("refs/remotes/{}/main", remote_name);

    progress_data pd = {{0}};
    git_oid head_oid;
    git_commit *commit;
    git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
    checkout_opts.checkout_strategy = GIT_CHECKOUT_FORCE;
    checkout_opts.progress_cb = checkout_progress;
    checkout_opts.progress_payload = &pd;

    // 获取远程的 head commit
    g_state->addLog(std::format("Checkout to '{}'...", reference_name));
    if (git_reference_name_to_id(&head_oid, repo, reference_name.c_str()) < 0)
        return -1;
    if (git_object_lookup((git_object **)&commit, repo, &head_oid, GIT_OBJ_COMMIT) < 0)
        return -1;
    if (git_checkout_tree(repo, (git_object *)commit, &checkout_opts) < 0)
        return -1;
    if (git_repository_set_head_detached(repo, &head_oid) < 0)
        return -1;

    char head_digist[GIT_OID_SHA1_HEXSIZE + 1];
    git_oid_fmt(head_digist, &head_oid);
    g_state->addLog(std::format("[*] Checkout: {}", head_digist));
    return 0;
}

static int fetch_remote(git_repository *repo, git_remote *remote, const char *http_proxy_url)
{
    const git_indexer_progress *stats;
    git_fetch_options fetch_opts = GIT_FETCH_OPTIONS_INIT;
    /* Set up the callbacks (only update_tips for now) */
    progress_data pd = {{0}};

    fetch_opts.proxy_opts = GIT_PROXY_OPTIONS_INIT;
    if (strlen(http_proxy_url))
    {
        fetch_opts.proxy_opts.type = GIT_PROXY_SPECIFIED;
        fetch_opts.proxy_opts.url = http_proxy_url;
        g_state->addLog(std::format("fetch with http proxy: {}", g_state->httpProxyUrl));
    }
    fetch_opts.callbacks.update_tips = update_cb;
    fetch_opts.callbacks.sideband_progress = sideband_progress;
    fetch_opts.callbacks.transfer_progress = fetch_progress;
    fetch_opts.callbacks.payload = &pd;

    /**
     * Perform the fetch with the configured refspecs from the
     * config. Update the reflog for the updated references with
     * "fetch".
     */
    if (git_remote_fetch(remote, NULL, &fetch_opts, "fetch") < 0)
        return -1;

    /**
     * If there are local objects (we got a thin pack), then tell
     * the user how many objects we saved from having to cross the
     * network.
     */
    stats = git_remote_stats(remote);
    if (stats->local_objects > 0)
    {
        g_state->addLog(std::format("Received {}/{} objects in {} bytes (used {} local objects)",
                                    stats->indexed_objects, stats->total_objects, stats->received_bytes,
                                    stats->local_objects));
    }
    else
    {
        g_state->addLog(std::format("rReceived {}/{} objects in {} bytes", stats->indexed_objects, stats->total_objects,
                                    stats->received_bytes));
    }
    return 0;
}

bool git_force_update(AppState *state)
{
    g_state = state;
    std::string &gh = state->i18nProjectGitUrl;
    git_repository *repo;
    git_remote *remote;
    bool status;
    int error;

    error = git_repository_open(&repo, state->i18nProjectDir.string().c_str());
    if (error)
        goto on_error;

    g_state->addLog("Preparing...");
    if (error = git_remote_set_url(repo, "origin", gh.c_str()) < 0)
        goto on_error;
    if (error = git_remote_lookup(&remote, repo, "origin") < 0)
        goto on_error;

    g_state->addLog(std::format("Fetching {}...", gh));
    if (error = fetch_remote(repo, remote, state->httpProxyUrl) < 0)
        goto on_error;
    g_state->addLog(std::format("[*] Fetched {}", gh));

    if (error = force_checkout(repo, remote) < 0)
        goto on_error;

    git_remote_free(remote);
    git_repository_free(repo);
    return true;
on_error:
    print_git_error(error);
    git_remote_free(remote);
    git_repository_free(repo);
    return false;
}
