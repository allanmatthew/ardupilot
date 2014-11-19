
#ifndef __AP_HAL_LINUX_RCOUTPUT_PCA9685_H__
#define __AP_HAL_LINUX_RCOUTPUT_PCA9685_H__

#include <AP_HAL_Linux.h>

#define PCA9685_ADDRESS             0x40 // All address pins low, PCA9685 default

#define PCA9685_MODE1            0x00
#define PCA9685_MODE2            0x01
#define PCA9685_LED0_ON_L        0x06
#define PCA9685_LED0_ON_H        0x07
#define PCA9685_LED0_OFF_L       0x08
#define PCA9685_LED0_OFF_H       0x09
#define PCA9685_ALL_LED_ON_L     0xFA
#define PCA9685_ALL_LED_ON_H     0xFB
#define PCA9685_ALL_LED_OFF_L    0xFC
#define PCA9685_ALL_LED_OFF_H    0xFD
#define PCA9685_PRESCALE        0xFE

class Linux::LinuxRCOutput_PCA9685 : public AP_HAL::RCOutput {
    void     init(void* machtnichts);
    void     set_freq(uint32_t chmask, uint16_t freq_hz);
    uint16_t get_freq(uint8_t ch);
    void     enable_ch(uint8_t ch);
    void     disable_ch(uint8_t ch);
    void     write(uint8_t ch, uint16_t period_us);
    void     write(uint8_t ch, uint16_t* period_us, uint8_t len);
    uint16_t read(uint8_t ch);
    void     read(uint16_t* period_us, uint8_t len);

private:
    void reset();
    AP_HAL::Semaphore *_i2c_sem;
    uint16_t _frequency;    
};

#endif // __AP_HAL_LINUX_RCOUTPUT_PCA9685_H__
