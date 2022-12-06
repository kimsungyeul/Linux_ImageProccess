#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
	srand((unsigned int)time(NULL));
	printf("난수 : %d\n", rand()%255);
	printf("난수 : %d\n", rand()%255);
	printf("난수 : %d\n", rand()%255);
	printf("난수 : %d\n", rand()%255);
	printf("난수 : %d\n", rand()%255);
	printf("난수 : %d\n", rand()%255);
	return 0;
}

