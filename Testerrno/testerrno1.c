#include <stdio.h>
#include <errno.h>

int main()
{
	FILE * fp;
	if((fp = fopen("1.txt","r")) == NULL)
	{
		printf("fail to fopen:%s\n",strerror(errno));
		return -1;
	}
	fclose(fp);
	return 0;
}
