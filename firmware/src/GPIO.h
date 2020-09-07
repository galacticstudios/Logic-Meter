
#ifndef _GPIO_H
#define _GPIO_H

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

static inline void GPIO_PortWPUEnable(GPIO_PORT port, uint32_t mask)
{
    *(volatile uint32_t *)(&CNPUBSET + (port * 0x40)) = mask;
}

static inline void GPIO_PortWPUDisable(GPIO_PORT port, uint32_t mask)
{
    *(volatile uint32_t *)(&CNPUBCLR + (port * 0x40)) = mask;
}

static inline void GPIO_PinWPUEnable(GPIO_PIN pin)
{
    GPIO_PortWPUEnable((GPIO_PORT) (pin>>4), (uint32_t)(0x1) << (pin & 0xF));
}

static inline void GPIO_PinWPUDisable(GPIO_PIN pin)
{
    GPIO_PortWPUDisable((GPIO_PORT) (pin>>4), (uint32_t)(0x1) << (pin & 0xF));
}

static inline void GPIO_PortWPDEnable(GPIO_PORT port, uint32_t mask)
{
    *(volatile uint32_t *)(&CNPDBSET + (port * 0x40)) = mask;
}

static inline void GPIO_PortWPDDisable(GPIO_PORT port, uint32_t mask)
{
    *(volatile uint32_t *)(&CNPDBCLR + (port * 0x40)) = mask;
}

static inline void GPIO_PinWPDEnable(GPIO_PIN pin)
{
    GPIO_PortWPDEnable((GPIO_PORT) (pin>>4), (uint32_t)(0x1) << (pin & 0xF));
}

static inline void GPIO_PinWPDDisable(GPIO_PIN pin)
{
    GPIO_PortWPDDisable((GPIO_PORT) (pin>>4), (uint32_t)(0x1) << (pin & 0xF));
}

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
