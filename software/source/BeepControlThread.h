/* thrBeepControl.h --- 
 * 
 * @file thrBeepControl.h
 * @brief Beep controller thread.
 * @author Zoltán Molnár
 * @date szo dec 19 14:11:42 2015 (+0100)
 * Version: 
 * Last-Updated: Sun Aug 28 16:01:13 2016 (+0200)
 *           By: Molnár Zoltán
 * 
 */

#ifndef __THRBEEPCONTROL_H
#define __THRBEEPCONTROL_H

/*******************************************************************************/
/* INCLUDES                                                                    */
/*******************************************************************************/
#include "ch.h"

/*******************************************************************************/
/* DEFINED CONSTANTS                                                           */
/*******************************************************************************/


/*******************************************************************************/
/* MACRO DEFINITIONS                                                           */
/*******************************************************************************/


/*******************************************************************************/
/* TYPE DEFINITIONS                                                            */
/*******************************************************************************/
typedef enum {
        UPDATE_BEEPER,
        STEP_BEEP_VOLUME,
        SYSTEM_SHUTDOWN
} BeeperEvent_t;

/*******************************************************************************/
/* DECLARATIONS OF GLOBAL VARIABLES                                           */
/*******************************************************************************/
extern event_source_t beeper_event_source;

/*******************************************************************************/
/* DECLARATION OF GLOBAL FUNCTIONS                                             */
/*******************************************************************************/
THD_FUNCTION(BeepControlThread, arg);

#endif /* THRBEEPCONTROL_H */

/******************************* END OF FILE ***********************************/

