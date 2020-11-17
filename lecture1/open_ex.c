#include <fcntl.h> // for open
#include <unistd.h> // for read, write, close
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

const char *kFilename = "my_file";
const int kFileExistsErr = 17;

int main() {
    umask(0);
    int file_descriptor = open(kFilename, O_WRONLY | O_CREAT | O_EXCL, 0644);
    if (file_descriptor==-1) {
        printf("There was a problem creating '%s'!\n", kFilename);
        if (errno == kFileExistsErr) {
            printf("The file already exists!\n");
        } else {
            printf("Unknown errono: %d\n", errno);
        }
        return -1;
    }
    close(file_descriptor);
    return 0;
}