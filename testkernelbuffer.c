#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
int fd = open("temp.txt",O_RDWR | O_CREAT | O_TRUNC,0664);
char temp[1024] = {0};
dup2(fd,STDOUT_FILENO);
for(int i = 0; i < 4096;i++)
{
printf("%c",48);
}
while(1)
{
read(STDIN_FILENO,temp,sizeof(temp));
if(strncmp(temp,"exit",4) == 0)
	break;
printf("%s\n",temp);
}
close(fd);
return 0;
}
