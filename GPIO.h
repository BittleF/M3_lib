/*
 * GPIO.h
 *
 *  Created on: 10 февр. 2018 г.
 *      Author: Devil
 */

#ifndef GPIO_H_
#define GPIO_H_

#define GPIO_BLOCKS	7

#define GPIO_PINS	16
#define PIN_ALL		0xffff

#define GPIO_FLOAT				(GPIO_CNF_INPUT_FLOAT<<2)				|GPIO_MODE_INPUT
#define GPIO_IN_ANALOG			(GPIO_CNF_INPUT_ANALOG<<2)				|GPIO_MODE_INPUT
#define GPIO_IN_PULL_UPDN		(GPIO_CNF_INPUT_PULL_UPDOWN<<2)			|GPIO_MODE_INPUT
#define GPIO_OUT_PUSHPULL		(GPIO_CNF_OUTPUT_PUSHPULL<<2)			|GPIO_MODE_OUTPUT_10_MHZ
#define GPIO_OUT_OPENDRAIN		(GPIO_CNF_OUTPUT_OPENDRAIN<<2)			|GPIO_MODE_OUTPUT_10_MHZ
#define GPIO_ALT_PUSHPULL		(GPIO_CNF_OUTPUT_ALTFN_PUSHPULL<<2)		|GPIO_MODE_OUTPUT_10_MHZ
#define GPIO_ALT_OPENDRAIN		(GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN<<2)	|GPIO_MODE_OUTPUT_10_MHZ

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#include <SYS_LOG.h>
#include <SYS_MISC.h>

enum GPIO_blocks
{
	PORTA, PORTB, PORTC, PORTD, PORTE, PORTF, PORTG
};
enum GPIO_pins
{
	PIN0, PIN1, PIN2, PIN3, PIN4, PIN5, PIN6, PIN7, PIN8, PIN9, PIN10, PIN11, PIN12, PIN13, PIN14, PIN15
};

class GPIO
{
private:
	static const uint32_t BLOCK_ADDRESSES[GPIO_BLOCKS];
	static const uint32_t RCC_ADDRESSES[GPIO_BLOCKS];
	static GPIO* _instances[];
	static Multi_delegate* ISR;
	static uint8_t _AFIO_use;
	uint32_t _block_address;
	uint32_t _block_number;

	friend GPIO* add<GPIO>(uint8_t block);
	GPIO(uint8_t block);
	uint8_t is_accessible();
public:
	static void search_flags(uint8_t start, uint8_t finish);
	static void invoke_ISR(uint8_t EXTI_line);

	void enable();
	void AFIO_enable();
	void set_mode(uint8_t pin, uint8_t mode);
	void set(uint8_t pin);
	void toggle(uint8_t pin);
	void clear(uint8_t pin);

	template<class class_target, class method_target>
	void set_irq(uint8_t pin, uint8_t edge, class_target* target_this, method_target method)
	{
		if(pin>=16)
			return;
		AFIO_enable();
		if(pin>=10)
			nvic_enable_irq(NVIC_EXTI15_10_IRQ);
		else if(pin>=5)
			nvic_enable_irq(NVIC_EXTI9_5_IRQ);
		else
			nvic_enable_irq(EXTI0+pin);

		AFIO_EXTICR(pin/4) = ((AFIO_EXTICR(pin/4) & ~(0x0F<<((pin%4)*4)))) | (_block_number<<((pin%4)*4));	//Choose the port
		exti_set_trigger((1<<pin), (exti_trigger_type)edge);
		exti_enable_request(1<<pin);

		ISR->add<class_target, method_target>(pin, target_this, method);
	}
};


#endif /* GPIO_H_ */
