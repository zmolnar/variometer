/* thrBeepControl.h --- 
 * 
 * @file thrBeepControl.h
 * @brief Beep controller thread.
 * @author Zoltán, Molnár
 */

#ifndef BEEPCONTROLTHREAD_H
#define BEEPCONTROLTHREAD_H

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
        START_BEEPER    = (1 << 1),
        STOP_BEEPER     = (1 << 2),
        STEP_VOLUME     = (1 << 3),
        SYSTEM_SHUTDOWN = (1 << 4)
} BeeperEvent_t;

/*******************************************************************************/
/* DECLARATIONS OF GLOBAL VARIABLES                                           */
/*******************************************************************************/
extern event_source_t beeper_event_source;

/*******************************************************************************/
/* DECLARATION OF GLOBAL FUNCTIONS                                             */
/*******************************************************************************/
THD_FUNCTION(BeepControlThread, arg);

#endif

/******************************* END OF FILE ***********************************/

