#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <malloc.h>
#include <stdlib.h>



int main(int argc, char *argv[]) {
    const int n = argc;
    int **pipefd = (int**) malloc((n - 2)*sizeof(int*));
    for (int i = 0; i < n - 2; ++i) {
        pipefd[i] = (int*) malloc(2*sizeof(int));
    }
    for (int i = 0; i < n - 2; ++i) {
        if (pipe(pipefd[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 1; i < n; ++i) {
        __pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) { // child
            if (i != 1) {
                dup2(pipefd[i - 2][0], 0);
            }
            if(i != n - 1) {
                dup2(pipefd[i - 1][1], 1);
            }
            for(int j = 0; j < n - 2; ++j) {
                close(pipefd[j][0]);
                close(pipefd[j][1]);
            }
            if (execlp(argv[i], argv[i], NULL) == -1) {
                printf("%s : команда не найдена", argv[i]);
            }
            exit(EXIT_SUCCESS);
        } else {
            // parent
        }
    }

    for(int i = 0; i < n - 2; ++i) {
        close(pipefd[i][0]);
        close(pipefd[i][1]);
    }
    for(int i = 0; i < n; ++i) {
        wait(NULL);
    }
    for (int i = 0; i < n - 2; ++i) {
        free(pipefd[i]);
    }
    free(pipefd);
    return 0;
}
