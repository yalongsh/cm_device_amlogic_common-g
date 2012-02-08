#ifndef C_LITEPING_H
#define C_LITEPING_H

#include<string.h>
#include<errno.h>
 

#include<signal.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdio.h>
#include <stdlib.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<netdb.h>
#include<netinet/in_systm.h>
#include<netinet/ip.h>
#include<netinet/ip_icmp.h>
#include<sys/types.h>

#define ICMPHEAD 8
#define MAXICMPLEN 28

struct CRawSock
{
    int m_sock;
    int m_error;
};

int CRawSock_Init(struct CRawSock* pRawSock, int protocol);
int CRawSock_DeInit(struct CRawSock* pRawSock);
int CRawSock_SendByIP(struct CRawSock* pRawSock, const void* msg, int msglen, const struct sockaddr* toAddr, unsigned int len);
int CRawSock_SendByName(struct CRawSock* pRawSock, const void* msg, int msglen, const char* hostname);
int CRawSock_Receive(struct CRawSock* pRawSock, void* buf, int buflen, struct sockaddr* from, socklen_t* len);
int CRawSock_Error(struct CRawSock* pRawSock);

struct CICMP 
{
	struct CRawSock m_rawsock;
	struct icmp* m_icmpPacket;
	int m_icmpMaxLen;
	int m_dataLength;
};

int CICMP_InitAsVoid(struct CICMP* pICMP);
int CICMP_DeInit(struct CICMP* pICMP);
int CICMP_SendICMP(struct CICMP* pICMP, char* host, void* buf, int len);
int CICMP_RecvICMP(struct CICMP* pICMP, struct sockaddr* from);
void CICMP_SetCode(struct CICMP* pICMP, int c);
void CICMP_SetId(struct CICMP* pICMP, int i);
void CICMP_SetSeq(struct CICMP* pICMP, int s);
void CICMP_SetType(struct CICMP* pICMP, int t);
#endif
