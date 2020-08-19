/**
* \copyright
* MIT License
*
* Copyright (c) 2020 Infineon Technologies AG
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
* \file example_utilities.c
*
* \brief   This file provides the utilities for example code.
*
* \ingroup grOptigaExamples
*
* @{
*/

#include "optiga/pal/pal_os_memory.h"

#define OPTIGA_EXAMPLE_UTIL_DER_BITSTRING_TAG               (0x03)
#define OPTIGA_EXAMPLE_UTIL_DER_NUM_UNUSED_BITS             (0x00)

void example_util_encode_ecc_public_key_in_bit_string_format(const uint8_t * q_buffer,
                                                        uint8_t q_length,
                                                        uint8_t * pub_key_buffer,
                                                        uint16_t * pub_key_length)
{
#define OPTIGA_EXAMPLE_UTIL_ECC_DER_ADDITIONAL_LENGTH (0x02)

    uint16_t index = 0;

    pub_key_buffer[index++] = OPTIGA_EXAMPLE_UTIL_DER_BITSTRING_TAG;
    pub_key_buffer[index++] = q_length + OPTIGA_EXAMPLE_UTIL_ECC_DER_ADDITIONAL_LENGTH;
    pub_key_buffer[index++] = OPTIGA_EXAMPLE_UTIL_DER_NUM_UNUSED_BITS;
    // Compression format. Supports only 04 [uncompressed]
    pub_key_buffer[index++] = 0x04;

    pal_os_memcpy(&pub_key_buffer[index], q_buffer, q_length);
    index += q_length;

    *pub_key_length = index;

#undef OPTIGA_EXAMPLE_UTIL_ECC_DER_ADDITIONAL_LENGTH
}

/**
* @}
*/
