#include "liteping.h"
#define LOG_TAG "AML_RIL"
#include <utils/Log.h>

#include <cutils/properties.h>
#include <cutils/sockets.h>
#include <linux/capability.h>


int CRawSock_Init(struct CRawSock* pRawSock, int protocol/* = 0*/)
{
	if (pRawSock == NULL)
    {
		return -1;
    }
   seteuid(0);
   pRawSock->m_sock = socket(AF_INET, SOCK_RAW, protocol);
    setuid(getuid());
   if( pRawSock->m_sock == -1 ) 
	{
		LOGE("socket create failed ");
		pRawSock->m_error = 1;
	}
   else
	{
		int ttl = 64;
		struct timeval tv;
		tv.tv_sec = 5;
		tv.tv_usec = 0;		
		setsockopt(pRawSock->m_sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));
		setsockopt(pRawSock->m_sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(struct timeval));
		setsockopt(pRawSock->m_sock, IPPROTO_IP, IP_TTL, (int *)&ttl, sizeof(int));		
		pRawSock->m_error = 0;
	}

	return pRawSock->m_error;
}

int CRawSock_DeInit(struct CRawSock* pRawSock)
{
	if (pRawSock == NULL)
    {
		return -1;
    }

	close(pRawSock->m_sock);
	return pRawSock->m_error;
}

int CRawSock_SendByIP(struct CRawSock* pRawSock, const void* msg, int msglen, const struct sockaddr* toAddr, unsigned int len)
{
	if (pRawSock == NULL || CRawSock_Error(pRawSock) != 0)
	{
		LOGD("CRawSock_SendByIP return -1");
		return -1;
	}
	int length = sendto(pRawSock->m_sock, msg, msglen, 0, toAddr, len);
	if ( length == -1)
	{
		pRawSock->m_error = 2;
		return -1;
	}
	return length;
}

int CRawSock_SendByName(struct CRawSock* pRawSock, const void* msg, int msglen, const char* hostname)
{
	struct sockaddr_in sin;
	if (pRawSock == NULL || CRawSock_Error(pRawSock) != 0)
	{
		return -1;
	}

	if(hostname)
	{
		struct hostent *hostnm = gethostbyname(hostname);
		if( hostnm == (struct hostent *)0)
		{
			return -1;
		}
		//LOGD("host ip: %s \n",  inet_ntoa(*(struct in_addr*)&hostnm->h_addr));
		sin.sin_addr = *((struct in_addr *)hostnm->h_addr);
	}
	else
	{
		return -1;
	}
	sin.sin_family = AF_INET;
	return CRawSock_SendByIP(pRawSock, msg, msglen, (struct sockaddr *)&sin, sizeof(sin));
}

int CRawSock_Receive(struct CRawSock* pRawSock, void* buf, int buflen, struct sockaddr* from, socklen_t* len)
{
	int length = 0;
	if (pRawSock == NULL || CRawSock_Error(pRawSock) != 0)
	{
		return -1;
	}
	do
	{
		length = recvfrom(pRawSock->m_sock, buf, buflen, 0, from, len);
		if(length == -1)
		{
			if(errno == EINTR )
			{
				continue;
			}
			else 
			{
				pRawSock->m_error = 3;
				return -1;
			}
		}
	}while (length < 0);

	return length;	
}

int CRawSock_Error(struct CRawSock* pRawSock)
{
	return pRawSock->m_error;
}
 

/********************
* ICMP 数据报
*
*
* ********************/
int CICMP_InitAsVoid(struct CICMP* pICMP)
{
	if (pICMP == NULL)
    {
		LOGE("pICMP is NULL");
		return -1;
	}

	pICMP->m_icmpMaxLen = MAXICMPLEN;
	pICMP->m_icmpPacket = NULL;
	CRawSock_Init(&pICMP->m_rawsock, IPPROTO_ICMP);
	pICMP->m_icmpPacket = (struct icmp*)malloc(sizeof(struct icmp) * MAXICMPLEN);
	if (pICMP->m_icmpPacket == NULL)
    {
		return -2;
    }
	pICMP->m_icmpPacket->icmp_code = 0;
	pICMP->m_icmpPacket->icmp_id = 0;
	pICMP->m_icmpPacket->icmp_seq = 0;
	pICMP->m_icmpPacket->icmp_type = ICMP_ECHO;

	return CRawSock_Error(&pICMP->m_rawsock);
}

