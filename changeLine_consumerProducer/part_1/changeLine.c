#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "line.h"

int main(int argc, char *argv[])
{
	int id;
	struct info *ctrl;
	if (argc != 3)
	{
		perror("changeLine <oneChar> <count: 0 to exit>\n");
		exit(-1);
	}
	if ( (id = shmget( key, MSIZ, 0 )) < 0 )
	{
		perror("Error: getting ID of a shared memory\n");
		exit(-1);
	}
	ctrl = (struct info *) shmat( id, 0, 0);
	if ( ctrl <= (struct info *) (0) )
	{
		perror("Error: attaching a shared memory\n");
		exit(1) ;
	}
	/* copy command line data to shared memory  */
	ctrl->c = argv[1][0] ;
	ctrl->length = atoi(argv[2]);
	return 0;
}

