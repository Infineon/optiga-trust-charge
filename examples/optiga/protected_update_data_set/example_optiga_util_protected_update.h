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
* \file example_optiga_util_protected_update.h
*
* \brief   This file provides the data set for protected update of ecc key object with confidentiality.
*
* \ingroup grOptigaExamples
*
* @{
*/

#include <stdint.h>

#include "data_object.h"

/** @brief Macro to enable secure data update in OID example */
#define INTEGRITY_PROTECTED                  (0x00)

/**
 * \brief Specifies the structure for protected update manifest and fragment configuration
 */
typedef struct optiga_protected_update_manifest_fragment_configuration
{
    /// Manifest version.
    uint8_t manifest_version;
    /// Pointer to a buffer where manifest data is stored.
    const uint8_t * manifest_data;
    /// Manifest length
    uint16_t manifest_length;
    /// Pointer to a buffer where continue fragment data is stored.
    const uint8_t * continue_fragment_data;
    /// Continue fragment length
    uint16_t continue_fragment_length;
    /// Pointer to a buffer where final fragment data is stored.
    const uint8_t * final_fragment_data;
    /// Final fragment length
    uint16_t final_fragment_length;
}optiga_protected_update_manifest_fragment_configuration_t;

#ifdef INTEGRITY_PROTECTED
/**
 * Integrity protected manifest and fragment configuration
 */
optiga_protected_update_manifest_fragment_configuration_t data_integrity_configuration =
                                                                {
                                                                     0x01,
                                                                     manifest_int,
                                                                     sizeof(manifest_int),
                                                                     int_continue_fragment_array,
                                                                     sizeof(int_continue_fragment_array),
                                                                     int_final_fragment_array,
                                                                     sizeof(int_final_fragment_array)
                                                                };
#endif


/**
 * \brief Specifies the structure for protected update data configuration
 */
typedef struct optiga_protected_update_data_configuration
{
    /// Target OID
    uint16_t target_oid;
    /// Target OID metadata
    const uint8_t * target_oid_metadata;
    /// Target OID metadata length
    uint16_t target_oid_metadata_length;
    /// Pointer to a buffer where continue fragment data is stored.
    const optiga_protected_update_manifest_fragment_configuration_t * data_config;
}optiga_protected_update_data_configuration_t;



/**
* @}
*/
