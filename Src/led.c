#include <stdint.h>
#include "led.h"


// Initialize all LEDs (already provided)
void led_init_all(void)
{
    uint32_t *pRccAhb1enr = (uint32_t*)0x40023830;     // RCC AHB1ENR
    uint32_t *pGpiodModeReg = (uint32_t *)0x40020c00; // GPIOD MODER

    // Enable clock for GPIOD
    *pRccAhb1enr |= (1 << 3);

    // Configure LED pins as output (MODER = 01)
    *pGpiodModeReg &= ~(0x3 << (2 * LED_GREEN));  // Clear previous mode
    *pGpiodModeReg |=  (0x1 << (2 * LED_GREEN));

    *pGpiodModeReg &= ~(0x3 << (2 * LED_ORANGE));
    *pGpiodModeReg |=  (0x1 << (2 * LED_ORANGE));

    *pGpiodModeReg &= ~(0x3 << (2 * LED_RED));
    *pGpiodModeReg |=  (0x1 << (2 * LED_RED));

    *pGpiodModeReg &= ~(0x3 << (2 * LED_BLUE));
    *pGpiodModeReg |=  (0x1 << (2 * LED_BLUE));
}

// Turn on a specific LED
void led_on(uint8_t led_no)
{
    uint32_t *pGpiodOdr = (uint32_t*)0x40020C14; // GPIOD ODR
    *pGpiodOdr |= (1 << led_no);
}

// Turn off a specific LED
void led_off(uint8_t led_no)
{
    uint32_t *pGpiodOdr = (uint32_t*)0x40020C14; // GPIOD ODR
    *pGpiodOdr &= ~(1 << led_no);
}
