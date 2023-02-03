#include "EthernetSocket.h"

// 创建socket
EtherSock *createEthernetSocket(const char *networkCardName, int protocol)
{
    int sd = socket(PF_PACKET, SOCK_RAW, protocol);
    if (sd < 0)
        return NULL;

    EtherSock *etherSock = (EtherSock *)malloc(sizeof(EtherSock));
    etherSock->ifr = (struct ifreq *)malloc(sizeof(struct ifreq));
    etherSock->linkAddr = (struct sockaddr_ll *)malloc(sizeof(struct sockaddr_ll));

    etherSock->protocol = protocol;
    etherSock->sd = sd;

    // 绑定网卡与socket
    memset(etherSock->ifr, 0, sizeof(struct ifreq));
    memcpy(etherSock->ifr->ifr_name, networkCardName, sizeof(etherSock->ifr->ifr_name));
    if (setsockopt(sd, SOL_SOCKET, SO_BINDTODEVICE, (void *)etherSock->ifr, sizeof(struct ifreq)) < 0)
    {
        perror("Failed to bind socket to device, please check the network card name\n");
        exit(EXIT_FAILURE);
    }

    // 获取MAC地址
    if (ioctl(sd, SIOCGIFHWADDR, etherSock->ifr) < 0)
    {
        perror("Failed to obtain local mac address, please check the network card name\n");
        exit(EXIT_FAILURE);
    }

    // 获取网卡索引
    if (ioctl(sd, SIOCGIFINDEX, etherSock->ifr) < 0)
    {
        perror("Failed to obtain device index, please check the network card name\n");
        exit(EXIT_FAILURE);
    }

    // MAC地址拷贝
    memcpy(etherSock->localMacAddr, etherSock->ifr->ifr_hwaddr.sa_data, sizeof(uint8_t) * 6);

    etherSock->linkAddr->sll_halen = 6;
    etherSock->linkAddr->sll_family = PF_PACKET;
    etherSock->linkAddr->sll_protocol = protocol;
    etherSock->linkAddr->sll_ifindex = etherSock->ifr->ifr_ifindex;

    return etherSock;
}

// 释放socket
void freeEthernetSocket(EtherSock *etherSock)
{
    free(etherSock->ifr);
    free(etherSock->linkAddr);
    free(etherSock);
}

// 发送函数
int sendOverEthernet(const EtherSock *etherSock, const uint8_t *destMacAddress, const uint8_t *packetData, int packetDataLen)
{
    if (packetDataLen > 1500 || packetDataLen < 0)
    {
        perror("Invalid input of packetDataLen\n");
        exit(EXIT_FAILURE);
    }

    // 创建 Ethernet II 帧头
    struct ether_header header;

    // 创建缓冲区
    int bufferSize = sizeof(struct ether_header) + sizeof(uint8_t) * packetDataLen;
    uint8_t *buffer = (uint8_t *)malloc(bufferSize);

    // MAC地址拷贝
    memcpy(header.ether_dhost, destMacAddress, sizeof(uint8_t) * 6);
    memcpy(header.ether_shost, etherSock->localMacAddr, sizeof(uint8_t) * 6);

    // 拷贝协议
    header.ether_type = etherSock->protocol;

    memcpy(buffer, &header, sizeof(header));
    memcpy(buffer + sizeof(header), packetData, packetDataLen);

    // 通过socket发送数据
    if (sendto(etherSock->sd, buffer, bufferSize, 0, (struct sockaddr *)etherSock->linkAddr, sizeof(struct sockaddr_ll)) <= 0)
    {
        perror("Failed when send data over socket\n");
        exit(EXIT_FAILURE);
    }

    free(buffer);
    return 1;
}

// 接收函数
int recvOverEthernet(const EtherSock *etherSock, uint8_t *packetBuffer, int packetLen)
{
    int length = 0;

    uint8_t *tempBuffer = (uint8_t *)malloc(packetLen + sizeof(struct ether_header));
    if (!tempBuffer)
    {
        printf("Failed to malloc\n");
        return -1;
    }
    memset(tempBuffer, 0, packetLen + sizeof(struct ether_header));

    length = recvfrom(etherSock->sd, tempBuffer, packetLen + sizeof(struct ether_header), 0, NULL, NULL);
    if (length < sizeof(struct ether_header))
    {
        perror("Failed to receive packet data\n");
        exit(EXIT_FAILURE);
    }

    // 数据包长度
    length -= sizeof(struct ether_header);

    memcpy(packetBuffer, tempBuffer + sizeof(struct ether_header), length);
    free(tempBuffer);

    return length;
}

void ShowData(unsigned char *d1, int len)
{
    int i;
    for (i = 0; i < len; i++)
        printf("%02x ", d1[i]);
    printf("\n");
}