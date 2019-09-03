#include "sender.h"
#define binary(bit) strtol(bit,NULL,2)

Sender::Sender(){};

Sender::~Sender(){};

// Send 1 data & address by UDP

void Sender::udp_send(SiTCP* sitcp, unsigned int address, int data){// data: send only 1 data length

  sitcp->RBCPskeleton(0xff, 0x80, address, 0x1, address);
  sitcp->sndData[0] = data;

  memcpy(sitcp->sndBuf+sizeof(sitcp->GetsndHeader()), sitcp->sndData, sizeof sitcp->sndData);
  
  int len = 0;
  len = sendto(sitcp->GetUDPSock(), sitcp->sndBuf, 1 + sizeof(sitcp->GetsndHeader()), 0,
	       (struct sockaddr *)&sitcp->udpAddr, sizeof(sitcp->udpAddr));//important!
  if(len < 0){
    perror("UDP send error");
    exit(1);
  }
   sitcp->rcvRBCP_ACK(1);

}


// Recv 1 data & address by UDP

int Sender::udp_recv(SiTCP* sitcp, unsigned int address, int data){// data: send only 1 data length

    sitcp->RBCPskeleton(0xff, 0xc0, address, 0x1, address);
  
  int len = 0;
  len = sendto(sitcp->GetUDPSock(), sitcp->sndBuf, 1 + sizeof(sitcp->GetsndHeader()), 0,
	       (struct sockaddr *)&sitcp->udpAddr, sizeof(sitcp->udpAddr));//important!
  if(len < 0){
    perror("UDP recv error");
    exit(1);
  }

  int rd_data = 0;
  rd_data =  sitcp->rcvRBCP_ACK(1);
  // std::cout << rd_data << std::endl;

  return rd_data;
}


// Send 1 packet data & address by UDP

void Sender::RBCPpacket_send(SiTCP* sitcp, unsigned int address,
			     unsigned char length, int* data){
  
  std::cout << "enter packet function" << std::endl;
  
  sitcp->RBCPskeleton(0xff, 0x80, address, length, address);
  
  for(int i=0; i<256; ++i){
    sitcp->sndData[i] = data[i];// <- sndData: 0~255
  }

  memcpy(sitcp->sndBuf+sizeof(sitcp->GetsndHeader()), sitcp->sndData, sizeof sitcp->sndData);
  
  int len = 0;
  len = sendto(sitcp->GetUDPSock(), sitcp->sndBuf, sizeof sitcp->sndData + sizeof(sitcp->GetsndHeader()), 0,
  	       (struct sockaddr *)&sitcp->udpAddr, sizeof(sitcp->udpAddr));//important!

  if(len < 0){
    perror("UDP send error");
    exit(1);
  }
  sitcp->rcvRBCP_ACK(0);
}


// Recv 1 packet data & address by UDP

void Sender::RBCPpacket_recv(SiTCP* sitcp, unsigned int address,
			     unsigned char length, int* data){
  
  std::cout << "enter packet function" << std::endl;
  
  sitcp->RBCPskeleton(0xff, 0xc0, address, length, address);

  
  int len = 0;
  len = sendto(sitcp->GetUDPSock(), sitcp->sndBuf, sizeof sitcp->sndData + sizeof(sitcp->GetsndHeader()), 0,
  	       (struct sockaddr *)&sitcp->udpAddr, sizeof(sitcp->udpAddr));//important!

  if(len < 0){
    perror("UDP recv error");
    exit(1);
  }
  sitcp->rcvRBCP_ACK(1);
}

/*=============== multi packet sending method ===============*/

void Sender::RBCP_multi_packet_send(SiTCP* sitcp, unsigned int address,
				    unsigned int total_length, int* data){

  unsigned int total_packet = ((total_length/255)+1)*2;
  unsigned int packetNo(0);

  std::cout << "------------------ RBCP : register access ------------------" << std::endl;    
  std::cout << "" << std::endl;    
  std::cout << "Start sending packet data";    

  for(packetNo=0; packetNo<total_packet; ++packetNo){

    //    sitcp->rcvRBCP_ACK(0);
    sitcp->RBCPskeleton(0xff, 0x80, address+(packetNo*255), 0xff, address+(packetNo*255));
      
    for(int i=0; i<255; ++i){
      sitcp->sndData[i] = data[i+(packetNo*255)];// <- sndData: 0~255
    }
    
    memcpy(sitcp->sndBuf+sizeof(sitcp->GetsndHeader()), sitcp->sndData, sizeof sitcp->sndData);
  
    int len = 0;
    len = sendto(sitcp->GetUDPSock(), sitcp->sndBuf, sizeof sitcp->sndData + sizeof(sitcp->GetsndHeader()), 0,
		 (struct sockaddr *)&sitcp->udpAddr, sizeof(sitcp->udpAddr));//important!

    if(len < 0){
      perror("Send DAQ Start Signal");
      exit(1);
    }

    sitcp->rcvRBCP_ACK(0);
    //    std::cout << sitcp->rcvRBCP_ACK(1) << std::endl;

    std::cout << ".";
  }
  std::cout << "Done!!!" << std::endl;
  std::cout << "" << std::endl;

  std::cout << "Number of sent packets = " << packetNo << std::endl;
  std::cout << "" << std::endl;

}

