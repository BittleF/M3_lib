/*
 * MSG_SYST.h
 *
 *  Created on: 4 февр. 2018 г.
 *      Author: Devil
 */

#ifndef SYS_LOG_H_
#define SYS_LOG_H_

#define SYS_LOG_LENGTH 10

#include <stdint.h>

enum Error_messages
{
	ERR_NULL_PTR,
	ERR_SHIT_HAPPENED,
	ERR_INSTANCE_NOT_FOUND,
	ERR_INCREMENT_FAILED,
	ERR_DECREMENT_FAILED,
	ERR_CREATION_FAILED,
	ERR_OUT_OF_RANGE,
	ERR_END_OF_LIST
};

class Sys_log
{
private:
	uint8_t _error_log_pointer;
	uint8_t _error_log_downcount;
	uint8_t error_log [SYS_LOG_LENGTH];
	static Sys_log* _default_instance;

	Sys_log();
public:
	//static System_log* create_new_log();
	static void push_error(uint8_t error);
	static uint8_t pop_error();
	//static uint8_t* pop_error();
};

#endif /* SYS_LOG_H_ */
