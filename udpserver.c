#include <stdio.h>	//printf
#include <stdlib.h>
#include <string.h> //memset
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define PORT 25344
#define BUFFER_LENGTH 1028
#define NUM_ARGS 3 // It counts the initial ./udpserver so add 1

struct sockaddr_in S_SOCKET;
char S_UDP_BUFFER[BUFFER_LENGTH];

/// @brief Main function, takes two command-line arguments: broadcast address and number of packets
/// @param argc Argument count
/// @param argv Argument vector
/// @return 0 on success, error code if failure
int main(int argc, char** argv) {
    if(argc != NUM_ARGS) {
        printf("Only passed %i arguments, please use the following syntax: udpserver <IP_ADDRESS> <NUM_PACKETS>\r\n", argc);
        return -1;
    }
    
    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(fd == -1) {
        printf("Failed to create UDP socket\r\n");
        return -2;
    }

    S_SOCKET.sin_family = AF_INET;
    S_SOCKET.sin_port = htons(PORT);
    S_SOCKET.sin_addr.s_addr = inet_addr(argv[1]);
    memset(S_UDP_BUFFER, 0xFF, BUFFER_LENGTH);

    for(int i = 0; i < strtoul(argv[2], NULL, 0); i++) {
        printf("Sending message %i...\r\n", i);
        if (sendto(fd, S_UDP_BUFFER, sizeof(S_UDP_BUFFER), 0, (struct sockaddr*)&S_SOCKET, sizeof(S_SOCKET)) < 0){
            printf("Cannot send message to port\r\n");
            close(fd);
            return -3;
        }
    }

    close(fd);
    return 0;
}