#include <AP_HAL.h>

#if CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_IMX6 //Only supported on the i.MX6 right now
    
#include <stdio.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include "RCInput.h"

#define PORT 5005
#define BUFSIZE 2048

extern const AP_HAL::HAL& hal;

using namespace Linux;

void LinuxRCInput_UDP::init(void*)
{
	struct sockaddr_in myaddr;      /* our address */

        /* create a UDP socket */

        if ((_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("cannot create socket\n");
                return;
        }

        fcntl(_fd, F_SETFL, O_NONBLOCK);

        /* bind the socket to any valid IP address and a specific port */
        memset((char *)&myaddr, 0, sizeof(myaddr));
        myaddr.sin_family = AF_INET;
        myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        myaddr.sin_port = htons(PORT);

        if (bind(_fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
                perror("bind failed");
                return; 
        }
}

/*
  called at 1kHz to check for new pulse capture data from the UDP
 */
void LinuxRCInput_UDP::_timer_tick()
{
        int recvlen;                    /* # bytes received */
        unsigned char buf[BUFSIZE];     /* receive buffer */
        long long unsigned timestamp;
	uint16_t sequence;
	uint16_t channelVals[8];
        
        recvlen = recvfrom(_fd, buf, BUFSIZE, 0, 0, 0);

        if (recvlen == 26) {
		memcpy(&timestamp, buf, 8);
		memcpy(&sequence, &buf[8], 2);

		//Reorder the channels a bit.
		//1 -> 3
		//2 -> 1
		//3 -> 2
		//4 -> 4
		memcpy(channelVals, &buf[12], 2); //2->1
		memcpy(&channelVals[1], &buf[14], 2); //3->2
		memcpy(&channelVals[2], &buf[10], 2); //1->3
		memcpy(&channelVals[3], &buf[16], 2); //4->4
		//Copy the rest
		memcpy(&channelVals[4], &buf[18], 8);

		//printf("[");
		//for(int i=0; i<8; ++i)
		//	printf("%i,", channelVals[i]);
		//printf("]\n");

		_process_digital_values(channelVals, 8);

        }
	else if (recvlen > 0) {
		printf("Got wrong number of bytes from RC: %i\n",recvlen);
	}
	
}

#endif // CONFIG_HAL_BOARD_SUBTYPE
