/********************************************************************************
 * File Name:   main.c
 *
 * Description: This is the source code for CM0+ in the the Dual CPU IPC Semaphore
 *              Application for ModusToolbox.
 *
 * Related Document: See README.md
 *
 *
 *********************************************************************************
 * Copyright 2020-2022, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 *********************************************************************************/

#include "cy_pdl.h"
#include "ipc_def.h"
#include <stdlib.h> 
#include "cycfg.h"
#include "cyhal.h"
#include "cybsp.h"


int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init() ;
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }
    /* Enable global interrupts */
    __enable_irq();

    /* Lock the sempahore to wait for CM4 to be init */
    Cy_IPC_Sema_Set(SEMA_NUM, false);

    /* Enable CM4. CY_CORTEX_M4_APPL_ADDR must be updated if CM4 memory layout is changed. */
    Cy_SysEnableCM4(CY_CORTEX_M4_APPL_ADDR);

    /* Wait till CM4 unlocks the semaphore */
    do
    {
        __WFE();
    }
    while (Cy_IPC_Sema_Status(SEMA_NUM) == CY_IPC_SEMA_STATUS_LOCKED);

    /* Update clock settings */
    SystemCoreClockUpdate();

    for (;;)
    {
        /* Check if the button is pressed */
        if (Cy_GPIO_Read(CYBSP_SW2_PORT, CYBSP_SW2_PIN) == 0)
        {
        #if ENABLE_SEMA
            if (Cy_IPC_Sema_Set(SEMA_NUM, false) == CY_IPC_SEMA_SUCCESS)
        #endif
            {
                /* Print a message to the console */
                Cy_SCB_UART_PutString(CYBSP_UART_HW, "Message sent from CM0+\r\n");

            #if ENABLE_SEMA
                while (CY_IPC_SEMA_SUCCESS != Cy_IPC_Sema_Clear(SEMA_NUM, false));
            #endif
            }
        }
    }
}

/* [] END OF FILE */
