/**
* \copyright
* MIT License
*
* Copyright (c) 2019 Infineon Technologies AG
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE
*
* \endcopyright
*
* \author Infineon Technologies AG
 *
 * \file    pal_logger.c
 *
 * \brief   This file provides the prototypes declarations for pal logger.
 *
 * \ingroup grPAL
 *
 * @{
 */
#include <DAVE.h>
#include "optiga/pal/pal_logger.h"

/// @cond hidden

//lint --e{552,714} suppress "Accessed by user of this structure"
pal_logger_t logger_console =
{
        .logger_config_ptr = NULL,
        .logger_rx_flag = 1,
        .logger_tx_flag = 1
};

/// @endcond


pal_status_t pal_logger_init(void * p_logger_context)
{
    return PAL_STATUS_SUCCESS;
}


pal_status_t pal_logger_deinit(void * p_logger_context)
{
    return PAL_STATUS_SUCCESS;
}


pal_status_t pal_logger_write(void * p_logger_context, const uint8_t * p_log_data, uint32_t log_data_length)
{
	for (int i=0; i<log_data_length; i++) {
		if (p_log_data[i] == '\n') {
		      /* Send '\r'(0x0D) before each '\n'(0x0A). */
		      /* Save a character into the transmit buffer of the UART0 device */
			uint8_t c = 0x0DU;
			UART_Transmit(&UART_0, &c, 1);
			//Wait till the data transmission is ongoing.
			while(UART_0.runtime->tx_busy) {}
			c = 0x0AU;
			UART_Transmit(&UART_0, &c, 1);
			//Wait till the data transmission is ongoing.
			while(UART_0.runtime->tx_busy) {}
		}
		else {
			UART_Transmit(&UART_0, (uint8_t *)&p_log_data[i], 1);
			//Wait till the data transmission is ongoing.
			while(UART_0.runtime->tx_busy) {}
		}
	}
	return PAL_STATUS_SUCCESS;
}

pal_status_t pal_logger_read(void * p_logger_context, uint8_t * p_log_data, uint32_t log_data_length)
{
	return PAL_STATUS_SUCCESS;
}
/**
 * @}
 */
