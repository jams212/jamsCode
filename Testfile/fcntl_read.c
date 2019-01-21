#include <unistd.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "lock_set.c"

//int lock_set(int fd,int type);

int main(void)
{
	int fd;
	fd = open("hello",O_RDWR | O_CREAT,0644);
	if(fd < 0)
	{
		printf("Open file error\n");
		exit(1);
	}
	lock_set(fd,F_RDLCK);
	getchar();
	lock_set(fd,F_UNLCK);
	getchar();
	close(fd);
	exit(0);
}

/*int lock_set(int fd,int type)
{
	struct flock old_lock,lock;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;
	lock.l_type = type;
	lock.l_pid = -1;

	fcntl(fd,F_GETLK,&lock);
	if(lock.l_type != F_UNLCK)
	{
		if(lock.l_type == F_RDLCK)
		{
			printf("Read lock already set by %d\n",lock.l_pid);
		}
		else if(lock.l_type == F_WRLCK)
		{
			printf("Write lock already set by %d\n",lock.l_pid);
		}
	}
	lock.l_type = type;
	if((fcntl(fd,F_SETLKW,&lock)) < 0)
	{
		printf("Lock failed:type = %d\n",lock.l_type);
		return -1;
	}
	switch(lock.l_type)
	{
		case F_RDLCK:
			{
				printf("Read lock set by %d\n",getpid());
			}
			break;
		case F_WRLCK:
			{
				printf("Write lock set by %d\n",getpid());
			}
			break;
		case F_UNLCK:
			{
				printf("Release lock by %d\n",getpid());
				return 1;
			}
			break;
	}
	return 0;
}
*/
