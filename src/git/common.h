#pragma once
#include <format>
#include <git2.h>

#ifndef PRIuZ
/* Define the printf format specifier to use for size_t output */
#if defined(_MSC_VER) || defined(__MINGW32__)
#define PRIuZ "Iu"
#else
#define PRIuZ "zu"
#endif
#endif

static AppState *g_state;

typedef struct progress_data
{
    git_indexer_progress fetch_progress;
    size_t completed_steps;
    size_t total_steps;
    const char *path;
} progress_data;

static char PRINT_BUFFER[1024];
static void print_progress(const progress_data *pd)
{
    int network_percent = pd->fetch_progress.total_objects > 0
                              ? (100 * pd->fetch_progress.received_objects) / pd->fetch_progress.total_objects
                              : 0;
    int index_percent = pd->fetch_progress.total_objects > 0
                            ? (100 * pd->fetch_progress.indexed_objects) / pd->fetch_progress.total_objects
                            : 0;

    int checkout_percent = pd->total_steps > 0 ? (int)((100 * pd->completed_steps) / pd->total_steps) : 0;
    size_t kbytes = pd->fetch_progress.received_bytes / 1024;

    if (pd->fetch_progress.total_objects && pd->fetch_progress.received_objects == pd->fetch_progress.total_objects)
    {
        sprintf(PRINT_BUFFER, "Resolving deltas %u/%u\r", pd->fetch_progress.indexed_deltas,
                pd->fetch_progress.total_deltas);
        g_state->addLog(PRINT_BUFFER);
    }
    else
    {
        sprintf(PRINT_BUFFER,
                "net %3d%% (%4" PRIuZ " kb, %5u/%5u)  /  idx %3d%% (%5u/%5u)  /  chk %3d%% (%4" PRIuZ "/%4" PRIuZ
                ")%s\n",
                network_percent, kbytes, pd->fetch_progress.received_objects, pd->fetch_progress.total_objects,
                index_percent, pd->fetch_progress.indexed_objects, pd->fetch_progress.total_objects, checkout_percent,
                pd->completed_steps, pd->total_steps, pd->path);
        g_state->addLog(PRINT_BUFFER);
    }
}

static int sideband_progress(const char *str, int len, void *payload)
{
    (void)payload; /* unused */

    g_state->addLog(std::format("remote: {}", str));
    return 0;
}

/**
 * This gets called during the download and indexing. Here we show
 * processed and total objects in the pack and the amount of received
 * data. Most frontends will probably want to show a percentage and
 * the download rate.
 */
static int fetch_progress(const git_indexer_progress *stats, void *payload)
{
    progress_data *pd = (progress_data *)payload;
    pd->fetch_progress = *stats;
    print_progress(pd);
    return 0;
}

static void checkout_progress(const char *path, size_t cur, size_t tot, void *payload)
{
    progress_data *pd = (progress_data *)payload;
    pd->completed_steps = cur;
    pd->total_steps = tot;
    pd->path = path;
    print_progress(pd);
}

static inline void print_git_error(int error)
{
    const git_error *err = git_error_last();
    if (err)
    {
        g_state->addLog(std::format("[error] {}: {}\n", err->klass, err->message));
    }
    else
    {
        g_state->addLog(std::format("[error] {}: no detailed info\n", error));
    }
}
