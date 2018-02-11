/*
 * GPIO.cpp
 *
 *  Created on: 10 февр. 2018 г.
 *      Author: Devil
 */

#include <GPIO.h>


GPIO* GPIO::_instances[GPIO_BLOCKS] = { 0 };
Multi_delegate*  GPIO::ISR = new Multi_delegate(0);
uint8_t GPIO::_AFIO_use = 0;
const uint32_t GPIO::BLOCK_ADDRESSES[GPIO_BLOCKS] = {GPIOA,GPIOB,GPIOC,GPIOD,GPIOE,GPIOF,GPIOG};
const rcc_periph_clken GPIO::RCC_ADDRESSES[GPIO_BLOCKS] = {RCC_GPIOA,RCC_GPIOB,RCC_GPIOC,RCC_GPIOD,RCC_GPIOE,RCC_GPIOF,RCC_GPIOG};

GPIO::GPIO(uint8_t block)
{
	_block_number = block;
	_block_address = BLOCK_ADDRESSES[_block_number];
}

uint8_t GPIO::is_accessible()
{
	if(_block_number <= 15)
		return true;
	else
		return false;
}

static void GPIO::search_flags(uint8_t start, uint8_t finish)
{
	for(uint8_t bit = start; bit<=finish; bit++)
	{
		if(EXTI_PR & (1<<bit))
			invoke_ISR(bit);
	}
}

static void GPIO::invoke_ISR(uint8_t EXTI_line)
{
	EXTI_PR |= (1<<EXTI_line);

	ISR->call(EXTI_line);
}

void GPIO::enable()
{
	rcc_periph_clock_enable((rcc_periph_clken)RCC_ADDRESSES[_block_number]);
}

void GPIO::AFIO_enable()
{
	if(_AFIO_use == 0)
		rcc_periph_clock_enable(RCC_AFIO);
	_AFIO_use++;
}

void GPIO::set_mode(uint8_t pin, uint8_t mode)
{
	if(pin>=8)
		GPIO_CRH(_block_address) = ((GPIO_CRH(_block_address) & ~(0x0F<<(((pin-8)%8)*4))) | (mode<<(((pin-8)%8)*4)));
	else if(pin<8)
		GPIO_CRL(_block_address) = ((GPIO_CRL(_block_address) & ~(0x0F<<((pin%8)*4))) | (mode<<((pin%8)*4)));
}

void GPIO::set(uint8_t pin)
{
	gpio_set(_block_address, (1<<pin));
}

void GPIO::toggle(uint8_t pin)
{
	gpio_toggle(_block_address, (1<<pin));
}

void GPIO::clear(uint8_t pin)
{
	gpio_clear(_block_address, (1<<pin));
}


