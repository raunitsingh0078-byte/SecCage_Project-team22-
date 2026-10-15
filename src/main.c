#include "sandbox.h"
#include "logger.h"
#include "report.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>

static void usage(const char *name) {
    fprintf(stderr,
        "SecCage - Linux syscall sandbox\n\n"
        "Usage:\n"
        "  %s --profile basic|restricted|strict -- PROGRAM [ARGS...]\n"
        "  %s --help\n\n"
        "Examples:\n"
        "  %s --profile basic -- /bin/echo Hello\n"
        "  %s --profile restricted -- ./tests/allowed_test\n",
        name, name, name, name);
}

int main(int argc, char **argv) {
    const char *profile = NULL;
    int command_index = -1;

    if (argc == 2 && (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h"))) {
        usage(argv[0]);
        return 0;
    }

    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--profile") && i + 1 < argc) {
            profile = argv[++i];
        } else if (!strcmp(argv[i], "--")) {
            command_index = i + 1;
            break;
        } else {
            usage(argv[0]);
            return 2;
        }
    }

    if (!profile || command_index < 0 || command_index >= argc) {
        usage(argv[0]);
        return 2;
    }

    if (!sandbox_profile_exists(profile)) {
        fprintf(stderr, "[SecCage] Unknown profile: %s\n", profile);
        fprintf(stderr, "[SecCage] Available profiles: basic, restricted, strict\n");
        return 2;
    }

    printf("========================================\n");
    printf("       SecCage Security Sandbox\n");
    printf("========================================\n");
    printf("Profile : %s\n", profile);
    printf("Program : %s\n", argv[command_index]);
    printf("Starting sandboxed process...\n\n");
    fflush(stdout);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    pid_t child = fork();
    if (child < 0) {
        perror("fork");
        return 1;
    }

    if (child == 0) {
        if (sandbox_apply_profile(profile) != 0) {
            perror("[SecCage] Failed to apply seccomp profile");
            _exit(125);
        }
        execvp(argv[command_index], &argv[command_index]);
        perror("[SecCage] execvp");
        _exit(127);
    }

    int status = 0;
    if (waitpid(child, &status, 0) < 0) {
        perror("waitpid");
        return 1;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    long elapsed_ms = (end.tv_sec - start.tv_sec) * 1000L +
                      (end.tv_nsec - start.tv_nsec) / 1000000L;

    report_execution(child, status, elapsed_ms);
    logger_event("execution finished; inspect exit status for the outcome");

    if (WIFEXITED(status)) return WEXITSTATUS(status);
    if (WIFSIGNALED(status)) return 128 + WTERMSIG(status);
    return 1;
}
