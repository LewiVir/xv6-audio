#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "buf.h"
#include "sound.h"

struct soundNode audiobuf[3];
int bufcount;
int datacount;//currently occupied count

static int cur_pid = 0;

int sys_setplaypid(void){
    int pid;
    argint(0, &pid);
    // cprintf("setting play_pid as %d\n", pid);
    cur_pid = pid;
    return 0;
}

int sys_readplaypid(void){
    // cprintf("current play_pid is %d\n", cur_pid);
    return cur_pid;
}

void sys_audiostop(void){
    clearSound();
}

int
sys_setaudiosmprate(void)
{
	// cprintf("sys_setaudiosmprate\n");
    int rate, i;
    //获取系统的第0个参数
    if (argint(0, &rate) < 0)
        return -1;

    datacount = 0;
    bufcount = 0;
    //将soundNode清空并置为已处理状态
    for (i = 0; i < 3; i++){
        memset(&audiobuf[i], 0, sizeof(struct soundNode));
        //audiobuf[i].flag = PROCESSED;
        audiobuf[i].flag = AVAILABLE;
    }
    //audio.c设置采样率
    setSoundSampleRate(rate);
    return 0;
}

int sys_setaudiovolume(void)
{
	// cprintf("sys_setaudiovolume\n");
    int n;
    ushort volume;

    if (argint(0, &n) < 0)
        return -1;

    volume = ((n & 0x3F) << 8) + (n & 0x3F);

    // cprintf("id");

    //audio.c设置音量
    setVolume(volume);
    return 0;
}

int sys_audiopause(void)
{
	//cprintf("sys_audiopause\n");
    if(cur_pid != 0){
        pauseSound();
    }
    else{
        cprintf("Not playing.\n");
    }
    return 0;
}

int sys_audioresume(void)
{
	//cprintf("sys_audioresume\n");
    if(cur_pid != 0){
        resumeSound();
    }
    else{
        cprintf("Not playing.\n");
    }
    return 0;
}

int sys_writeaudio(void)
{
	int size;   //second param in
    char *buf;  //first param in
    //soundNode的数据大小
    int bufsize = DMA_BUF_NUM * DMA_BUF_SIZE;
    //获取待播放的数据和数据大小
    if (argint(1, &size) < 0 || argptr(0, &buf, size) < 0)
        return -1;
    //cprintf("sys_writeaudio: size: %d, bufcount: %d, datacount: %d\n", size, bufcount, datacount);
    if (datacount == 0){//i.e., data in audiobuf[bufcount] is meanningless, so clear it first
        //cprintf("datacount == 0, so clear it first.\n");
        memset(&audiobuf[bufcount], 0, sizeof(struct soundNode));
    }

    //若soundNode的剩余大小remaining size = bufsize - datacount大于数据大小size，将数据写入soundNode中
    if (bufsize - datacount > size) {
        //cprintf("sys_writeaudio: bufsize: %d, datacount: %d, size: %d, one node is OK.\n", bufsize, datacount, size);
        memmove(&audiobuf[bufcount].data[datacount], buf, size);
        //audiobuf[bufcount].flag = PCM_OUT | PROCESSED;
        audiobuf[bufcount].flag =   PCM_OUT | AVAILABLE;
        datacount += size;
    }
    else {//not enough remaining size within a node ---> split into multiple nodes to store
        int temp = bufsize - datacount, i;//temp is the max allowable size for audiobuf[bufcount]
        //cprintf("sys_writeaudio: bufsize: %d, datacount: %d, size: %d, one node is NOT enough, temp: %d.\n", bufsize, datacount, size, temp);
        //soundNode存满后调用audioplay进行播放
        memmove(&audiobuf[bufcount].data[datacount], buf, temp);//move the max allowable amount of data
        //audiobuf[bufcount].flag = PCM_OUT;//bufcount is the current index of audiobufs that contain data
        audiobuf[bufcount].flag = PCM_OUT | SENT;
        addSound(&audiobuf[bufcount]);//transfer the audiobuf node to the end of soundQueue
        //once it is sent to soundQueue, this audiobuf could be reused!
        int flag = 1;
        //寻找一个已经被处理(PROCESSED)的soundNode，将剩余数据写入
        while(flag == 1) {
            for (i = 0; i < 3; ++i){//only 3 audiobuf arrays in total
                //if ((audiobuf[i].flag & PROCESSED) == PROCESSED){//find a suitable audiobuf
                if ((audiobuf[i].flag & AVAILABLE) == AVAILABLE){
                    memset(&audiobuf[i], 0, sizeof(struct soundNode));//clear it at first
                    if (bufsize > size - temp) {//remaining data could be placed into one soundNode
                        memmove(&audiobuf[i].data[0], (buf +temp), (size-temp));
                        //audiobuf[i].flag = PCM_OUT | PROCESSED;
                        audiobuf[i].flag = PCM_OUT | AVAILABLE;
                        datacount = size - temp;
                        bufcount = i;
                        flag = -1;
                        break;//break the for loops
                    }
                    else {//too much remaining data that one soundNode is not enough!
                        memmove(&audiobuf[i].data[0], (buf +temp), bufsize);
                        temp = temp + bufsize;
                        //audiobuf[i].flag = PCM_OUT;
                        audiobuf[i].flag = PCM_OUT | SENT;
                        addSound(&audiobuf[i]);//transfer this audiobuf to the end of soundQueue
                    }
                }
                //What if the last node? it is not full but could not be addSound
            }
        }
    }
	return 0;
}

int sys_finishwrite(void)
{
    if (datacount > 0)
        addSound(&audiobuf[bufcount]);
    return 0;
}