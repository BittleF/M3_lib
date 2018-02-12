/*
 * TIMER.cpp
 *
 *  Created on: 10 февр. 2018 г.
 *      Author: Devil
 */

#include <TIMER.h>


Timer* Timer::_instances[TIMER_BLOCKS] = { 0 };
const uint32_t Timer_interface::BLOCK_ADDRESSES[TIMER_BLOCKS] = {SYS_TICK_BASE, TIM1,TIM2,TIM3,TIM4,TIM5,TIM6,TIM7,TIM8,TIM9,TIM10,TIM11,TIM12,TIM13,TIM14};
const uint32_t Timer_interface::RCC_ADDRESSES[TIMER_BLOCKS] = {0, RCC_TIM1, RCC_TIM2,RCC_TIM3,RCC_TIM4,RCC_TIM5,RCC_TIM6,RCC_TIM7,RCC_TIM8,RCC_TIM9,RCC_TIM10,RCC_TIM11,RCC_TIM12,RCC_TIM13,RCC_TIM14};

Timer_interface::Timer_interface()
{
	_block_number = 0;
		_block_address = 0;
}
Timer_interface::Timer_interface(uint8_t block)
{
	_block_number = 0;
		_block_address = 0;
}
void Timer_interface::enable(){}
void Timer_interface::reset(){}
void Timer_interface::set_period(timer_parameters config){}
void Timer_interface::channel_mode(uint8_t channel, channel_parameters config){}
void Timer_interface::set_irq_hardware(uint8_t interrupt) {}
void Timer_interface::start(){}
void Timer_interface::start_single(){}

Timer_interface::~Timer_interface() {}

// systick
Timer_sys::Timer_sys(uint8_t block)
{
	_block_number = block;
	_block_address = BLOCK_ADDRESSES[_block_number];
}
void Timer_sys::set_period(timer_parameters config)
{
	uint32_t reload;
	reload = (uint32_t)rcc_ahb_frequency/config.frequency;
	if(reload >= 0x00FFFFFF)
	{
		reload /= 8;
		if(reload >= 0x00FFFFFF)
		{
			Sys_log::push_error(ERR_OUT_OF_RANGE);
			return;
		}
		else
			systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);
	}
	else
		systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	_block_parameters.mode = config.mode;
	_block_parameters.frequency = config.frequency;
	systick_set_reload(reload - 1);
}
void Timer_sys::set_irq_hardware(uint8_t interrupt)
{
	systick_interrupt_enable();
}
void Timer_sys::start()
{
	systick_counter_enable();
}
//void Timer_sys::invoke_ISR(uint8_t block, uint8_t interrupt)
//{
//	_instances[block]->ISR->call(interrupt);
//}

// timer6, timer7
Timer_basic::Timer_basic()
{
	_block_number = 0;
		_block_address = 0;
}
Timer_basic::Timer_basic(uint8_t block) {}
void Timer_basic::enable()
{
	rcc_periph_clock_enable((rcc_periph_clken)RCC_ADDRESSES[_block_number]);
}
void Timer_basic::reset()
{
	if(_RCC_REG(RCC_ADDRESSES[_block_number]) == RCC_APB1ENR)
	{
		RCC_APB1RSTR |=  _RCC_BIT(RCC_ADDRESSES[_block_number]);
		RCC_APB1RSTR &= ~_RCC_BIT(RCC_ADDRESSES[_block_number]);
	}
	else if(_RCC_REG(RCC_ADDRESSES[_block_number]) == RCC_APB2ENR)
	{
		RCC_APB2RSTR |=  _RCC_BIT(RCC_ADDRESSES[_block_number]);
		RCC_APB2RSTR &= ~_RCC_BIT(RCC_ADDRESSES[_block_number]);
	}
}
void Timer_basic::start()
{
	TIM_CR1(_block_address) &= ~TIM_CR1_OPM;
	TIM_CR1(_block_address) |=  TIM_CR1_CEN;
}
void Timer_basic::start_single()
{
	TIM_CR1(_block_address) |=  TIM_CR1_OPM;
	TIM_CR1(_block_address) |=  TIM_CR1_CEN;
}
void Timer_basic::set_period(timer_parameters config)
{
	uint32_t reload;
	uint32_t clock;

	if(_RCC_REG(RCC_ADDRESSES[_block_number]) == RCC_APB1ENR)
		clock = rcc_apb1_frequency;
	else if(_RCC_REG(RCC_ADDRESSES[_block_number]) == RCC_APB2ENR)
		clock = rcc_apb2_frequency;
	else
	{
		Sys_log::push_error(ERR_PARAMETER_INCORRECT);
		return;
	}

	reload = (uint32_t)clock/config.frequency;
	if(reload > 0xFFFF)
	{
		reload /= 0xFFFF;
		if(reload >= 0xFFFF)
		{
			Sys_log::push_error(ERR_OUT_OF_RANGE);
			return;
		}
		else
		{
			TIM_ARR(_block_address)=0xFFFF;
			TIM_PSC(_block_address)=(reload-1);
		}
	}
	else
	{
		TIM_ARR(_block_address)=reload;
		TIM_PSC(_block_address)=0;
	}
	_block_parameters.mode = config.mode;
	_block_parameters.frequency = config.frequency;
}
void Timer_basic::set_irq_hardware(uint8_t interrupt)
{

}

