#include <stdio.h>
#include <sys/mman.h> 
#include <fcntl.h> 
#include <unistd.h>
#define _BSD_SOURCE

#define RADIO_TUNER_FAKE_ADC_PINC_OFFSET 0
#define RADIO_TUNER_TUNER_PINC_OFFSET 1
#define RADIO_TUNER_CONTROL_REG_OFFSET 2
#define RADIO_TUNER_TIMER_REG_OFFSET 3
#define RADIO_PERIPH_ADDRESS 0x43c00000
#define FIFO_PERIPH_ADDRESS 0x43c10000
#define FIFO_OCCUPANCY_OFFSET 7
#define FIFO_READ_DATA_OFFSET 8

// the below code uses a device called /dev/mem to get a pointer to a physical
// address.  We will use this pointer to read/write the custom peripheral
volatile unsigned int * get_a_pointer(unsigned int phys_addr)
{

	int mem_fd = open("/dev/mem", O_RDWR | O_SYNC); 
	void *map_base = mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, phys_addr); 
	volatile unsigned int *radio_base = (volatile unsigned int *)map_base; 
	return (radio_base);
}

int main()
{
    // first, get a pointer to the peripheral base address using /dev/mem and the function mmap
    volatile unsigned int *fifo = get_a_pointer(FIFO_PERIPH_ADDRESS);

    printf("Now reading 480,000 words from the FIFO\r\n");
    int words_read = 0;
    while(words_read < 480000) {
        volatile unsigned int fifo_count = *(fifo+FIFO_OCCUPANCY_OFFSET);
        if(fifo_count > 0) {
            volatile unsigned int data = *(fifo+FIFO_READ_DATA_OFFSET); // read the data
            words_read++;
        }
    }
    
    return 0;
}
