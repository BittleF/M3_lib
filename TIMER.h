/*
 * TIMER.h
 *
 *  Created on: 10 февр. 2018 г.
 *      Author: Devil
 */

#ifndef TIMER_H_
#define TIMER_H_

#define TIMER_BLOCKS			15
#define SYSTICK_INTERRUPTS		1
#define BASIC_TIM_INTERRUPTS	1
#define GENERAL_TIM_INTERRUPTS	6
#define ADVANCED_TIM_INTERRUPTS	8

#define UPDATE_INT 		0
#define COMPARE1_INT 	1
#define COMPARE2_INT 	2
#define COMPARE3_INT 	3
#define COMPARE4_INT 	4
#define COM_INT			5
#define TRIGGER_INT		6
#define BREAK_INT		7


#define _RCC_REG(i)		MMIO32(RCC_BASE + ((i) >> 5))
#define _RCC_BIT(i)		(1 << ((i) & 0x1f))
#define TIM_CCR(tim_base, channel)		MMIO32((tim_base) + 0x34 + (channel*4))
#define TIM_CCMR(tim_base, channel)		MMIO32((tim_base) + 0x18 + ((channel/2)*4))

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>

#include <SYS_LOG.h>
#include <SYS_MISC.h>

enum TIMER_blocks
{
	SYSTICK, TIMER1, TIMER2, TIMER3, TIMER4, TIMER5, TIMER6, TIMER7, TIMER8, TIMER9, TIMER10, TIMER11, TIMER12, TIMER13, TIMER14
};
//enum TIMER_mode
//{
//	MANUAL_MODE, TIMEBASE_MODE, PWM_MODE, INPUT_MODE, ENCODER_MODE
//};
//enum TIMER_interrupts
//{
//	UPDATE_INT, COMPARE1_INT, COMPARE2_INT, COMPARE3_INT, COMPARE4_INT, COM_INT, TRIGGER_INT, BREAK_INT
//};
enum CHANNEL_mode
{
	MANUAL_MODE, INPUT_MODE, PWM_MODE, ENCODER_MODE
};

typedef struct timer_param
{
	uint8_t mode;
	uint8_t direction;
	float frequency;
} timer_parameters;
typedef struct channel_param
{
	uint8_t mode;
	uint8_t polarity;
	float duty;
} channel_parameters;


class Timer_interface
{
protected:
	static const uint32_t BLOCK_ADDRESSES[TIMER_BLOCKS];
	static const uint32_t RCC_ADDRESSES[TIMER_BLOCKS];
	uint32_t _block_address;
	uint8_t _block_number;

	timer_param _block_parameters;
public:
	Timer_interface();
	Timer_interface(uint8_t block);
	virtual void enable();
	virtual void reset();
	virtual void set_period(timer_parameters config);
	virtual void channel_mode(uint8_t channel, channel_parameters config);
	virtual void set_irq_hardware(uint8_t interrupt);
	virtual void start();
	virtual void start_single();
	static void search_flags(uint8_t block);

	virtual ~Timer_interface();
};

class Timer_sys: public Timer_interface
{
public:
	Timer_sys(uint8_t block);

	virtual void enable() {}
	virtual void reset() {}
	virtual void set_period(timer_parameters config);
	virtual void set_irq_hardware(uint8_t interrupt);
	virtual void start();
};
class Timer_basic: public Timer_interface
{
public:
	Timer_basic();
	Timer_basic(uint8_t block);

	virtual void enable();
	virtual void reset();
	virtual void set_period(timer_parameters config);
	virtual void set_irq_hardware(uint8_t interrupt);
	virtual void start();
	virtual void start_single();
};
class Timer_general_2ch : public Timer_basic
{
public:
	Timer_general_2ch(uint8_t block);

	virtual void channel_mode(uint8_t channel, channel_parameters config);
};
class Timer_general_4ch : public Timer_basic
{
public:
	Timer_general_4ch(uint8_t block);

	virtual void channel_mode(uint8_t channel, channel_parameters config);
};
class Timer_advance : public Timer_basic
{
public:
	Timer_advance(uint8_t block);

	virtual void channel_mode(uint8_t channel, channel_parameters config);
};


class Timer : public Timer_interface
{
private:
	Timer(uint8_t block);
	bool is_accessible();
	bool _created_properly;
	static Timer* _instances[];
	friend Timer* add<Timer>(uint8_t block);
	Timer_interface* instance;
	Multi_delegate* ISR;
public:
	~Timer();
	virtual void enable();
	virtual void reset();
	virtual void set_period(timer_parameters config);
	virtual void channel_mode(uint8_t channel, channel_parameters config);
	virtual void start();
	virtual void start_single();
	template<class class_target, class method_target>
	void set_irq(uint8_t interrupt, class_target* target_this, method_target method)
	{
		instance->set_irq_hardware(interrupt);
		ISR->add<class_target, method_target>(interrupt, target_this, method);
	}
	static void search_flags(uint8_t block);
	static void invoke_ISR(uint8_t block, uint8_t interrupt);
};

#endif /* TIMER_H_ */
