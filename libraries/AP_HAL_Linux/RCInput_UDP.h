
#ifndef __AP_HAL_LINUX_RCINPUT_UDP_H__
#define __AP_HAL_LINUX_RCINPUT_UDP_H__

/*
  This class implements RCInput on the i.mx6 with a UDP socket input
  of dsm control data
 */

#include <AP_HAL_Linux.h>

class Linux::LinuxRCInput_UDP : public Linux::LinuxRCInput 
{
public:
    void init(void*);
    void _timer_tick(void);

 private:
    int _fd;
};

#endif // __AP_HAL_LINUX_RCINPUT_UDP_H__
