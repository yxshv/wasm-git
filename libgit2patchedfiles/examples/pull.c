#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <git2.h>

void print_error(const char *action, int error_code) {
    const git_error *error = giterr_last();
    fprintf(stderr, "%s failed with error %d: %s\n", action, error_code, error->message);
}

int lg2_pull(git_repository *repo, int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <repository_path> <branch_name>\n", argv[0]);
        return EXIT_FAILURE;
    }

		const char *repository_path = git_repository_path(repo);

    // Get the currently checked-out branch
    git_reference *head_ref = NULL;
    if (git_repository_head(&head_ref, repo) != GIT_OK) {
        print_error("git_repository_head", -1);
        git_repository_free(repo);
        git_libgit2_shutdown();
        return EXIT_FAILURE;
    }

    const char *branch_name = git_reference_name(head_ref);


    git_libgit2_init();

    // Open the repository
    if (repo == NULL && git_repository_open(&repo, repository_path) != GIT_OK) {
        print_error("git_repository_open", -1);
        git_libgit2_shutdown();
        return EXIT_FAILURE;
    }

    // Fetch changes from the remote repository
    git_remote *remote = NULL;
    if (git_remote_lookup(&remote, repo, "origin") != GIT_OK) {
        print_error("git_remote_lookup", -1);
        git_repository_free(repo);
        git_libgit2_shutdown();
        return EXIT_FAILURE;
    }

    git_fetch_options fetch_options = GIT_FETCH_OPTIONS_INIT;
    if (git_remote_fetch(remote, NULL, &fetch_options, NULL) != GIT_OK) {
        print_error("git_remote_fetch", -1);
        git_remote_free(remote);
        git_repository_free(repo);
        git_libgit2_shutdown();
        return EXIT_FAILURE;
    }

    // Get the local branch
    git_reference *local_branch = NULL;
    if (git_branch_lookup(&local_branch, repo, branch_name, GIT_BRANCH_LOCAL) != GIT_OK) {
        print_error("git_branch_lookup", -1);
        git_remote_free(remote);
        git_repository_free(repo);
        git_libgit2_shutdown();
        return EXIT_FAILURE;
    }

    // Get the upstream branch
    git_reference *upstream_branch = NULL;
    if (git_branch_upstream(&upstream_branch, local_branch) != GIT_OK) {
        print_error("git_branch_upstream", -1);
        git_reference_free(local_branch);
        git_remote_free(remote);
        git_repository_free(repo);
        git_libgit2_shutdown();
        return EXIT_FAILURE;
    }

    // Merge changes from the upstream branch
    git_merge_options merge_options = GIT_MERGE_OPTIONS_INIT;
    if (git_merge(repo, upstream_branch, &merge_options, NULL) != GIT_OK) {
        if (git_index_has_conflicts(git_repository_index(repo))) {
            printf("Merge conflicts detected. Please resolve conflicts and commit.\n");
        } else {
            print_error("git_merge", -1);
        }

        git_reference_free(upstream_branch);
        git_reference_free(local_branch);
        git_remote_free(remote);
        git_repository_free(repo);
        git_libgit2_shutdown();
        return EXIT_FAILURE;
    }

    // Clean up
    git_reference_free(upstream_branch);
    git_reference_free(local_branch);
    git_remote_free(remote);

    // If repo was opened inside this function, free it
    if (argv[1] != NULL) {
        git_repository_free(repo);
    }

    git_libgit2_shutdown();

    printf("Pull successful.\n");
    return EXIT_SUCCESS;
}

