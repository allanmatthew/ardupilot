
#include <AP_HAL.h>

#if CONFIG_HAL_BOARD == HAL_BOARD_LINUX

#include "RCOutput_PCA9685.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

using namespace Linux;

#define PWM_CHAN_COUNT 16

static const AP_HAL::HAL& hal = AP_HAL_BOARD_DRIVER;

void LinuxRCOutput_PCA9685::init(void* machtnicht)
{
    _i2c_sem = hal.i2c->get_semaphore();
    if (_i2c_sem == NULL) {
        hal.scheduler->panic(PSTR("PANIC: RCOutput_PCA9685 did not get "
                                  "valid I2C semaphore!"));
        return; // never reached
    }

    // Software reset
    hal.i2c->writeRegister(PCA9685_ADDRESS, PCA9685_MODE1, 0x00);    

    // Set the initial frequency
    set_freq(0, 50);
    
    if (!_i2c_sem->take(10)){
        hal.scheduler->panic(PSTR("PANIC: RCOutput_PCA9685: failed to take "
                    "I2C semaphore for init"));
        return; /* never reached */
    }
    
    _i2c_sem->give();
}

void LinuxRCOutput_PCA9685::set_freq(uint32_t chmask, uint16_t freq_hz)
{   
    uint8_t prescale, oldmode, newmode;
    float freq_adj, f_prescale;
 
    if (!_i2c_sem->take(10)) {
        return;
    }
    freq_adj = freq_hz * 0.9; 
    f_prescale = 25000000;
    f_prescale /= 4096;
    f_prescale /= freq_adj;
    f_prescale -= 1;
    prescale = floor(f_prescale + 0.5);
    
    hal.i2c->readRegister(PCA9685_ADDRESS, PCA9685_MODE1, &oldmode);
    newmode = (oldmode&0x7F) | 0x10; // sleep
    hal.i2c->writeRegister(PCA9685_ADDRESS, PCA9685_MODE1, newmode); // go to sleep
    hal.i2c->writeRegister(PCA9685_ADDRESS, PCA9685_PRESCALE, prescale);
    hal.i2c->writeRegister(PCA9685_ADDRESS, PCA9685_MODE1, oldmode);
    hal.scheduler->delay(5);
    hal.i2c->writeRegister(PCA9685_ADDRESS, PCA9685_MODE1, oldmode|0xa1);//Auto increment
    
    _frequency = freq_hz;
    
    _i2c_sem->give();
}

uint16_t LinuxRCOutput_PCA9685::get_freq(uint8_t ch)
{
    return _frequency;
}

void LinuxRCOutput_PCA9685::enable_ch(uint8_t ch)
{
    
}

void LinuxRCOutput_PCA9685::disable_ch(uint8_t ch)
{
    write(ch, 0);
}

void LinuxRCOutput_PCA9685::write(uint8_t ch, uint16_t period_us)
{   
    if(ch >= PWM_CHAN_COUNT){
        return;
    }
    
    if (!_i2c_sem->take_nonblocking()) {
        return;
    }
    
    uint16_t length;
    
    if (period_us == 0)
        length = 0;
    else
        length = round((period_us * 4096) / (1000000.f / _frequency)) - 1;
        
    uint8_t data[4] = {0x00, 0x00, (uint8_t)(length & 0xFF), (uint8_t)(length >> 8)};
    hal.i2c->writeRegisters(PCA9685_ADDRESS, 
                            PCA9685_LED0_ON_L + 4 * ch, 
                            4, 
                            data);
                                             
    _i2c_sem->give();                                         
}

void LinuxRCOutput_PCA9685::write(uint8_t ch, uint16_t* period_us, uint8_t len)
{
    for (int i = 0; i < len; i++)
        write(ch + i, period_us[i]);
}

uint16_t LinuxRCOutput_PCA9685::read(uint8_t ch)
{
    if (!_i2c_sem->take_nonblocking()) {
        return 0;
    }
    
    uint8_t data[4] = {0x00, 0x00, 0x00, 0x00};
    hal.i2c->readRegisters(PCA9685_ADDRESS, 
                           PCA9685_LED0_ON_L + 4 * ch, 
                           4, 
                           data);
                           
    uint16_t length = data[2] + ((data[3] & 0x0F) << 8);    
    uint16_t period_us = (length + 1) * (1000000.f / _frequency) / 4096.f;
    
    _i2c_sem->give(); 
    
    return length == 0 ? 0 : period_us;   
}

void LinuxRCOutput_PCA9685::read(uint16_t* period_us, uint8_t len)
{
    for (int i = 0; i < len; i++) 
        period_us[i] = read(0 + i);
}

#endif // CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_PCA9685