//--------------------EASIROC MADC sender---------------------------
int Sender::read_madc(SiTCP* sitcp, int data){// data: send only 1 data length

 //Set ADC ch to FPGA re
  sitcp->RBCPskeleton(0xff, 0x80, 0x00000012, 0x1, 0x00000012);
  sitcp->sndData[0] = data;

  memcpy(sitcp->sndBuf+sizeof(sitcp->GetsndHeader()), sitcp->sndData, sizeof sitcp->sndData);
  int len = 0;
  len = sendto(sitcp->GetUDPSock(), sitcp->sndBuf, 1 + sizeof(sitcp->GetsndHeader()), 0,
	       (struct sockaddr *)&sitcp->udpAddr, sizeof(sitcp->udpAddr));//important!
  if(len < 0){
    perror("UDP Send error");
    exit(1);
  }
  sitcp->rcvRBCP_ACK(1);
  
  sitcp->RBCPskeleton(0xff, 0x80, 0x0000001f, 0x1, 0x0000001f);
  sitcp->sndData[0] = 1;
  
  memcpy(sitcp->sndBuf+sizeof(sitcp->GetsndHeader()), sitcp->sndData, sizeof sitcp->sndData);
  len = 0;
  len = sendto(sitcp->GetUDPSock(), sitcp->sndBuf, 1 + sizeof(sitcp->GetsndHeader()), 0,
	       (struct sockaddr *)&sitcp->udpAddr, sizeof(sitcp->udpAddr));//important!
  if(len < 0){
    perror("UDP Send error");
    exit(1);
  }
  sitcp->rcvRBCP_ACK(1);
  
  usleep(20000);//wait ADC ch change
 
  //start Read ADC
  sitcp->RBCPskeleton(0xff, 0x80, 0x00000012, 0x1, 0x00000012);
  sitcp->sndData[0] = 240;
  
  memcpy(sitcp->sndBuf+sizeof(sitcp->GetsndHeader()), sitcp->sndData, sizeof sitcp->sndData);
  len = 0;
  len = sendto(sitcp->GetUDPSock(), sitcp->sndBuf, 1 + sizeof(sitcp->GetsndHeader()), 0,
	       (struct sockaddr *)&sitcp->udpAddr, sizeof(sitcp->udpAddr));//important!
  if(len < 0){
    perror("UDP Send error");
    exit(1);
  }
  sitcp->rcvRBCP_ACK(1);

  sitcp->RBCPskeleton(0xff, 0x80, 0x0000001f, 0x1, 0x0000001f);
  sitcp->sndData[0] = 0;
  
  memcpy(sitcp->sndBuf+sizeof(sitcp->GetsndHeader()), sitcp->sndData, sizeof sitcp->sndData);
  len = 0;
  len = sendto(sitcp->GetUDPSock(), sitcp->sndBuf, 1 + sizeof(sitcp->GetsndHeader()), 0,
	       (struct sockaddr *)&sitcp->udpAddr, sizeof(sitcp->udpAddr));//important!
  if(len < 0){
    perror("UDP Send error");
    exit(1);
  }
  sitcp->rcvRBCP_ACK(1);
  
  //read data
  int rd_data = 0;
  int rd_data1 = 0;
  int rd_data2 = 0;
  sitcp->RBCPskeleton(0xff, 0xc0, 0x00000004, 0x1, 0x00000004);
  len = 0;
  len = sendto(sitcp->GetUDPSock(), sitcp->sndBuf, 1 + sizeof(sitcp->GetsndHeader()), 0,
	       (struct sockaddr *)&sitcp->udpAddr, sizeof(sitcp->udpAddr));//important!
  if(len < 0){
    perror("UDP recv error");
    exit(1);
  }
  rd_data1 =  sitcp->rcvRBCP_ACK(1);
  // std::cout << rd_data << std::endl;
  sitcp->RBCPskeleton(0xff, 0xc0, 0x00000005, 0x1, 0x00000005);
  len = 0;
  len = sendto(sitcp->GetUDPSock(), sitcp->sndBuf, 1 + sizeof(sitcp->GetsndHeader()), 0,
	       (struct sockaddr *)&sitcp->udpAddr, sizeof(sitcp->udpAddr));//important!
  if(len < 0){
    perror("UDP recv error");
    exit(1);
  }
  rd_data2 =  sitcp->rcvRBCP_ACK(1);
  rd_data = rd_data1*256 + rd_data2;
  //std::cout << rd_data << std::endl;
  return rd_data;

}




void Sender::clear_all(SiTCP* sitcp){
  unsigned char address;
  int data;
  //int nbyte;

  // Send register clear
  address = 0x00;
  data = 0x0;
  udp_send(sitcp, address, data);

  std::cout << "Clear all registers in FPGA." << std::endl;
}

