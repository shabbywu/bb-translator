#include "head.h"
#include "common.h"
#include <format>
#include <functional>
#include <git2.h>

std::string git_head_digest(AppState *state)
{
    git_repository *repo;
    git_reference *head;
    const git_oid *oid;
    int error;

    char head_digist[GIT_OID_SHA1_HEXSIZE + 1];
    if (error = git_repository_open(&repo, state->i18nProjectDir.string().c_str()) < 0)
        goto on_error;
    if (error = git_repository_head(&head, repo) < 0)
        goto on_error;
    oid = git_reference_target(head);
    git_oid_fmt(head_digist, oid);
    head_digist[GIT_OID_SHA1_HEXSIZE] = '\0';
    return head_digist;
on_error:
    print_git_error(error);
    git_reference_free(head);
    git_repository_free(repo);
    return "";
}