// timer9, timer12
//Timer_general_2ch::Timer_general_2ch() {}
Timer_general_2ch::Timer_general_2ch(uint8_t block) {}
void Timer_general_2ch::channel_mode(uint8_t channel, channel_parameters config) {}

// timer2 - timer5
Timer_general_4ch::Timer_general_4ch(uint8_t block)
{
	_block_number = block;
	_block_address = BLOCK_ADDRESSES[_block_number];
}
void Timer_general_4ch::channel_mode(uint8_t channel, channel_parameters config) {}

Timer_advance::Timer_advance(uint8_t block)
{
	_block_number = block;
	_block_address = BLOCK_ADDRESSES[_block_number];
}
void Timer_advance::channel_mode(uint8_t channel, channel_parameters config) {}


Timer::Timer(uint8_t block)
{
	_block_number = block;
	_created_properly = true;
	switch (_block_number)
	{
	case SYSTICK:
		instance = new Timer_sys(_block_number);
		ISR = new Multi_delegate(SYSTICK_INTERRUPTS);
		break;
	case TIMER1:
	case TIMER8:
		instance = new Timer_advance(_block_number);
		ISR = new Multi_delegate(ADVANCED_TIM_INTERRUPTS);
		break;
	case TIMER2:
	case TIMER3:
	case TIMER4:
	case TIMER5:
		instance = new Timer_general_4ch(_block_number);
		ISR = new Multi_delegate(GENERAL_TIM_INTERRUPTS);
		break;
	case TIMER9:
	case TIMER12:
		instance = new Timer_general_2ch(_block_number);
		ISR = new Multi_delegate(GENERAL_TIM_INTERRUPTS);
		break;
	case TIMER6:
	case TIMER7:
		instance = new Timer_basic(_block_number);
		ISR = new Multi_delegate(ADVANCED_TIM_INTERRUPTS);
		break;
	default:
		_created_properly = false;
		instance = new Timer_interface(_block_number);
		break;
	}
}
bool Timer::is_accessible()
{
	return _created_properly;
}

void Timer::enable()
{
	instance->enable();
}
void Timer::reset()
{
	instance->reset();
}
void Timer::set_period(timer_parameters config)
{
	instance->set_period(config);
}
void Timer::channel_mode(uint8_t channel, channel_parameters config)
{
	instance->channel_mode(channel, config);
}
void Timer::start()
{
	instance->start();
}
void Timer::start_single()
{
	instance->start_single();
}
void Timer::invoke_ISR(uint8_t block, uint8_t interrupt)
{
	_instances[block]->ISR->call(UPDATE_INT);
}
Timer::~Timer(){}

// timer1, timer8
void sys_tick_handler(void)
{
	Timer::invoke_ISR(SYSTICK, UPDATE_INT);
}
