
#ifndef __AP_HAL_LINUX_GPIO_IMX6_H__
#define __AP_HAL_LINUX_GPIO_IMX6_H__

#include <AP_HAL_Linux.h>

#define SYSFS_GPIO_DIR "/sys/class/gpio"

#define GPIO0_BASE 0x209C000
#define GPIO1_BASE 0x20A0000
#define GPIO2_BASE 0x20A4000
#define GPIO3_BASE 0x20A8000
#define GPIO4_BASE 0x20AC000
#define GPIO5_BASE 0x20B0000
#define GPIO6_BASE 0x20B4000

#define GPIO_SIZE  0x1F

// OE: 0 is output, 1 is input
#define GPIO_DR    0x000  //Data register
#define GPIO_DIR   0x004  //Direction register

#define LED_AMBER       2   //USER_LED
#define LED_BLUE        19
#define LED_SAFETY      21  
#define SAFETY_SWITCH   101 //USER_BUTTON

#define LOW             0
#define HIGH            1

#if CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_IMX6
#define LINUX_GPIO_NUM_BANKS 7
#else
// disable GPIO
#define LINUX_GPIO_NUM_BANKS 0
#endif

// i.MX6 GPIO mappings
#define IMX6_GPIO(B, P) ((B-1)*32+P)

class Linux::LinuxGPIO_IMX6 : public AP_HAL::GPIO {
private:
    struct GPIO {
        volatile uint32_t *base;
        volatile uint32_t *dr;
        volatile uint32_t *dir;
     } gpio_bank[LINUX_GPIO_NUM_BANKS];

public:
    LinuxGPIO_IMX6();
    void    init();
    void    pinMode(uint8_t pin, uint8_t output);
    int8_t  analogPinToDigitalPin(uint8_t pin);
    uint8_t read(uint8_t pin);
    void    write(uint8_t pin, uint8_t value);
    void    toggle(uint8_t pin);

    /* Alternative interface: */
    AP_HAL::DigitalSource* channel(uint16_t n);

    /* Interrupt interface: */
    bool    attach_interrupt(uint8_t interrupt_num, AP_HAL::Proc p,
            uint8_t mode);

    /* return true if USB cable is connected */
    bool    usb_connected(void);
};

#endif // __AP_HAL_LINUX_GPIO_IMX6_H__
