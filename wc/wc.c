#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>



void scan_one_file(char *name, int *total_str, int *total_wrd, int *total_smb) {
    int strings = 0, words = 0, symbols = 0;
    int fid;
    if (!(fid = open(name, O_RDONLY))) {
        printf("Cannot open file %s\n", name);
        return;
    }
    char ch;
    bool ok = true;
    while (read(fid, &ch, 1) > 0) {
        if (ch == '\n') {
            strings++;
        }
        if (ch == ' ' || ch == '\n' || ch == '\t') {
            ok = true;
        }
        if (ok && !(ch == ' ' || ch == '\n' || ch == '\t')) {
            words++;
            ok = false;
        }
        symbols++;
    }

    close(fid);
    printf("\t%i\t%i\t%i\n", strings, words, symbols);
    (*total_str) += strings;
    (*total_wrd) += words;
    (*total_smb) += symbols;
}



void wc(int argc, char *argv[]) {
    int total_str = 0, total_wrd = 0, total_smb = 0;
    for (int i = 1; i < argc; ++i) {
        printf("%s%s", argv[i], " : ");
        scan_one_file(argv[i], &total_str, &total_wrd, &total_smb);
    }
    if (argc != 2) {
        printf("%s%i, %i, %i\n", "total : ", total_str, total_wrd, total_smb);
    }
}



int main(int argc, char *argv[]) {
    if (argc == 1) {
        int strings = 0, words = 0, symbols = 0;
        char ch;
        bool ok = true;
        while (read(0, &ch, 1) > 0) {
            if (ch == '\n') {
                strings++;
            }
            if (ch == ' ' || ch == '\n' || ch == '\t') {
                ok = true;
            }
            if (ok && !(ch == ' ' || ch == '\n' || ch == '\t')) {
                words++;
                ok = false;
            }
            symbols++;
        }
        printf("\t%i\t%i\t%i\n", strings, words, symbols);
    } else {
        wc(argc, argv);
    }
    return 0;
}
