#include <dirent.h>
#include <stdio.h>
#include <string.h>



void ls_aR(const char *name, int indent) {
    DIR *directory;
    struct dirent *entry;
    if (!(directory = opendir(name))) {
        return;
	}
    while ((entry = readdir(directory))) {
        if (entry->d_type != DT_DIR) {
            printf("%*s- %s\n", indent, "", entry->d_name);
		} else {
            if (!(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)) {
                char path[512];
                snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
                printf("%*s%s\n", indent, "", entry->d_name);
                ls_aR(path, indent + 4);
            }
        }
	}
    closedir(directory);
}



int main(int argc, char *argv[]) {
    if (argc != 2) {
        ls_aR(".", 0);
	} else {
        ls_aR(argv[1], 0);
	}
    return 0;
}
