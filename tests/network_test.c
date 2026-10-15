#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>

int main(void) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        if (errno == EPERM) {
            puts("Expected result: socket syscall was denied by the sandbox.");
            return 0;
        }
        return 1;
    }
    puts("Socket creation was allowed.");
    return 0;
}
