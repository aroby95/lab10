#include <stdio.h>	//printf
#include <stdlib.h>
#include <string.h> //memset
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h> 

#define PORT 25344
#define BUFFER_LENGTH 1028
#define NUM_SAMPLES 256
#define NUM_ARGS 2 // It counts the initial ./udpserver so add 1
#define RADIO_TUNER_FAKE_ADC_PINC_OFFSET 0
#define RADIO_TUNER_TUNER_PINC_OFFSET 1
#define RADIO_TUNER_CONTROL_REG_OFFSET 2
#define RADIO_TUNER_TIMER_REG_OFFSET 3
#define RADIO_PERIPH_ADDRESS 0x43c00000
#define FIFO_PERIPH_ADDRESS 0x43c10000
#define FIFO_OCCUPANCY_OFFSET 7
#define FIFO_READ_DATA_OFFSET 8

struct sockaddr_in S_SOCKET;
char S_UDP_BUFFER[BUFFER_LENGTH];

#pragma pack()
typedef struct {
    uint32_t counter;
    uint32_t samples[NUM_SAMPLES];
} udp_packet;

// the below code uses a device called /dev/mem to get a pointer to a physical
// address.  We will use this pointer to read/write the custom peripheral
volatile unsigned int * get_a_pointer(unsigned int phys_addr)
{

	int mem_fd = open("/dev/mem", O_RDWR | O_SYNC); 
	void *map_base = mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, phys_addr); 
	volatile unsigned int *radio_base = (volatile unsigned int *)map_base; 
	return (radio_base);
}

/// @brief Main function, takes two command-line arguments: broadcast address and number of packets
/// @param argc Argument count
/// @param argv Argument vector
/// @return 0 on success, error code if failure
int main(int argc, char** argv) {
    if(argc != NUM_ARGS) {
        printf("Only passed %i arguments, please use the following syntax: udpserver <IP_ADDRESS>\r\n", argc);
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
    
    volatile unsigned int *fifo = get_a_pointer(FIFO_PERIPH_ADDRESS);
    volatile unsigned int fifo_count = *(fifo+FIFO_OCCUPANCY_OFFSET);

    uint32_t packet_cnt = 0;
    volatile udp_packet packet_to_send = {0};
    while(1) {
        
        if(fifo_count > 0) {
            packet_to_send.counter = packet_cnt++;
            for(int i = 0; i < NUM_SAMPLES; i++) {
                // read the data
                packet_to_send.samples[i] = *(fifo+FIFO_READ_DATA_OFFSET);
                usleep(50);
            }
            sendto(fd, (const void*)&packet_to_send, sizeof(udp_packet), 0, (struct sockaddr*)&S_SOCKET, sizeof(S_SOCKET));
        }
    }

    close(fd);
    return 0;
}