/*-----------------------------------------------------------
 * FreeRTOS configuration.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE. 
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <p24FJ256GA106.h>

#define MPLAB_PIC24_PORT

#define configUSE_PREEMPTION            1
#define configUSE_IDLE_HOOK             0
#define configUSE_TICK_HOOK             0
#define configTICK_RATE_HZ              ( (portTickType) 1000 )
#define configCPU_CLOCK_HZ              ( (unsigned long) 16000000 )  /* Fosc/2 */
#define configMAX_PRIORITIES            ( (unsigned portBASE_TYPE) 4 )
#define configMINIMAL_STACK_SIZE        ( 115 )
#define configTOTAL_HEAP_SIZE           ( (size_t) (5000) )
#define configMAX_TASK_NAME_LEN         ( 4 )
#define configUSE_TRACE_FACILITY        0
#define configUSE_16_BIT_TICKS          1
#define configIDLE_SHOULD_YIELD         1
#define configUSE_CO_ROUTINES           0

// Set the following definitions to 1 to include the API function, or zero
// to exclude the API function.
#define INCLUDE_vTaskPrioritySet		1
#define INCLUDE_uxTaskPriorityGet		0
#define INCLUDE_vTaskDelete				1
#define INCLUDE_vTaskCleanUpResources	1
#define INCLUDE_vTaskSuspend			1
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay				1
#define configUSE_MUTEXES 				1
#define configKERNEL_INTERRUPT_PRIORITY	0x01

#endif /* FREERTOS_CONFIG_H */
