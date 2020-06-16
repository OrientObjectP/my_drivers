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
	char wbuf[BUFSIZE] = {0};
    char r1buf[BUFSIZ] = {0};
    char r2buf[BUFSIZ] = {0};   
    int i = 0;
	for (i = 0; i < sizeof(wbuf); i++) {
		wbuf[i] = 'a' + i % 26;
	}

    int fd1 = open(PATH1,O_RDWR);
    int fd2 = open(PATH2,O_RDWR);

    write(fd1,wbuf,50);
    read(fd1,r1buf,20);

    read(fd2,r2buf,20);

    printf("r1buf*****%s\n",r1buf);
    printf("r2buf*****%s\n",r2buf);







    return 0;
}