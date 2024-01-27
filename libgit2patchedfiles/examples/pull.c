#include "common.h"

#include <git2.h>
#include <stdio.h>

void check_error(int error, const char *message) {
    if (error != 0) {
        fprintf(stderr, "%s: %s\n", message, git_error_last());
        exit(1);
    }
}

int lg2_pull(git_repository *repo, int argc, char **argv) {

	int error = 0;

	char *_argv[2];
	argv[0] = "fetch";
	argv[1] = "origin";

	lg2_fetch(repo, 2, _argv);


	git_reference *head;
	const char *branch = NULL;

	error = git_repository_head(&head, repo);
	
  if (error == GIT_EUNBORNBRANCH || error == GIT_ENOTFOUND) {
    branch = NULL;
	} else if (!error) {
    branch = git_reference_shorthand(head);

		char *name;

		// Allocate memory for the formatted name
		name = malloc(strlen("origin/") + strlen(branch) + 1);
		if (name == NULL) {
				// Handle memory allocation error
				fprintf(stderr, "Memory allocation failed\n");
				goto cleanup;
		}

		// Create the formatted name
		sprintf(name, "origin/%s", branch);

		char *__argv[2];
		__argv[0] = "merge";
		__argv[1] = name;

		lg2_merge(repo, 2, __argv);

  } else {
    check_lg2(error, "failed to get current branch", NULL);	
	}

	printf("\nSuccessfully pulled!\n");

cleanup:
	git_reference_free(head);
	git_repository_free(repo);



}

