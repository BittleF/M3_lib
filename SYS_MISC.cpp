/*
 * SYS_MISC.cpp
 *
 *  Created on: 8 февр. 2018 г.
 *      Author: Devil
 */

//#include <array.h>
#include <SYS_MISC.h>



Multi_delegate::Multi_delegate(uint8_t capacity) :_capacity(capacity)
{
	_pointers = new Base_container* [_capacity];
	for(uint8_t i=0; i<_capacity; i++)
		_pointers[i] = 0;
}

void Multi_delegate::call(uint8_t number)
{
	if (_pointers[number])
		_pointers[number]->Execute();
}