int CICMP_DeInit(struct CICMP* pICMP)
{
	if (pICMP)
	{
		return -1;
	}
	if (pICMP->m_icmpPacket)
	{
		free(pICMP->m_icmpPacket);
	}
	CRawSock_DeInit(&pICMP->m_rawsock);
	return CRawSock_Error(&pICMP->m_rawsock);
}

uint16_t CommFunc_Checksum(uint16_t *addr, int len)
{
	int nleft = len;
	int sum = 0;
	unsigned short *w = addr;
	unsigned short answer = 0;

	while(nleft > 1)
	{
		sum += *w++;
		nleft -=2;
	}

	if(nleft == 1)
	{
		*(unsigned char *) (&answer) = *(unsigned char *) w;
		sum += answer;
	}

	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	answer = ~sum;
	return (answer);
}

int CICMP_SendICMP(struct CICMP* pICMP, char* host, void* buf, int len)
{
	if (pICMP == NULL)
	{
		return -1;
	}
	memcpy(pICMP->m_icmpPacket->icmp_data, buf, len);
	pICMP->m_icmpPacket->icmp_cksum =0;
	pICMP->m_icmpPacket->icmp_cksum = CommFunc_Checksum((uint16_t *)pICMP->m_icmpPacket, ICMPHEAD + 6);
	return CRawSock_SendByName(&pICMP->m_rawsock, pICMP->m_icmpPacket, MAXICMPLEN, host);
}

int CICMP_RecvICMP(struct CICMP* pICMP, struct sockaddr* from)
{
	char buf[MAXICMPLEN + 100];
	int hlen1, icmplen;
	struct ip *ip;
	struct icmp *icmp;
	socklen_t addrlen = 0;

	if( pICMP == NULL || CRawSock_Error(&pICMP->m_rawsock) ) 
	{
		LOGE("Error() = %d\n", CRawSock_Error(&pICMP->m_rawsock));
		return -1;
	}

	pICMP->m_dataLength = CRawSock_Receive(&pICMP->m_rawsock, buf, MAXICMPLEN, from , &addrlen);

	if ( pICMP->m_dataLength == -1)
	{
		LOGE("Receive Failed\n");
		return -1;
	}

	ip = (struct ip *)buf;
	hlen1 = ip->ip_hl << 2;

	icmp = (struct icmp *) (buf + hlen1);

	if( (icmplen = pICMP->m_dataLength -hlen1) < 8)
	{
		LOGE("Receive Fail 227\n");
		return -1;
	}

	memcpy(pICMP->m_icmpPacket, icmp , icmplen);

	return 0;
}

void CICMP_SetCode(struct CICMP* pICMP, int c)
{
	if (pICMP)
	{
		pICMP->m_icmpPacket->icmp_code = c;
	}
}

void CICMP_SetId(struct CICMP* pICMP, int i)
{
	if (pICMP)
	{
		pICMP->m_icmpPacket->icmp_id = i; 
	}
}

void CICMP_SetSeq(struct CICMP* pICMP, int s)
{
	if (pICMP)
	{
		pICMP->m_icmpPacket->icmp_seq = s;
	}
}

void CICMP_SetType(struct CICMP* pICMP, int t)
{
	if (pICMP)
	{
		pICMP->m_icmpPacket->icmp_type = t;
	}
}

/*int ping(char *host)
{
	struct CICMP icmp;
	struct sockaddr from; //接收数据包的源IP
	int count = 1; //发送次数
	int num = 0;
	int  i = 0;
	char *test_data = "";
	if(host == NULL )
	{
		return 0;
	}
	CICMP_InitAsVoid(&icmp);
	for(i = 0; i < count; i++)
	{
		CICMP_SetId(&icmp, getpid());
		CICMP_SetSeq(&icmp, i);
		CICMP_SendICMP(&icmp, host, test_data, strlen(test_data));
		LOGD("send %s  to ping %s :\n", test_data, host);
	}


	do
	{
		if (CICMP_RecvICMP(&icmp, &from) < 0) 
		{
			break;
		}

		if (icmp.m_icmpPacket->icmp_type == ICMP_ECHOREPLY)
		{
			if (icmp.m_icmpPacket->icmp_id == getpid())
			{
				LOGD("%d bypes from %s : icmp_seq= %d, data= %s\n", icmp.m_dataLength, host, icmp.m_icmpPacket->icmp_seq, icmp.m_icmpPacket->icmp_data);
				num++;
				break;
			}
		}
	}while (num < count);
	CICMP_DeInit(&icmp);
	return num;
}*/

