#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>


#define PATH1 "/dev/scull1"
#define PATH2 "/dev/scull2"

#define BUFSIZE 100
int main(void)
{
	char wbuf1[BUFSIZE] = {0};
	char wbuf2[BUFSIZE] = {0};  
    char r1buf[BUFSIZ] = {0};
    char r2buf[BUFSIZ] = {0};   
    int i = 0;
	for (i = 0; i < sizeof(wbuf1); i++) {
		wbuf1[i] = 'a' + i % 26;
	}
	for (i = 0; i < sizeof(wbuf2); i++) {
		wbuf2[i] = 'A' + i % 26;
	}    

    int pid = fork();

    if (pid == 0)
    {    
        int fd1 = open(PATH1,O_RDWR);
        write(fd1,wbuf1,30);
        int fd2 = open(PATH1,O_RDWR);
        read(fd2,r1buf,40);
        printf("pid==0,r1buf*****%s\n",r1buf);       
    }

    else
    {
        int fd1 = open(PATH1,O_RDWR);    
        write(fd1,wbuf2,30);
        int fd2 = open(PATH1,O_RDWR);
        read(fd2,r1buf,40);
        printf("pid==1,r1buf*****%s\n",r1buf);  
    }
  



    return 0;
}