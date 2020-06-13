#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FILE "/dev/test"

int main(int argc,char * argv[])
{
	
	
	char *name = argv[1]; 
	int fd = open(name,O_RDWR);
	if (fd == -1){
		printf("file open failed\n");
	}
	
	char recv[100];

	
	write(fd,"hello world",100);
	
	read(fd,recv,100);	
		printf("**********%s",recv);	
	close(fd);
	
		
	
	return 0;
}