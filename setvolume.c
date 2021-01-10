#include "types.h"
#include "user.h"

int main(int argc, char *argv[])
{
    int vol = atoi(argv[1]);

	setaudiovolume(vol);
	exit();
}