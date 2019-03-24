/////////////////////////////////////////////////////////////////////////////////////
//// 语音交流程序                                                            /////////
//// 基于RTP                                                                /////////        
//// 创建两个线程                                                            /////////
//// 一个线程负责录音然后发送数据包                                            /////////  
//// 另一个接受数据包并播放                                                   /////////
/////////////////////////////////////////////////////////////////////////////////////
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>   
#include <netinet/in.h>
#include <iostream>
#include <rtpsession.h>
#include <rtppacket.h>
//#include <rtpdefines.h>
//#include <rtpstructs.h>
//#include <rtptransmitter.h>
#include <rtpsessionparams.h>
#include <rtpudpv4transmitter.h>
#include <rtpipv4address.h>
//#include <arpa/inet.h>
//#include <cstdlib>
//#include <cstring>
#include "checkerror.h"
#include "device.h"

#define BUFSZ 960
#define SMALL 60//6 frames

using std::cout;
using std::endl;

pthread_t thread[2];   //线程ID
pthread_mutex_t mutex; //初始化互斥锁
pthread_cond_t condition; 

RTPSession session;
//char inbuffer[BU1FSZ];
//unsigned char outbuffer[SMALL] ;
char buf[BUFSZ];
int aic23;//, encoder, decoder;
bool flag = true;
//bool dec_flag = false;

  /*
   * Thread0, write audio data to encoder,read data from encoder and send it
   * to net.
   */
void *Thread0(void* arg)     //创建线程函数
{
  int i;//, j, cnt;
  while(1)
  {
    pthread_mutex_lock(&mutex);
    if(!flag)
    {
      pthread_cond_signal(&condition);
      pthread_cond_wait(&condition, &mutex);
    }
    //cout << "send........." << endl;
    i = read(aic23, buf,BUFSZ );
    //printf("read %d data from aic23 \n",i);
 //   j = write(encoder, inbuffer, BUFSZ);
   // printf("write %d data to encoder \n",j);
   // cnt = read(encoder, outbuffer, SMALL);
   // printf("read %d data from encoder \n",cnt);
    i=session.SendPacket(buf, i);
    checkerror(i);
  //  printf("send ok\n");
   // RTPTime::Wait(RTPTime(1,0));
    flag = false;
    pthread_mutex_unlock(&mutex);
    
  }  
}

  /*
   * thread1,receive data from the net and write it to decoder,
   * read data from decoder and write it to aic23.
   */
void *Thread1(void* arg)
{
  int len,status;
  unsigned char *data;
//  char outdata[BUFSZ];
//  printf("into thread1\n");
  while(1)
  { 
   // printf("into while\n");
    pthread_mutex_lock(&mutex);
    if(flag)
    { 
      pthread_cond_signal(&condition);
      pthread_cond_wait(&condition, &mutex);
    }
    //cout << "receive.........." << endl;
    //printf("into receive\n");
 //   session.Poll();
    session.BeginDataAccess();
    RTPPacket* packet = NULL;
    status=session.GotoFirstSourceWithData();
//printf("the value of gotofirstsourcewithdata is %d\n",status);
    if(session.GotoFirstSourceWithData() &&
        (packet = session.GetNextPacket()) != NULL)
    {
     // printf("can receive\n");
      len = packet->GetPayloadLength();
      data = packet->GetPayloadData();
     // write(decoder, data, len);
     // read(decoder, outdata, BUFSZ);
      write(aic23, data, BUFSZ);
      //printf("write %d data to aic23 \n",len);
    }
    delete packet;
    session.EndDataAccess();
    status = session.Poll();
    checkerror(status);
    //RTPTime::Wait(RTPTime(1,0));
    flag = true;
    pthread_mutex_unlock(&mutex);
    
  }
}

int main(int argc, char** argv)
{
  unsigned long dest_ip;
  int mixer, comm_port, dest_port, state;
  char *codec = (char *)("/dev/dsp");
  char *sound_ctrl =(char *)("/dev/mixer");
 // char *task0 = "/dev/dsptask/encoder";
 // char *task1 = "/dev/dsptask/decoder";



  /*
   * Usage.....
   */
  cout << endl;
  if (argc != 2) 
  {
    cout << "Usage: talking  dest_ip" << endl;
    exit(-1);
  }	
  /* 
   * Get ip and port user specified
   */
  dest_ip = inet_addr(argv[1]);//2]);
  if (dest_ip == INADDR_NONE) 
  {
    cout << "Bad IP address specified." << endl;
    exit(-1);
  }
  dest_ip = ntohl(dest_ip);
  //comm_port = atoi(argv[1]);
  //dest_port = atoi(argv[3]);

  /*
   * Open devices and set parameters
   */
  aic23 = open_device(codec);
  mixer = open_device(sound_ctrl);
 // encoder = open_device(task0);
 // decoder = open_device(task1);
  audio_set(aic23);
  mixer_set(mixer);
  /*	
   * Create a session  
   */
  RTPUDPv4TransmissionParams transparams;
  RTPSessionParams sessionparams;
  sessionparams.SetOwnTimestampUnit(1.0/8000.0);
  transparams.SetPortbase(6000);//comm_port);

  state = session.Create(sessionparams,&transparams);
  checkerror(state);


  /*
   * Add destination
   */
  RTPIPv4Address addr(dest_ip,6000);//dest_port);
  state = session.AddDestination(addr);
  checkerror(state);

  
  /*
   * Set default parameter  
   */
  session.SetDefaultPayloadType(0);
  session.SetDefaultMark(false);
  session.SetDefaultTimestampIncrement(10);
  
  /*
   * Set receiving mode
   */
 
  session.SetReceiveMode(RTPTransmitter::AcceptSome);//RECEIVEMODE_ACCEPTSOME);
  session.AddToAcceptList(addr);

  /*
   * Initialize condition sigal and thread handle
   */
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&condition, NULL);
  memset(&thread, 0, sizeof(thread));

  /*
   * Now create two threads
   */
  if(pthread_create(&thread[0],NULL,Thread0,NULL) != 0)
  {
        cout << "Thread0 not created!" << endl;
        exit(-1);
  }
  else
        cout << "Thread0 created OK!" << endl;

  if(pthread_create(&thread[1],NULL,Thread1,NULL) != 0)
  {
        cout << "Thread1 not created!" << endl;
        exit(-1);
  }
  else
        cout << "Thread1 created OK!" << endl;

  /*
   * Wait any thread to exit
   */
  if(thread[0] != 0)
  {
    pthread_join(thread[0],NULL);
  }
 
 // close(decoder);
 // close(encoder);
  close(mixer);
  close(aic23); 

  return 0;
}

