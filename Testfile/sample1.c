#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
	int fd;

	if((fd = open("test.txt",O_RDWR|O_CREAT|O_TRUNC,0666)) < 0)
	{
		perror("fail to open");
		return -1;
	}
	close(fd);

	return 0;
}
