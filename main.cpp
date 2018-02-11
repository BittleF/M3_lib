/*
 * main.cpp
 *
 *  Created on: 27 дек. 2017 г.
 *      Author: Devil
 */

#define RCC_BLOCKS	1
#define ADC_BLOCKS 	2



#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>

#include <GPIO.h>
#include <TIMER.h>
#include <SYS_LOG.h>
#include <SYS_MISC.h>


template<class device_type>
static device_type* add(uint8_t block)
{
	if (!(device_type::_instances[block]))
	{
		device_type::_instances[block] = new device_type(block);
		if(!(device_type::_instances[block]->is_accessible()))
		{
			delete device_type::_instances[block];
			device_type::_instances[block] = 0;
			Sys_log::push_error(ERR_CREATION_FAILED);
		}
		else
			device_type::_instances[block]-> enable();
	}
	return device_type::_instances[block];
}


void exti9_5_isr(void)
{
	GPIO::search_flags(5,9);
}

class Logic
{
public:
	Logic()
	{
		rcc_clock_setup_in_hsi_out_64mhz();

		port_A = add<GPIO>(PORTA);
		port_A->set_mode(PIN7, GPIO_IN_PULL_UPDN);
		port_A->set_mode(PIN8, GPIO_ALT_PUSHPULL);
		port_A->set(PIN7);

		port_B = add<GPIO>(PORTB);
		port_B->set_mode(PIN0, GPIO_OUT_PUSHPULL);
		port_B->set_mode(PIN13, GPIO_ALT_PUSHPULL);
		//port_B->set(PIN0);

		port_A->set_irq(PIN7, EXTI_TRIGGER_FALLING, this, &Logic::test);
		//port_B->set_irq(PIN0, EXTI_TRIGGER_FALLING, this, &Logic::test);
	}

	void test()
	{
		//port_A->toggle(PIN7);
		port_B->toggle(PIN0);
	}
private:
	GPIO* port_A;
	GPIO* port_B;

};

int main(void)
{
	Logic Main_logic;
	//rcc_periph_clock_enable(RCC_GPIOB);
	//rcc_periph_clock_enable(RCC_AFIO);
	//rcc_periph_clock_enable(RCC_TIM1);

	//gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO7);
    //gpio_set(GPIOA, GPIO7);
	//gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO0);
	//nvic_enable_irq(NVIC_EXTI9_5_IRQ);
	//exti_set_trigger(EXTI7, EXTI_TRIGGER_BOTH);
	//exti_enable_request(EXTI7);
	//nvic_generate_software_interrupt(NVIC_EXTI9_5_IRQ);
	//EXTI_SWIER |= (1<<7);
	//etl::array<uint32_t, 10> data;
    while(1)
    {
    	//if(gpio_get(GPIOA, GPIO7) == 0)
    	//	gpio_set(GPIOB, GPIO0);
    	//else
    	//	gpio_clear(GPIOB, GPIO0);
        //gpio_toggle(GPIOA, GPIO6);
        //for (int i = 0; i < 500000; i++)    /* Wait a bit. */
        //    __asm__("nop");
	}

	return 0;
}

