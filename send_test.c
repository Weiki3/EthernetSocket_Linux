#include "EthernetSocket.h"

const uint8_t board_mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

int main()
{
    int protocol = 0x0909;
    char *cardName = "eth0";
    EtherSock *etherSock = createEthernetSocket(cardName, htons(protocol));

    char *text = "hello world!\n";
    int size = 14;

    sendOverEthernet(etherSock, board_mac, text, size);
    printf("Send Packet on 0x0909: hello world!\n");

    freeEthernetSocket(etherSock);
}