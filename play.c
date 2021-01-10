#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "sound.h"

int
main(int argc, char *argv[])
{
  int fd, my_pid, running_pid;
  struct wav info;

  int pid = fork();
  if (pid != 0) {  // father exit, child continue
    // exec("sh",argv);
    exit();
  }

  my_pid = getpid();
  running_pid = readplaypid();
  if (my_pid != running_pid && running_pid != 0) {
    // printf(0, "killing proc %d\n", my_pid);
    kill(running_pid);
    audiostop();
  }

  setplaypid(my_pid);

  fd = open(argv[1], O_RDWR);
  if (fd < 0)
  {
    printf(0, "open wav file fail\n");
    exit();
  }
 
  read(fd, &info, sizeof(struct wav));

  if ((info.riff_id != 0x46464952)||(info.wave_id != 0x45564157)) {
    printf(0, "invalid file format\n");
    close(fd);
    exit();
  }

  if ((info.info.id != 0x20746d66)||
      (info.info.channel != 0x0002)||
      (info.info.bytes_per_sample != 0x0004)||
      (info.info.bits_per_sample != 0x0010)) {
      printf(0, "%x %x %x %x\n", info.info.id, info.info.channel, info.info.bytes_per_sample, info.info.bytes_per_sample);
    printf(0, "data encoded in an unaccepted way\n");
    close(fd);
    exit();
  }

  setaudiosmprate(info.info.sample_rate);
  uint rd = 0;
  char buf[2049];
  /*
  printf(0, "proc %d is doing preparation work...\n", getpid());
  memset(buf, 0, 2048);
  for (i = 0; i < DMA_BUF_NUM * DMA_BUF_SIZE/2048+1; i++){
    writeaudio(buf, 2048);
  }*/
  // printf(0, "proc %d is playing...\n", getpid());
  while (rd < info.dlen){
    read(fd, buf, (info.dlen - rd < 2048 ? info.dlen -rd : 2048));
    // printf(0, "read pointer: %d, tot_len: %d\n", rd, info.dlen);
    writeaudio(buf, (info.dlen - rd < 2048 ? info.dlen -rd : 2048));
    rd += (info.dlen - rd < 2048 ? info.dlen -rd : 2048);
  }
  finishwrite();
  /*
  memset(buf, 0, 2048);//this part is important! To make the last node sent to addSound!!!
  printf(0, "writing 0...\n");
  for (i = 0; i < DMA_BUF_NUM * DMA_BUF_SIZE/2048+1; i++){
    writeaudio(buf, 2048);
  }*/
  // printf(0, "proc %d finish playing...\n", getpid());
  close(fd);

  //kill(pid);  wait();

  setplaypid(0);
  exit();
}
