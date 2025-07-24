/*
 * Copyright (c) 2021 Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */

#include "hardware/gpio.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "gpio-board.h"

static Gpio_t *current_gpio_obj = NULL;

void GpioMcuInit( Gpio_t *obj, PinNames pin, PinModes mode, PinConfigs config, PinTypes type, uint32_t value )
{
    obj->pin = pin;

    if( pin == NC )
    {
        return;
    }

    gpio_init(pin);

    if (mode == PIN_INPUT)
    {
        gpio_set_dir(pin, GPIO_IN);
    }
    else if (mode == PIN_OUTPUT)
    {
        gpio_set_dir(pin, GPIO_OUT);
    }

    if (config == PIN_PUSH_PULL)
    {
        if (type == PIN_NO_PULL)
        {
            gpio_disable_pulls(pin);
        }
        else if (type == PIN_PULL_UP)
        {
            gpio_pull_up(pin);
        }
        else if (type == PIN_PULL_DOWN)
        {
            gpio_pull_down(pin);
        }
    }

    if( mode == PIN_OUTPUT )
    {
        GpioMcuWrite( obj, value );
    }
}

void GpioMcuWrite( Gpio_t *obj, uint32_t value )
{
    gpio_put(obj->pin, value);
}

uint32_t GpioMcuRead( Gpio_t *obj )
{
    return gpio_get(obj->pin);
}

static void gpio_irq_wrapper(unsigned int gpio, unsigned long events) {
    if (current_gpio_obj != NULL && current_gpio_obj->IrqHandler != NULL) {
        current_gpio_obj->IrqHandler(NULL);
    }
}

void GpioMcuSetInterrupt( Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler )
{
    current_gpio_obj = obj;
    obj->IrqHandler = irqHandler;
    gpio_set_irq_enabled_with_callback(obj->pin, GPIO_IRQ_EDGE_RISE, true, gpio_irq_wrapper);
}

void GpioMcuRemoveInterrupt( Gpio_t *obj )
{
    gpio_set_irq_enabled(obj->pin,GPIO_IRQ_EDGE_RISE,false);
}

