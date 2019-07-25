/*
 * State.h
 *
 *  Created on: 1/02/2019
 *      Author: julian
 */

#ifndef STATE_STATE_H_
#define STATE_STATE_H_

#include "../Inc/Common/Globals.h"


void State_Failure(qSM_t *fsm);
qSM_Status_t State_Init(qSM_t *fsm);
qSM_Status_t State_SendPayload(qSM_t *fsm);
qSM_Status_t State_ConfirmationChangeFrq( qSM_t *fsm);

qSM_Status_t State_Blink(qSM_t *fsm);

qSM_Status_t State_Sleep(qSM_t *fsm);

qSM_Status_t State_VerifiedLowBaterry(qSM_t *fsm);
qSM_Status_t State_WaitPushButton(qSM_t *fsm);

void BeforeAny(qSM_t *fsm);


#endif /* STATE_STATE_H_ */
