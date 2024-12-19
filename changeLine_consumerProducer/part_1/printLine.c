#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include "line.h"

int main()
{
	int i, id;
	struct info *ptr;
	if( (id = shmget(key, MSIZ, IPC_CREAT | 0666) ) < 0)
	{
		perror("Error: creating shared memory\n");
		exit(-1);
	}
	if( (ptr = (struct info *) shmat(id, 0, 0)) <= (struct info *) (0) )
	{
		perror("Error: attaching shared memory\n");
		exit(-1);
	}
	ptr->c = 'a';
	ptr->length = 10;
	while(ptr->length > 0)
	{
		for(i=0; i<ptr->length; i++)
			putchar(ptr->c);
		putchar('\n');
		sleep(4);
	}
	return 0;
}
