/*
 * GPIO.cpp
 *
 *  Created on: 10 февр. 2018 г.
 *      Author: Devil
 */

#include <TIMER.h>


Timer* Timer::_instances[TIMER_BLOCKS] = { 0 };
Multi_delegate*  Timer::ISR = new Multi_delegate(0);
const uint32_t Timer::BLOCK_ADDRESSES[TIMER_BLOCKS] = {SYS_TICK, TIM1,TIM2,TIM3,TIM4,TIM5,TIM6,TIM7,TIM8,TIM9,TIM10,TIM11,TIM12,TIM13,TIM14};
const rcc_periph_clken Timer::RCC_ADDRESSES[TIMER_BLOCKS] = {RCC_TIM1,RCC_TIM2,RCC_TIM3,RCC_TIM4,RCC_TIM5,RCC_TIM6,RCC_TIM7,RCC_TIM8,RCC_TIM9,RCC_TIM10,RCC_TIM11,RCC_TIM12,RCC_TIM13,RCC_TIM14};

Timer::Timer(uint8_t block)
{
	_block_number = block;
	_block_address = BLOCK_ADDRESSES[_block_number];
	_block_mode = MANUAL_MODE;
}

virtual uint8_t Timer::is_accessible()
{
	if((_block_number == TIMER6) || (_block_number == TIMER7))
		return true;
	else
		return false;
}

Timer::~Timer() {}

static void Timer::search_flags(uint8_t start, uint8_t finish)
{
	for(uint8_t bit = start; bit<=finish; bit++)
	{
		if(EXTI_PR & (1<<bit))
			invoke_ISR(bit);
	}
}

static void Timer::invoke_ISR(uint8_t EXTI_line)
{
	EXTI_PR |= (1<<EXTI_line);

	ISR->call(EXTI_line);
}

virtual void Timer::enable()
{
	rcc_periph_clock_enable((rcc_periph_clken)RCC_ADDRESSES[_block_number]);
}

void virtual Timer::set_mode(uint8_t mode)
{
	//if(pin>=8)
	//	GPIO_CRH(_block_address) = ((GPIO_CRH(_block_address) & ~(0x0F<<(((pin-8)%8)*4))) | (mode<<(((pin-8)%8)*4)));
	//else if(pin<8)
	//	GPIO_CRL(_block_address) = ((GPIO_CRL(_block_address) & ~(0x0F<<((pin%8)*4))) | (mode<<((pin%8)*4)));
}




virtual uint8_t Basic_timer::is_accessible()
{
	if((_block_number == TIMER6) || (_block_number == TIMER7))
		return true;
	else
		return false;
}
