/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: Generic GPIO driver implementation

Comment: Relies on the specific board GPIO implementation as well as on
         IO expander driver implementation if one is available on the target
         board.

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#ifndef __GPIO_H__
#define __GPIO_H__

/*!
 * Operation Mode for the GPIO
 */
typedef enum
{
    PIN_INPUT = 0,
    PIN_OUTPUT,
    PIN_ALTERNATE_FCT,
    PIN_ANALOGIC
}PinModes;

/*!
 * Add a pull-up, a pull-down or nothing on the GPIO line
 */
typedef enum
{
    PIN_NO_PULL = 0,
    PIN_PULL_UP,
    PIN_PULL_DOWN
}PinTypes;

/*!
 * Define the GPIO as Push-pull type or Open Drain
 */
typedef enum
{
    PIN_PUSH_PULL = 0,
    PIN_OPEN_DRAIN
}PinConfigs;

/*!
 * Define the GPIO IRQ on a rising, falling or both edges
 */
typedef enum
{
    NO_IRQ = 0,
    IRQ_RISING_EDGE,
    IRQ_FALLING_EDGE,
    IRQ_RISING_FALLING_EDGE
}IrqModes;

/*!
 * Define the IRQ priority on the GPIO
 */
typedef enum
{
    IRQ_VERY_LOW_PRIORITY = 15,
    IRQ_LOW_PRIORITY = 12,
    IRQ_MEDIUM_PRIORITY = 8,
    IRQ_HIGH_PRIORITY = 4,
    IRQ_VERY_HIGH_PRIORITY = 0
}IrqPriorities;

/*!
 * Structure for the GPIO
 */
typedef struct
{
    uint8_t pinIndex;
    uint8_t portIndex;
}Gpio_t;

/*!
 * GPIO IRQ handler function prototype
 */
typedef void(GpioIrqHandler)(void);

/*!
 * \brief Initializes the given GPIO object
 *
 * \param [IN] obj    Pointer to the GPIO object
 * \param [IN] mode   Pin mode [PIN_INPUT, PIN_OUTPUT,
 *                              PIN_ALTERNATE_FCT, PIN_ANALOGIC]
 * \param [IN] config Pin config [PIN_PUSH_PULL, PIN_OPEN_DRAIN]
 * \param [IN] type   Pin type [PIN_NO_PULL, PIN_PULL_UP, PIN_PULL_DOWN]
 * \param [IN] value  Default output value at initialisation
 */
void GpioInit(Gpio_t *obj, PinModes mode, PinConfigs config, PinTypes type, uint8_t value);

/*!
 * \brief Deinitializes the given GPIO object
 *
 * \param [IN] obj    Pointer to the GPIO object
 */
void GpioDeInit(Gpio_t *obj);

/*!
 * \brief GPIO IRQ Initialization
 *
 * \param [IN] obj         Pointer to the GPIO object
 * \param [IN] irqMode     IRQ mode [NO_IRQ, IRQ_RISING_EDGE,
 *                                   IRQ_FALLING_EDGE, IRQ_RISING_FALLING_EDGE]
 * \param [IN] irqPriority IRQ priority [IRQ_VERY_LOW_PRIORITY, IRQ_LOW_PRIORITY
 *                                       IRQ_MEDIUM_PRIORITY, IRQ_HIGH_PRIORITY
 *                                       IRQ_VERY_HIGH_PRIORITY]
 * \param [IN] irqHandler  Callback function pointer
 */
void GpioSetInterrupt(Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler);

/*!
 * \brief Removes the interrupt from the object
 *
 * \param [IN] obj Pointer to the GPIO object
 */
void GpioRemoveInterrupt(Gpio_t *obj);

/*!
 * \brief Writes the given value to the GPIO output
 *
 * \param [IN] obj   Pointer to the GPIO object
 * \param [IN] value New GPIO output value
 */
void GpioWrite(Gpio_t *obj, uint8_t value);

/*!
 * \brief Reads the current GPIO input value
 *
 * \param [IN] obj Pointer to the GPIO object
 * \retval value   Current GPIO input value
 */
uint8_t GpioRead(Gpio_t *obj);

#endif // __GPIO_H__
