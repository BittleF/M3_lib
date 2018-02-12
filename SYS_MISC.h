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
#include <SYS_LOG.h>


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
			device_type::_instances[block]->enable();
	}
	return device_type::_instances[block];
}


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
	Base_container** _pointers = 0;

public:
	Multi_delegate(uint8_t capacity);

	template<class class_target, class method_target>
	void add(uint8_t number, class_target* target_this, method_target method)
	{
		if(number<=MAX_CAPACITY)
		{
			if(_pointers[number])
			{
				delete _pointers[number];
			}
			_pointers[number] = new Container<class_target, method_target>(target_this, method);
		}
	}
	void call(uint8_t number);
};

#endif /* SYS_MISC_H_ */
