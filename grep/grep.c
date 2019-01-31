#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <wait.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

char **files;
int files_number;
int children_number;



void get_files_count(const char* files_list) {
    int fd = open(files_list, O_RDONLY);
    char ch;
    bool ok = false;
    while (read(fd, &ch, 1) > 0) {
        if (ch == '\n') {
            if (ok) {
                ok = false;
                files_number++;
            }
        } else {
            ok = true;
        }
    }
    close(fd);
}



void read_files_names(const char* files_list) {
    int fd = open(files_list, O_RDONLY);
    for (int i = 0; i < files_number; ++i) {
        int size = 512;
        char *name = malloc(size);
        int pointer = 0;
        char ch;
        bool ok = false;
        read(fd, &ch, 1);
        while (ch != '\n' || !ok) {
            if (pointer == size - 1) {
                size*=2;
                name = realloc(name, size);
            }
            if (ch != '\n') {
                ok = true;
                name[pointer] = ch;
                pointer++;
            }
            read(fd, &ch, 1);
        }
        files[i] = name;
    }
    close(fd);
}



int main(int argc, char *argv[]) {
    char *pattern = argv[1];
    char *files_list = argv[2];
    children_number = atoll(argv[3]);
    get_files_count(files_list);
    if (children_number > files_number) {
        children_number = files_number;
	}

    files = (char**) malloc(files_number*sizeof(char*));
    read_files_names(files_list);

    int *fd = (int*) malloc(files_number*sizeof(int));
    for (int i = 0; i < files_number; ++i) {
        if (i >= children_number) {
            wait(NULL);
        }
        char template[] = "/tmp/fileXXXXXX";
        fd[i] = mkstemp(template);
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(1);
        }
        if (pid == 0) {
            dup2(fd[i], 1);
            close(fd[i]);
            execlp("grep", "grep", pattern, files[i], NULL);
        }
        unlink(template);
    }

    for (int i = 0; i < children_number; ++i) {
        wait(NULL);
    }

    int strings_number = 512;
    char **strings = (char**) malloc(strings_number*sizeof(char*));
    int counter = 0;
    for (int i = 0; i < files_number; ++i) {
        lseek(fd[i], 0, SEEK_SET);
        FILE* f = fdopen(fd[i], "r");
        const int bufferSize = 512;
        char* str = (char*) malloc(bufferSize);
        char c;
        int len = 0;
        int mem = bufferSize;
        while (!feof(f)) {
            c = fgetc(f);
            if (len == mem) {
                mem += bufferSize;
                str = realloc(str, mem);
            }
            str[len++] = c;
            if (c == '\n') {
                int j = 0;
                char *str_temp = (char*) malloc(bufferSize);
                for ( ; j < strlen(files[i]); ++j) {
                    str_temp[j] = files[i][j];
				}
                str_temp[j++] = ':';
                for (int k = j; k < len + j; ++k) {
                    str_temp[k] = str[k-j];
				}

                bool is_dublicate = false;
                for (int j = 0; j < counter; ++j) {
                    if (!strcmp(strings[j], str_temp)) {
                        is_dublicate = true;
                        break;
                    }
                }

                if (!is_dublicate) {
                    strings[counter++] = str_temp;
                    if (counter == strings_number) {
                        strings_number*=2;
                        strings = (char **) realloc(strings, strings_number* sizeof(char*));
                    }
                }
                len = 0;
            }
        }
        free(str);
    }
    for (int i = 0; i < counter; ++i) {
        printf("%s", strings[i]);
	}
    for (int i = 0; i < files_number; ++i) {
        free(files[i]);
	}
    free(files);
    return 0;
}
