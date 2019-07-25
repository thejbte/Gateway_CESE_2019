/*
 * Task.h
 *
 *  Created on: 1/02/2019
 *      Author: julian
 */

#ifndef TASK_H_
#define TASK_H_


	#include "Kernel/QuarkTS.h"
	#include "Common/Globals.h"

	void IdleTask_Callback(qEvent_t e);
	void WisolService_Callback(qEvent_t e);
	void Leds_Callback(qEvent_t e);
	void UplinkDispatcher_Callback(qEvent_t e);

#endif /* TASK_H_ */
