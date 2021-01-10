#include "types.h"
#include "user.h"
int main()
{
	audiostop();
	int running_pid = readplaypid();
	if(running_pid > 0){
		kill(running_pid);
		setplaypid(0);
		printf(0, "Stopped.\n");
	}
	else{
		printf(0, "Not playing.\n");
	}
	exit();
}
