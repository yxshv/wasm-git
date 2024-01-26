#include <git2.h>
#include <stdio.h>

int pull(git_repository *repo) {
    int error = 0;
    git_remote *remote = NULL;
    git_fetch_options fetch_opts = GIT_FETCH_OPTIONS_INIT;
    git_merge_options merge_opts = GIT_MERGE_OPTIONS_INIT;
    const char *remote_url = NULL;
    const char *remote_branch = NULL;
    const char *local_branch = NULL;

    // Get the current local branch
    error = git_repository_head(&remote_branch, repo);
    check_error(error, "Failed to get current branch");

    // Get the remote of the current branch
    error = git_branch_upstream(&remote, repo, remote_branch);
    if (error == GIT_ENOTFOUND) {
        printf("Current branch has no upstream branch.\n");
        return 1;
    }
    check_error(error, "Failed to get upstream remote");

    // Get the remote URL and remote branch name
    error = git_remote_url(&remote_url, remote);
    check_error(error, "Failed to get remote URL");
    error = git_branch_name(&remote_branch, remote_branch);
    check_error(error, "Failed to get remote branch name");

    // Use the local branch name as is
    local_branch = remote_branch;    


		// Look up the remote
    error = git_remote_lookup(&remote, repo, remote_url);
    check_error(error, "Failed to find remote");

    // Fetch from the remote
    error = git_remote_fetch(remote, NULL, &fetch_opts, NULL);
    check_error(error, "Failed to fetch from remote");

    // Merge the remote branch into the local branch
    error = git_merge(repo, &merge_opts, (const git_annotated_commit **) &remote_branch, 1);
		error = git_merge(repo, (const git_annotated_commit **), &remote_branch, 1 );

    // Handle merge scenarios
    if (error == GIT_MERGE_ANALYSIS_UP_TO_DATE) {
        printf("Local branch is up-to-date with remote branch.\n");
    } else if (error == GIT_MERGE_ANALYSIS_FASTFORWARD) {
        printf("Fast-forward merge successful.\n");
    } else if (error == GIT_MERGE_ANALYSIS_NORMAL || error == GIT_MERGE_NO_RECURSIVE) {
        printf("Recursive merge successful.\n");
    } else {
        // Handle merge errors here (not implemented yet)
        check_error(error, "Merge failed");
    }

cleanup:
    git_remote_free(remote);
    git_repository_free(repo);
    return error;
}

void check_error(int error, const char *message) {
    if (error != 0) {
        fprintf(stderr, "%s: %s\n", message, giterr_last());
        exit(1);
    }
}

int lg2_pull(git_repository *repo,int argc, char **argv) {
    return pull(repo);
}

