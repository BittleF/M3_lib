/*
 * SYS_MISC.cpp
 *
 *  Created on: 8 февр. 2018 г.
 *      Author: Devil
 */

//#include <array.h>
#include <SYS_MISC.h>



Multi_delegate::Multi_delegate(uint8_t capacity) :_capacity(capacity)
{}

void Multi_delegate::call(uint8_t number)
{
	if (_pointers[number] != 0)
		_pointers[number]->Execute();
}

