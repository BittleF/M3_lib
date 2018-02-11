/*
 * GPIO.h
 *
 *  Created on: 10 февр. 2018 г.
 *      Author: Devil
 */

#ifndef GPIO_H_
#define GPIO_H_

#define TIMER_BLOCKS		14

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/systick.h>

#include <SYS_LOG.h>
#include <SYS_MISC.h>

enum TIMER_blocks
{
	SYSTICK, TIMER1, TIMER2, TIMER3, TIMER4, TIMER5, TIMER6, TIMER7, TIMER8, TIMER9, TIMER10, TIMER11, TIMER12, TIMER13, TIMER14
};
enum TIMER_state
{
	MANUAL_MODE, TIMEBASE_MODE, PWM_MODE, INPUT_MODE, ENCODER_MODE
};

typedef struct block_parameters
{
	uint8_t _mode;
	float frequency;
} block_param;


class Timer
{
private:
	static const uint32_t BLOCK_ADDRESSES[TIMER_BLOCKS];
	static const uint32_t RCC_ADDRESSES[TIMER_BLOCKS];
	static Timer* _instances[];
	static Multi_delegate* ISR;
	uint32_t _block_address;
	uint32_t _block_number;
	block_param parameters;

	friend Timer* add<Timer>(uint8_t block);
	Timer(uint8_t block);
	virtual uint8_t is_accessible();
	virtual ~Timer();
public:
	static void search_flags(uint8_t start, uint8_t finish);
	static void invoke_ISR(uint8_t EXTI_line);

	virtual void enable();
	virtual void set_mode(uint8_t mode);
	virtual void start();

	template<class class_target, class method_target>
	void set_irq(uint8_t pin, uint8_t edge, class_target* target_this, method_target method)
	{
		if(pin>=16)
			return;
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

class Basic_timer: public Timer
{
	virtual uint8_t is_accessible();
};


#endif /* GPIO_H_ */
