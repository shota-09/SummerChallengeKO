#include "SiTCP.h"

SiTCP::SiTCP(){
  std::cout<<"SiTCP control --> Start."<<std::endl;
};

SiTCP::~SiTCP(){};

bool SiTCP::SetIPPort(char* IpAddr, unsigned int tcp, unsigned int udp){
  sitcpIpAddr = IpAddr;
  if(tcp<=0 || 65535<=tcp || udp<=0 || 65535<=udp){
    if(tcp<=0 || 65535<=tcp){
      std::cout<<"error : invalid port : tcp = "<<tcp<<std::endl;
    }
    if(udp<=0 || 65535<=udp){
      std::cout<<"error : invalid port : udp = "<<udp<<std::endl;
    }
    exit(1);
  }
  tcpPort = tcp;
  udpPort = udp;
  puts("");
  std::cout<<"SiTCP:IP Address = "<<sitcpIpAddr<<std::endl;
  std::cout<<"      TCP Port   = "<<tcpPort<<std::endl;
  std::cout<<"      UDP Port   = "<<udpPort<<std::endl;
  puts("");
  return true;
}

bool SiTCP::CreateTCPSock(){
  std::cout<<"Create socket for TCP...";
  tcpsock = socket(AF_INET, SOCK_STREAM, 0);
  if(tcpsock < 0){
    perror("TCP socket");
    std::cout<<"errno = "<<errno<<std::endl;
    exit(1);
  }
  memset(&tcpAddr, 0, sizeof(tcpAddr));
  tcpAddr.sin_family      = AF_INET;
  tcpAddr.sin_port        = htons(tcpPort);
  tcpAddr.sin_addr.s_addr = inet_addr(sitcpIpAddr);
  std::cout<<"  Done"<<std::endl;

  std::cout<<"  ->Trying to connect to "<<sitcpIpAddr<<" ..."<<std::endl;
  if(connect(tcpsock, (struct sockaddr *)&tcpAddr, sizeof(tcpAddr)) < 0){
    if(errno != EINPROGRESS) perror("TCP connection");
    FD_ZERO(&rmask);
    FD_SET(tcpsock, &rmask);
    wmask = rmask;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;
    
    int rc = select(tcpsock+1, &rmask, NULL, NULL, &timeout);
    if(rc<0) perror("connect-select error");
    if(rc==0){
      puts("\n     =====time out !=====\n ");
      exit(1);
    }
    else{
      puts("\n     ===connection error===\n ");
      exit(1);
    }
  }  
  FD_ZERO(&readfds);
  FD_SET(tcpsock, &readfds);
  FD_SET(0, &readfds);
 
   puts("  ->Connect success!");

   return true;
 }

 bool SiTCP::CreateUDPSock(){
   std::cout<<"Create socket for RBCP...";
   udpsock = socket(AF_INET, SOCK_DGRAM, 0);
   if(udpsock < 0){
     perror("UDP socket");
     std::cout<<"errno = "<<errno<<std::endl;
     exit(1);
   }
   memset(&udpAddr, 0, sizeof(udpAddr));
   udpAddr.sin_family      = AF_INET;
   udpAddr.sin_port        = htons(udpPort);
   udpAddr.sin_addr.s_addr = inet_addr(sitcpIpAddr);
   std::cout<<"  Done"<<std::endl;
   return true;
 }

 bool SiTCP::CloseUDPSock(){
   std::cout<<"Close UDP Socket...";
   close(udpsock);
   std::cout<<"  Done"<<std::endl;
   return true;
 }

 bool SiTCP::CloseTCPSock(){
   std::cout<<"Close TCP Socket...";
   close(tcpsock);
   std::cout<<"  Done"<<std::endl;
   return true;
 }

 void SiTCP::RBCPskeleton(unsigned char type, unsigned char command, unsigned char id,
			  unsigned char length, unsigned int address){
   sndHeader.type=type;
   sndHeader.command=command;
   sndHeader.id=id;
   sndHeader.length=length;
   sndHeader.address=htonl(address);
   memcpy(sndBuf, &sndHeader, sizeof(sndHeader));
}

int SiTCP::rcvRBCP_ACK(int output){
  fd_set setSelect;
  int rcvdBytes;
  unsigned char rcvdBuf[1024];
  int rd_data = 0;

  //  puts("\nWait to receive the ACK packet...");

  int recvVal = 1;

  while(recvVal){

    FD_ZERO(&setSelect);
    FD_SET(udpsock, &setSelect);

    timeout.tv_sec  = 3;
    timeout.tv_usec = 0;

    if(select(udpsock+1, &setSelect, NULL, NULL,&timeout)==0){
      /* time out */
      recvVal = 0;
      puts("\n***** Timeout ! *****");
      return -1;
    } 
    else {
      /* receive packet */
      if(FD_ISSET(udpsock,&setSelect)){
        rcvdBytes=recvfrom(udpsock, rcvdBuf, 1024, 0, NULL, NULL);
        rcvdBuf[rcvdBytes]=0;
	if(output == 1){
	  //puts("\n***** A pacekt is received ! *****.");
	  //puts("Received data:");
	  
	  int j=0;
	  for(int i=0; i<rcvdBytes; i++){
	    if(j==0) {
	      //printf("\t[%.3x]:%.2x ",i,rcvdBuf[i]);
	      j++;
	    }else if(j==3){
	      //printf("%.2x \n",rcvdBuf[i]);
	      j=0;
	    }else{
	      //printf("%.2x ",rcvdBuf[i]);
	    j++;
	    }
	  }
	  //puts("\n");
	  rd_data = rcvdBuf[8];

	}
        recvVal = 0;
      }
    }
  }
  // printf("%.2x",rd_data);
  return rd_data;
  //usleep(50);
}

