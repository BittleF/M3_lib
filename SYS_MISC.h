/*
 * SYS_MISC.h
 *
 *  Created on: 7 февр. 2018 г.
 *      Author: Devil
 */

#ifndef SYS_MISC_H_
#define SYS_MISC_H_

#define MAX_CAPACITY 16

#include <stdint.h>

class Base_container
{
public:
	virtual void Execute() {};
	virtual ~Base_container() {};		//Without this line compilator gets warning "Class 'Container' has virtual method 'ISR_execute' but non-virtual destructor" interesting...
};

template <class target_class, class target_method>
class Container : public Base_container
{
public:
	Container(target_class* target_pointer, target_method method)
		:_target_class(target_pointer), _target_method(method) {}	// Member direct init list

	void Execute()
	{
		(_target_class->*_target_method)();
	}
private:
	target_class* _target_class;
	target_method _target_method;
};

class Multi_delegate
{
private:
	uint8_t _capacity;
	Base_container* _pointers[MAX_CAPACITY] = {0};

public:
	Multi_delegate(uint8_t capacity);

	template<class class_target, class method_target>
	void add(uint8_t number, class_target* target_this, method_target method)
	{
		if(number<=MAX_CAPACITY)
		{
			if(_pointers[number] != 0)
				delete _pointers[number];
			_pointers[number] = new Container<class_target, method_target>(target_this, method);
		}
	}
	void call(uint8_t number);
};

#endif /* SYS_MISC_H_ */
