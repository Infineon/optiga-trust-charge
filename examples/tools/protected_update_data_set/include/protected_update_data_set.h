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
* \file protected_update_data_set.h
*
* \brief   This file defines APIs, types and data structures used for protected update data set creation.
*
* \ingroup  grProtectedUpdateTool
*
* @{
*/
#ifndef _PROTECTED_UPDATE_DATA_SET_H_
#define _PROTECTED_UPDATE_DATA_SET_H_

#include <stdint.h>

#define FRAGMENT_DIGEST_LENGTH                      (0x20)
#define MAX_PAYLOAD_SIZE                            (640U)
#define MAX_PAYLOAD_FRAGMENT_SIZE                   (MAX_PAYLOAD_SIZE - FRAGMENT_DIGEST_LENGTH)

#define PAYLOAD_TYPE_DATA                           "data"
#define PROTECTED_INTEGRATY_TYPE                    "integrity"
#define TRUE                                        (1U)
#define FALSE                                       (0U)

typedef enum payload_type
{
    ePAYLOAD_DATA       = -1,
} payload_type_t;

typedef struct data_payload
{
    uint32_t offset_in_oid;
    uint8_t * data;
    uint16_t data_length;
    uint8_t write_type;
}data_payload_t;

typedef enum signature_algo
{
    eES_SHA                     = -7,
    eRSA_SSA_PKCS1_V1_5_SHA_256 = -65700,
}signature_algo_t;

typedef enum digest_algo
{
    eSHA_256                    = 41,
}digest_algo_t;

// Structure to store manifest data
typedef struct manifest
{
    uint16_t payload_version;
    uint16_t trust_anchor_oid;
    uint16_t target_oid;
    const uint8_t * couid;
    signature_algo_t signature_algo;
    const int8_t * private_key;
    digest_algo_t digest_algo;
    uint8_t * payload;
    uint32_t payload_length;
    payload_type_t payload_type;
    
    // payload data
    data_payload_t * p_data_payload;

} manifest_t;

// Structure to store protect update data set
typedef struct protected_update_data_set
{
    uint8_t * data;
    uint16_t data_length;

    uint8_t * fragments;
    uint16_t fragments_length;
    uint16_t actual_memory_allocated;

}protected_update_data_set_d;

// Create protected update manifest
int32_t protected_update_create_manifest(   manifest_t * manifest_data, 
                                        protected_update_data_set_d * p_cbor_manifest);

// Create protected update payload fragments
int32_t protected_update_create_fragments(  manifest_t * manifest_data, 
                                        protected_update_data_set_d * p_cbor_manifest);

void protected_update_print_output_dataset(const protected_update_data_set_d * p_cbor_manifest);

#ifndef _NO_STATIC_H
#define _STATIC_H static
#else
#define _STATIC_H
#endif

#endif //_PROTECTED_UPDATE_DATA_SET_H_

/**
* @}
*/