#ifndef ETHERNET_SOCKET
#define ETHERNET_SOCKET

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>          // close()
#include <string.h>          // strcpy, memset(), and memcpy()
#include <netdb.h>           // struct addrinfo
#include <sys/types.h>       // needed for socket(), uint8_t, uint16_t, uint32_t
#include <sys/socket.h>      // needed for socket()
#include <netinet/in.h>      // IPPROTO_ICMP, INET_ADDRSTRLEN
#include <netinet/ip.h>      // struct ip and IP_MAXPACKET (which is 65535)
#include <netinet/ip_icmp.h> // struct icmp, ICMP_ECHO
#include <arpa/inet.h>       // inet_pton() and inet_ntop()
#include <sys/ioctl.h>       // macro ioctl is defined
#include <bits/ioctls.h>     // defines values for argument "request" of ioctl.
#include <net/if.h>          // struct ifreq
#include <linux/if_ether.h>  // ETH_P_IP = 0x0800, ETH_P_IPV6 = 0x86DD
#include <linux/if_packet.h> // struct sockaddr_ll (see man 7 packet)
#include <net/ethernet.h>
#include <errno.h> // errno, perror()

// EtherSock 句柄
struct EtherSock
{
    int sd;
    int protocol; // 传输协议
    int ifindex;  // 网卡接口
    struct ifreq *ifr;
    struct sockaddr_ll *linkAddr;
    uint8_t localMacAddr[6]; // 网卡mac地址
};

typedef struct EtherSock EtherSock;

// 创建EthernetSocket
// 注意要使用 htons() 函数将protocol转换成网络字节序
EtherSock *createEthernetSocket(const char *networkCardName, int protocol);

// 释放EthernetSocket
void freeEthernetSocket(EtherSock *etherSock);

// 数据链路层发送函数接口
// 成功传输返回0,失败返回1
int sendOverEthernet(const EtherSock *etherSock, const uint8_t *destMacAddress, const uint8_t *packetData, int packetDataLen);

// 数据链路层接受函数接口
// 成功返回实际接收到的 packet 长度, 失败返回 -1
int recvOverEthernet(const EtherSock *etherSock, uint8_t *packetBuffer, int packetLen);

// 查看unsigned char数组
void ShowData(unsigned char *d1, int len);

#endif