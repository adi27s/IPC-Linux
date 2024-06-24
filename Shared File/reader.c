#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define Filename "file.txt"

void report_and_exit(const char *msg){
    perror(msg);
    exit(-1);
}

int main(){
    struct flock lock;
    lock.l_type = F_WRLCK;      /* Write lock */
    lock.l_whence = SEEK_SET;   /* Seek from the beginning of the file */
    lock.l_start = 0;           /* 1st byte in file */
    lock.l_len = 0;             /* until EOF */
    lock.l_pid = getpid();

    int fd;
    if ((fd = open(Filename, O_RDONLY)) < 0){
        report_and_exit("Open to read failed !\n");
    }

    fcntl(fd,F_GETLK,&lock); /* sets lock.l_type = F_UNLCK if no write lock */
    if(lock.l_type != F_UNLCK){
        report_and_exit("File is still write locked !\n");
    }

    lock.l_type = F_RDLCK; /* Prevents writing during reading */   
    if (fcntl(fd,F_SETLK, &lock)<0){
        report_and_exit("Cant get a read-only lock !\n");
    }

    int c;
    while(read(fd,&c,1)>0){
        write(STDOUT_FILENO,&c,1);
    }

    lock.l_type = F_UNLCK;
    if (fcntl(fd,F_SETLK, &lock) < 0){
        report_and_exit("Explicit unlocking failed !\n");
    }

    close(fd);
    return 0;
}