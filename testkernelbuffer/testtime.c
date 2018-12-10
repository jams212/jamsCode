#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
//#define LEVEL 
int main()
{
	struct timeval tv;
 
	struct timeval ts;
	int i = 0;
 
	//timestamp
#ifndef LEVEL 
      printf("hello world!");
#endif       
	gettimeofday(&tv, NULL);
	printf("%ld \n", tv.tv_sec);
	sleep(5);
	gettimeofday(&ts, NULL);
	printf("%ld \n", ts.tv_sec);
	printf("timestamp: %ld s \n", (ts.tv_sec - tv.tv_sec ));
 
	return 0;
}
