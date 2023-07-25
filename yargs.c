/* yargs.c by Shuaib Syed */
/* I pledge on my honor that I have not given or received any
 * unauthorized assitance on this assignment. */

/* This program is a small simulation of the UNIX utility xargs. This
 * program uses process control and some basic systems programming
 * concepts. The program first figures out if -n is given as an argument on
 * the commmand line. If it did appear all the commands get printed out but
 * they are not executed. But if there is no target porgram, target
 * arguments, and no standard input the program will exit after printing.
 * If -n is not used then the target program will be run with child
 * processes. */

#include "safe-fork.h"
#include "split.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int n_option = 0;
    int target_prog_idx = 1;
    char *target_prog;
    char **target_args;
    int target_args_count;
    char buffer[1024];
    char **input_args;
    int input_args_count;
    int exec_args_count;
    char **exec_args;
    int child_status;
    pid_t child_pid;
    int i;
    int start_idx;

    /* Check for -n option and set variables accordingly */
    if (argc > 1 && strcmp(argv[1], "-n") == 0) {
        n_option = 1;
        target_prog_idx = 2;
    }

    /* If no target program is there exit */
    if (argc <= target_prog_idx) {
        exit(0);
    }

    /* Set target program and target arguments */
    target_prog = argv[target_prog_idx];
    target_args = argv + target_prog_idx;
    target_args_count = argc - target_prog_idx;

    /* Read input lines from standard inputs */
    while (fgets(buffer, sizeof(buffer), stdin)) {
        /* Remove trailing newline character */
        buffer[strcspn(buffer, "\n")] = '\0';
        /* Split input line into separate arguments */
        input_args = split(buffer);
        input_args_count = 0;
        while (input_args[input_args_count] != NULL) {
            input_args_count++;
        }

        /* Combine target arguments and input arguments */
        exec_args_count = target_args_count + input_args_count;
        exec_args = malloc((exec_args_count + 1) * sizeof(char *));
        memcpy(exec_args, target_args, target_args_count * sizeof(char *));
        memcpy(exec_args + target_args_count, input_args,
               input_args_count * sizeof(char *));
        exec_args[exec_args_count] = NULL;

        child_pid = safe_fork();
        if (child_pid == 0) { /* Child process */
            /* If -n option is set, print the commands without executing */
            if (n_option) {
                printf("%s ", target_prog);
                start_idx = target_prog_idx == 1 ? 0 : 1;
                for (i = start_idx; i < exec_args_count; i++) {
                    printf("%s%s", exec_args[i],
                           (i == exec_args_count - 1) ? "\n" : " ");
                }
                exit(0);
            } else {
                /* Execute the target program with the combined agruments*/
                execvp(target_prog, exec_args);
                /* perror("Failed to execute target program"); */
                exit(255);
            }
        } else { /* Parent process */
            /* Wait for child process to finish and check the exit status
             */
            waitpid(child_pid, &child_status, 0);
            if (WIFEXITED(child_status) &&
                WEXITSTATUS(child_status) != 0) {
                exit(WEXITSTATUS(child_status));
            }
        }
        /* Free memory allocated for input_args and exec_args */
        for (i = 0; i < input_args_count; i++) {
            free(input_args[i]);
        }
        free(input_args);
        free(exec_args);
    }

    return 0;
}
