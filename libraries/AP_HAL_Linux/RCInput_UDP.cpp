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

#define PORT 1153
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
        
        printf("waiting on port %d\n", PORT);
        recvlen = recvfrom(_fd, buf, BUFSIZE, 0, 0, 0);
        printf("received %d bytes\n", recvlen);

        if (recvlen > 0) {
                buf[recvlen] = 0;
                printf("received message: \"%s\"\n", buf);
        }
        /* never exits */
}

#endif // CONFIG_HAL_BOARD_SUBTYPE
