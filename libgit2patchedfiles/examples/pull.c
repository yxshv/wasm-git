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

	char *_argv[2];
	argv[0] = "fetch";
	argv[1] = ".";

	lg2_fetch(repo, 2, _argv);

	char *__argv[2];
	argv[0] = "merge";
	argv[1] = "origin/master";

	lg2_merge(repo, 2, __argv);

	printf("\nSuccessfully pulled!\n");

}

