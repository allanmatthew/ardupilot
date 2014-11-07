#include <AP_HAL.h>

#if CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_IMX6

#include "GPIO.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/mman.h>
#include <sys/stat.h>

using namespace Linux;

static const AP_HAL::HAL& hal = AP_HAL_BOARD_DRIVER;
LinuxGPIO_IMX6::LinuxGPIO_IMX6()
{}

void LinuxGPIO_IMX6::init()
{
    int mem_fd;

    /* open /dev/mem */
    if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
            printf("can't open /dev/mem \n");
            exit (-1);
    }

    /* mmap GPIO */
    off_t offsets[LINUX_GPIO_NUM_BANKS] = { GPIO0_BASE, GPIO1_BASE, GPIO2_BASE, GPIO3_BASE, GPIO4_BASE, GPIO5_BASE, GPIO6_BASE };
    for (uint8_t i=0; i<LINUX_GPIO_NUM_BANKS; i++) {
        gpio_bank[i].base = (volatile unsigned *)mmap(0, GPIO_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, offsets[i]);
        if ((char *)gpio_bank[i].base == MAP_FAILED) {
            hal.scheduler->panic("unable to map GPIO bank");
        }
        gpio_bank[i].dir = gpio_bank[i].base + GPIO_DIR;
        gpio_bank[i].dr = gpio_bank[i].base + GPIO_DR;
    }

    close(mem_fd);
}

void LinuxGPIO_IMX6::pinMode(uint8_t pin, uint8_t output)
{
    uint8_t bank = pin/32;
    uint8_t bankpin = pin & 0x1F;
    if (bank >= LINUX_GPIO_NUM_BANKS) {
        return;
    }
    if (output == HAL_GPIO_OUTPUT) {
        *gpio_bank[bank].dir |= (1U<<bankpin);
    } else {
        *gpio_bank[bank].dir &= ~(1U<<bankpin);
    }
}

int8_t LinuxGPIO_IMX6::analogPinToDigitalPin(uint8_t pin)
{
    return -1;
}


uint8_t LinuxGPIO_IMX6::read(uint8_t pin) {

    uint8_t bank = pin/32;
    uint8_t bankpin = pin & 0x1F;
    if (bank >= LINUX_GPIO_NUM_BANKS) {
        return 0;
    }
    return *gpio_bank[bank].dr & (1U<<bankpin) ? HIGH : LOW;

}

void LinuxGPIO_IMX6::write(uint8_t pin, uint8_t value)
{
    uint8_t bank = pin/32;
    uint8_t bankpin = pin & 0x1F;
    if (bank >= LINUX_GPIO_NUM_BANKS) {
        return;
    }
    if (value == LOW) {
        *gpio_bank[bank].dr &= ~(1U<<bankpin);
    } else {
        *gpio_bank[bank].dr |= 1U<<bankpin;
    }
}

void LinuxGPIO_IMX6::toggle(uint8_t pin)
{
    write(pin, !read(pin));
}

/* Alternative interface: */
AP_HAL::DigitalSource* LinuxGPIO_IMX6::channel(uint16_t n) {
    return new LinuxDigitalSource(n);
}

/* Interrupt interface: */
bool LinuxGPIO_IMX6::attach_interrupt(uint8_t interrupt_num, AP_HAL::Proc p, uint8_t mode)
{
    return true;
}

bool LinuxGPIO_IMX6::usb_connected(void)
{
    return false;
}

#endif // CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_IMX6
