/*
 * SYS_LOG.cpp
 *
 *  Created on: 8 февр. 2018 г.
 *      Author: Devil
 */
#include <SYS_LOG.h>

	Sys_log* Sys_log::_default_instance = 0;

	Sys_log::Sys_log()
	{
		_error_log_pointer = 0;
		_error_log_downcount = 0;
	}
	void Sys_log::push_error(uint8_t error)
	{
		if(!_default_instance)
		{
			_default_instance = new Sys_log();
		}
		_default_instance->error_log[_default_instance->_error_log_pointer] = error;
		_default_instance->_error_log_pointer++;
		if(_default_instance->_error_log_pointer >= SYS_LOG_LENGTH)
			_default_instance->_error_log_pointer = 0;
		if(_default_instance->_error_log_downcount < SYS_LOG_LENGTH)
			_default_instance->_error_log_downcount++;
	}
	uint8_t Sys_log::pop_error()
	{
		if(!_default_instance)
		{
			if(_default_instance->_error_log_downcount == 0)
				return ERR_END_OF_LIST;
			else
			{
				_default_instance->_error_log_downcount--;
				_default_instance->_error_log_pointer--;
				return _default_instance->error_log[_default_instance->_error_log_pointer];
			}
		}
		else
			return ERR_INSTANCE_NOT_FOUND;
	}

