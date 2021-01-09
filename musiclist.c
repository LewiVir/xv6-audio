#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "sound.h"
int main(int argc, char *argv[]){
  int fd;
  char * p = 0;
  char * q = p;
 /* *q = '/';
  q++;
  *q = 'w';
  q++;
  *q = 'a';
  q++;
  *q = 'v';
  q++;
  *q = 's';
  q++;
  *q = '/';
  q++;*/
  *q = 'n';q++;*q = 'o';q++;
  *q = 'n';q++;*q = 'e';q++;
  *q = '.';q++;*q = 'w';q++;
  *q = 'a';q++;*q = 'v';q++;
 

  fd = musiclist(p, O_RDWR);
  //printf(0,"%c\n",*p);

  close(fd);

  exit();

    return 0;

}
