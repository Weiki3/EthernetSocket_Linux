#include "EthernetSocket.h"

void ShowData(unsigned char *d1, int len)
{
    int i;
    for (i = 0; i < len; i++)
        printf("%02x ", d1[i]);
    printf("\n\r");
}

int main()
{
    int protocol = 0x0909;
    const char *cardName = "eth0";
    EtherSock *etherSock = createEthernetSocket(cardName, htons(protocol));

    int size = 14;
    uint8_t buffer[size];

    int n = recvOverEthernet(etherSock, buffer, size); // 实际接收长度

    ShowData(buffer, size);
	printf("buffer:%s\n", buffer);
	printf("Capture len:%d\n", n);

    freeEthernetSocket(etherSock);
}