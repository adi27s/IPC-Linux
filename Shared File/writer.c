#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define Filename "file.txt"
#define DataString "Wassup !!!\n"

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

    int fd; /* file descriptor */
    if ((fd = open(Filename, O_RDWR | O_CREAT, 0666)) < 0){
        report_and_exit("Open failed !\n");
    }

   if (fcntl(fd,F_SETLK, &lock) < 0){ /* Set lock, is non-blocking function, returns immediately */
    report_and_exit("fnctl failed to get lock !\n");
   }
   else{
    write(fd,DataString,strlen(DataString));
    fprintf(stderr, "Process %d has written to data file\n", lock.l_pid);
   }

   lock.l_type = F_UNLCK;       /* Release the lock explicitly */
   if (fcntl(fd,F_SETLK, &lock) < 0){
    report_and_exit("Explicit unlocking failed !\n");
   }

    close(fd);
    return 0;
}

