#include "types.h"
#include "user.h"

int main(int argc, char *argv[])
{
    int vol = atoi(argv[1]);
    //printf(0, "setVol: %d\n", vol);
	setaudiovolume(vol);
	exit();
}