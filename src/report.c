#include "report.h"
#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>

void report_execution(int child_pid, int status, long elapsed_ms) {
    printf("\n----------------------------------------\n");
    printf("          Execution Summary\n");
    printf("----------------------------------------\n");
    printf("Process ID : %d\n", child_pid);
    printf("Runtime    : %ld ms\n", elapsed_ms < 0 ? 0 : elapsed_ms);

    if (WIFEXITED(status)) {
        printf("Exit code  : %d\n", WEXITSTATUS(status));
        printf("Result     : Process exited normally\n");
    } else if (WIFSIGNALED(status)) {
        printf("Signal     : %d\n", WTERMSIG(status));
        printf("Result     : Process terminated by signal\n");
        if (WTERMSIG(status) == SIGSYS)
            printf("Note       : SIGSYS can indicate a syscall policy violation\n");
    } else {
        printf("Result     : Process ended with an unclassified status\n");
    }
    printf("----------------------------------------\n");
}
