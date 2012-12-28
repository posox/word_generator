#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define WORD_LEN 15
#define BUFFER_SIZE 1048576

int main(int argc, char * argv[]) {
    if (argc != 3) {
        fprintf(stderr, "%s: Incorrect parameter, use %s <file size in KiB> <output path>\n", argv[0], argv[0]);
    }

    int fd_random = open("/dev/urandom", O_RDONLY);
    int filegen = open(argv[2], O_WRONLY | O_CREAT, S_IRWXU);
    
    char read_buf[BUFFER_SIZE];
    char write_buf[BUFFER_SIZE];
    int total = atoi(argv[1]);
    total *= 1024;
    int total_write = 0; 
    int cont = 1;
    while (cont) {
        int rd = read(fd_random, &read_buf, BUFFER_SIZE);
        int write_pos = 0;
        int word_pos = 0;
        int words = 0;
        int i;
        for (i = 0; i < BUFFER_SIZE; i++) {
            if ((read_buf[i] >= 'a' && read_buf[i] <= 'z')
                    || (read_buf[i] >= 'A' && read_buf[i] <= 'Z')) {
                write_buf[write_pos] = read_buf[i];
                write_pos++;
                word_pos++;
            }
            if (word_pos == WORD_LEN) {
                word_pos = 0;
                words++;
                write_buf[write_pos] = ' ';
                write_pos++;
            }
        }
        if (total <= total_write + write_pos) {
            write_pos = total - total_write;
            cont = 0;
        }
        write_pos &= ~0xf;
        write_buf[write_pos - 1] = '\n';
        if (write_pos != write(filegen, &write_buf, write_pos)) {
            fprintf(stderr, "No disk space\n");
            return EXIT_FAILURE;
        }
        total_write += write_pos;
    }
    close(fd_random);
    close(filegen);
    return 0;
}
