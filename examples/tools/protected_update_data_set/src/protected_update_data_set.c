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
* \file protected_update_data_set.c
*
* \brief   This file implements APIs, types and data structures used for protected update data set creation.
*
* \ingroup  grProtectedUpdateTool
*
* @{
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>    
#include "pal\pal_logger.h"
#include "protected_update_data_set.h"
#include "pal\pal_crypt.h"
#include "cbor.h"
#include "pal\pal_os_memory.h"
#include "common_utilites.h" 

#define PROTECT_UPDATE_MANIFEST_VERSION             (1U)

// This will be replaced by code
const int8_t signature_string[] = "Signature1";
uint8_t  signature_header[25];

uint8_t  local_manifest_buffer[550];
extern uint8_t * dataset_file_path;

// Prints protected update manifest and fragment to console
static void protected_update_print_output_to_console(const protected_update_data_set_d * p_cbor_manifest)
{
    uint16_t  count = p_cbor_manifest->data_length;
    uint16_t  index;
    uint16_t  fragment_length;
    uint8_t fragment_number;
    int8_t buffer[100] = {0};

    // Print manifest
    sprintf(buffer, "Manifest Data , size : [%03d]\n\t", p_cbor_manifest->data_length );
    pal_logger_print_message(buffer);
    pal_logger_print_variable_name((uint8_t  *)"manifest_data", 0);

    for (index = 0; index < count; index++)
    {
        pal_logger_print_byte(p_cbor_manifest->data[index]);
        sprintf(buffer, "0x%02X, ", p_cbor_manifest->data[index] );

        if (0 == ((index + 1)) % 16)
        {
            pal_logger_print_message("\n\t");
        }
    }
    pal_logger_print_variable_name(NULL,0);
    pal_logger_print_message("");

    // Print fragment
    count = p_cbor_manifest->fragments_length;
    memset(buffer, 0x00, sizeof(buffer));

    for (index = 0; index < count; index++)
    {
        if ((index % MAX_PAYLOAD_SIZE) == 0)
        {
            fragment_length = (p_cbor_manifest->fragments_length - index );
            fragment_length = (fragment_length > MAX_PAYLOAD_SIZE) ? MAX_PAYLOAD_SIZE : fragment_length;

            if (0 != index)    
            { 
                pal_logger_print_variable_name(NULL, 0);
            }
            sprintf(buffer, "\n\nFragment number:[%02d], size:[%03d]\n\t", ((uint8_t)(index / MAX_PAYLOAD_SIZE)+1), fragment_length);
            pal_logger_print_message(buffer);
            pal_logger_print_variable_name((uint8_t  *)"fragment", (uint8_t )(index / MAX_PAYLOAD_SIZE) + 1);
            fragment_number = (uint8_t )(index / MAX_PAYLOAD_SIZE) + 1;  
            sprintf(buffer, "uint8_t fragment_%02d[] = \n\t{\n\t", fragment_number);
        }
        pal_logger_print_byte(p_cbor_manifest->fragments[index]);
        sprintf(buffer, "0x%02X, ", p_cbor_manifest->fragments[index] );

        if (0 == ((index + 1)) % 16)
        {
            pal_logger_print_message("\n\t");
        }
    }
    if (count == index)    
    { 
        pal_logger_print_variable_name(NULL, 0);
    }
}

// Prints protected update manifest and fragment to file
static void protected_update_print_output_to_file(const protected_update_data_set_d * p_cbor_manifest)
{
    uint16_t  count = p_cbor_manifest->data_length;
    uint16_t  index;
    uint16_t  fragment_length = 0;
    uint8_t fragment_number = 0;
    int8_t buffer[100] = {0};  

    // print manifest to file
    pal_logger_print_to_file("\tuint8_t manifest_data[] = \n\t{\n\t\t", 0);

    for (index = 0; index < count; index++)
    {
        sprintf(buffer, "0x%02X, ", p_cbor_manifest->data[index] );
        pal_logger_print_to_file(buffer, 0);

        if (0 == ((index + 1)) % 16)
        {
            pal_logger_print_to_file("\n\t\t", 0);
        }
    }
    pal_logger_print_to_file("\n\t};\n", 0);

    // print fragment to file
    count = p_cbor_manifest->fragments_length;
    memset(buffer, 0x00, sizeof(buffer));
    for (index = 0; index < count; index++)
    {
        if ((index % MAX_PAYLOAD_SIZE) == 0)
        {
            fragment_length = (p_cbor_manifest->fragments_length - index );
            fragment_length = (fragment_length > MAX_PAYLOAD_SIZE) ? MAX_PAYLOAD_SIZE : fragment_length;

            if (0 != index)    
            { 
                pal_logger_print_to_file("};\n", 0);
            }
            pal_logger_print_to_file("\n", 0);
            fragment_number = (uint8_t )(index / MAX_PAYLOAD_SIZE) + 1;  
            sprintf(buffer, "\tuint8_t fragment_%02d[] = \n\t{\n\t\t", fragment_number);
            pal_logger_print_to_file(buffer, 0);
        }
        sprintf(buffer, "0x%02X, ", p_cbor_manifest->fragments[index] );
        pal_logger_print_to_file(buffer, 0);

        if (0 == ((index + 1)) % 16)
        {
            pal_logger_print_to_file("\n\t\t", 0);
        }
    }
    if (count == index)    
    { 
        pal_logger_print_to_file("\n\t};", 0);
    }
}

// Prints protected update manifest and fragment 
void protected_update_print_output_dataset(const protected_update_data_set_d * p_cbor_manifest)
{
    // prints to console by default
    protected_update_print_output_to_console(p_cbor_manifest);

    // if the file path is provided then print to file
    if (NULL != dataset_file_path)
    {
        protected_update_print_output_to_file(p_cbor_manifest);
    }
}

//Decodes ECC signature compoenents
_STATIC_H int32_t protected_update_decode_ecc_signature(   const uint8_t  * in_signature,
                                                    uint8_t  * out_signature,
                                                    uint16_t  sign_len)
{
    uint16_t  offset = 3;
    uint8_t  round = 0;
    uint16_t  out_index = 0;
    uint16_t  length_to_copy = 0;
    uint16_t  r_s_comp_length = sign_len/2;

    // if the sign len is greater than 0x7F and length is represented in 2 bytes
    if (0x7F < sign_len)
    {
        offset++;
    }

    while (round < 2)
    {
        // if compoenents are negative
        if (*(in_signature + offset) == (r_s_comp_length + 1))
        {
            length_to_copy = r_s_comp_length;
            offset+=2;
        }
        // if compoenents are less than key size
        else if (*(in_signature + offset) < r_s_comp_length)
        {
            length_to_copy = *(in_signature + offset);
            offset++;
            out_index += (uint16_t )(r_s_comp_length - length_to_copy);
        }
        else
        {
            offset++;
            length_to_copy = r_s_comp_length;
        }

        memcpy( out_signature + out_index,
                in_signature + offset,
                length_to_copy);

        offset += (uint16_t )(length_to_copy + 1);
        out_index += (uint16_t )length_to_copy;
        round++;
    } ;

    return 0;
}

_STATIC_H int32_t protected_update_form_payload(manifest_t * manifest_data)
{
    int32_t status = 0;

    if(NULL != manifest_data->p_data_payload)
    {
        // Do nothing but just copy the data to payload
        manifest_data->payload = manifest_data->p_data_payload->data;
        manifest_data->payload_length = manifest_data->p_data_payload->data_length;
        manifest_data->payload_type = ePAYLOAD_DATA;
    }
    else
    {
        pal_logger_print_message("Error : Invalid payload type\n");
        status = -1;
    }


    return status;
}

_STATIC_H uint16_t  protected_update_get_fragment_size(void)
{
    return (MAX_PAYLOAD_FRAGMENT_SIZE);
}

//lint --e{534, 818} suppress "return value of cbor functions is not considered"
int32_t protected_update_create_manifest(   manifest_t * manifest_data,
                                        protected_update_data_set_d * p_cbor_manifest)
{
    int32_t status = -1;
    uint8_t  digest[FRAGMENT_DIGEST_LENGTH];

    uint8_t signature[256];
    uint16_t signature_length;

    uint8_t data_to_sign[300];
    uint16_t data_to_sign_length;

    uint8_t extracted_signature[256];
    uint16_t payload_length_for_digest;

    uint16_t sign_algo_length;
    uint8_t * sign_header_ptr = NULL;
    uint16_t sign_header_length = 0;
    uint16_t offset = 0;
    uint16_t length_marker = 0;

    do
    {

        cbor_set_array_of_data(signature_header, 4, &sign_header_length);
        cbor_set_byte_string(signature_header, strlen(signature_string), &sign_header_length);
        memcpy(signature_header+sign_header_length, signature_string, strlen(signature_string));
        sign_header_length = sign_header_length + (uint16_t )strlen(signature_string);

        sign_header_length++;
        sign_algo_length = sign_header_length;
        cbor_set_map_tag(signature_header, 0x01, &sign_header_length);
        cbor_set_map_signed_type(signature_header, 0x01, (int32_t )manifest_data->signature_algo, &sign_header_length);
        sign_algo_length = (sign_header_length - sign_algo_length);
        sign_header_length = (sign_header_length - sign_algo_length) - 1;
        cbor_set_byte_string(signature_header, sign_algo_length, &sign_header_length);
        sign_header_length = sign_header_length + sign_algo_length;
        cbor_set_byte_string(signature_header, 0x00, &sign_header_length);
        sign_header_ptr = (uint8_t  *)signature_header;


        // 1.COSE
        cbor_set_array_of_data(local_manifest_buffer, 4, &offset);

        // 2.protected signed header trust
        cbor_set_byte_string(local_manifest_buffer, (sign_algo_length), &offset);
        cbor_set_map_tag(local_manifest_buffer, 0x01, &offset);
        cbor_set_map_signed_type(local_manifest_buffer, 0x01, (int32_t) manifest_data->signature_algo, &offset);
        // 3.unprotected -signed header Trust
        cbor_set_map_tag(local_manifest_buffer, 0x01, &offset);
        cbor_set_map_byte_string_type(local_manifest_buffer, 0x04, (uint8_t  *)&manifest_data->trust_anchor_oid, 0x02, &offset);


        // 4.Payload
        length_marker = (uint8_t )offset;
        //  Dummy length added to be improved
        cbor_set_byte_string(local_manifest_buffer, 0xa5, &offset);

        // 4.1 Trust_manifest
        cbor_set_array_of_data(local_manifest_buffer, 0x06, &offset);

        // manifestVersion
        cbor_set_unsigned_integer(local_manifest_buffer, PROTECT_UPDATE_MANIFEST_VERSION, &offset);
        cbor_set_null(local_manifest_buffer, &offset);
        cbor_set_null(local_manifest_buffer, &offset);
        cbor_set_array_of_data(local_manifest_buffer, 4, &offset);

        // Trust_resource
        // Trust_PayloadType
        cbor_set_signed_integer(local_manifest_buffer, (int32_t)(manifest_data->payload_type), &offset);

        if (0 != cbor_set_unsigned_integer(local_manifest_buffer, manifest_data->payload_length, &offset))
        {
            pal_logger_print_message(" Error : Assignment in cbor for payload_length\n");
            break;
        }
        // Trust_PayloadVersion
        if (0 != cbor_set_unsigned_integer(local_manifest_buffer, manifest_data->payload_version, &offset))
        {
            pal_logger_print_message(" Error : Assignment in cbor for payload_version\n");
            break;
        }

        cbor_set_array_of_data(local_manifest_buffer, 2, &offset);
        // AdditionalInfo
        // Trust_AddInfo_Data
        if(ePAYLOAD_DATA == manifest_data->payload_type)
        {
            // Trust_AddInfo_Data : offset
            if (0 != cbor_set_unsigned_integer(local_manifest_buffer, manifest_data->p_data_payload->offset_in_oid, &offset))
            {
                pal_logger_print_message(" Error : Assignment in cbor for offset_in_oid\n");
                break;
            }
            // Trust_AddInfo_Data : Trust_AddInfo_WriteType
            cbor_set_unsigned_integer(local_manifest_buffer, (uint8_t )manifest_data->p_data_payload->write_type, &offset);
        }
        else
        {
            pal_logger_print_message(" Error : Payload type mismatch for additional data\n");
            break;
        }
        // Trust_processors
        cbor_set_array_of_data(local_manifest_buffer, 2, &offset);
        cbor_set_array_of_data(local_manifest_buffer, 2, &offset);

        // Trust_processors.ProcessingStep_integrity.process
        cbor_set_signed_integer(local_manifest_buffer, -1, &offset);
        cbor_set_byte_string(local_manifest_buffer, FRAGMENT_DIGEST_LENGTH + 5, &offset);

        //Trust_processors.ProcessingStep_integrity.digestAlgorithm : DigestAlgorithms
        cbor_set_array_of_data(local_manifest_buffer, 2, &offset);
        cbor_set_unsigned_integer(local_manifest_buffer, (uint32_t)manifest_data->digest_algo, &offset);

        //creating digest
        payload_length_for_digest = (p_cbor_manifest->fragments_length > MAX_PAYLOAD_SIZE) ? MAX_PAYLOAD_SIZE : p_cbor_manifest->fragments_length;
        if (0 != pal_crypt_hash(    NULL,
                                    (uint8_t)eSHA_256,
                                    (uint8_t *)p_cbor_manifest->fragments,
                                    (uint16_t)payload_length_for_digest,
                                    (uint8_t * )digest))
        {
            pal_logger_print_message(" Error : Failed in pal_crypt_hash");
            break;
        }

        cbor_set_byte_string(local_manifest_buffer, sizeof(digest), &offset);
        //Trust_processors.ProcessingStep_integrity.digest: IFX_DigestSize  
        memcpy((local_manifest_buffer+offset), digest, sizeof(digest));
        offset += sizeof(digest);

        // Trust_processors.ProcessingStep2 : NULL
        cbor_set_null(local_manifest_buffer, &offset);

        // Component identifier
        cbor_set_array_of_data(local_manifest_buffer, 2, &offset);
        if(NULL != manifest_data->couid)
        {
            cbor_set_byte_string(local_manifest_buffer, strlen((const int8_t*)manifest_data->couid), &offset);
            memcpy((local_manifest_buffer+offset), manifest_data->couid, strlen((const int8_t*)manifest_data->couid));
            offset += (uint16_t )strlen((const int8_t*)manifest_data->couid);
        }
        else
        {
            cbor_set_byte_string(local_manifest_buffer, 0, &offset);
        }
        //target oid
        cbor_set_byte_string(local_manifest_buffer, 2, &offset);
        protected_tool_common_set_uint16(&local_manifest_buffer[offset], &offset, manifest_data->target_oid);

        //Updating length filed of payload
        data_to_sign_length = ((offset - length_marker) - 1);
        local_manifest_buffer[length_marker + 1] = (uint8_t )(data_to_sign_length - 1);

        //Generate signature
        memcpy(data_to_sign, sign_header_ptr, sign_header_length);
        data_to_sign_length += 1;
        memcpy(data_to_sign + sign_header_length, local_manifest_buffer + length_marker, data_to_sign_length);
        data_to_sign_length += sign_header_length;

        signature_length = sizeof(signature);
        if(0 != pal_crypt_sign( NULL,
                                (uint8_t * )data_to_sign,
                                (uint16_t)data_to_sign_length,
                                (uint8_t * )signature,
                                (uint16_t * )&signature_length,
                                (const uint8_t *)manifest_data->private_key,
                                0))
        {
            pal_logger_print_message(" Error : Failed in pal_crypt_calculate_signature");
            break;
        }

        // do only for EC_256
        if ((int16_t)eES_SHA == (int16_t)manifest_data->signature_algo)
        {
            status = pal_crypt_get_signature_length((uint8_t  *)manifest_data->private_key,&signature_length, eES_SHA);
            if (0 != status)
            {
                pal_logger_print_message(" Error : Getting signature length\n");
                break;
            }
            // Remove encoding
            memset(extracted_signature, 0, sizeof(extracted_signature));
            status = protected_update_decode_ecc_signature((const uint8_t  *)signature, extracted_signature, signature_length);
            if (0 != status)
            {
                pal_logger_print_message(" Error : Decode ecc signature");
                break;
            }
            cbor_set_byte_string(local_manifest_buffer, signature_length, &offset);
            memcpy(&local_manifest_buffer[offset], extracted_signature, signature_length);
        }
        else if ((int16_t)eRSA_SSA_PKCS1_V1_5_SHA_256 == (int16_t)manifest_data->signature_algo)
        {
            status = pal_crypt_get_signature_length((uint8_t  *)manifest_data->private_key,&signature_length, eRSA_SSA_PKCS1_V1_5_SHA_256);
            if (0 != status)
            {
                pal_logger_print_message(" Error : Getting signature length\n");
                break;
            }
            cbor_set_byte_string(local_manifest_buffer, signature_length, &offset);
            memcpy(&local_manifest_buffer[offset], signature, signature_length);
        }

        offset += signature_length;
        p_cbor_manifest->data_length = offset;
        p_cbor_manifest->data = (uint8_t  * )pal_os_malloc(offset);
        memcpy(p_cbor_manifest->data, local_manifest_buffer, offset);

        status = 0;
    } while (0);
    return status;

}

int32_t protected_update_create_fragments(manifest_t * manifest_data,
                                      protected_update_data_set_d * p_cbor_manifest)
{
    int32_t status = -1;
    uint8_t digest[FRAGMENT_DIGEST_LENGTH];
    uint16_t remaining_payload_length = 0;
    uint16_t max_memory_required;
    uint8_t * fragments = NULL;
    uint8_t count_of_fragments = 0, index_for_hashing = 0;
    uint8_t * p_current_fragment = NULL;
    uint16_t length_to_digest;
    uint16_t payload_len_to_copy = 0;
    uint16_t max_payload_fragment_size = 0;

    do
    {

        // Form payload
        if(0 != protected_update_form_payload(manifest_data))
        {
            pal_logger_print_message(" Error : Failed in protected_update_form_payload");
            break;
        }
        remaining_payload_length = (uint16_t )manifest_data->payload_length;
        max_payload_fragment_size = protected_update_get_fragment_size();
        max_memory_required = (uint16_t )((remaining_payload_length / max_payload_fragment_size) * MAX_PAYLOAD_SIZE);
        max_memory_required += (uint16_t )(remaining_payload_length % max_payload_fragment_size);

        fragments = (uint8_t  *)pal_os_calloc(max_memory_required, sizeof(uint8_t ));
        if(NULL == fragments)
        {
            pal_logger_print_message("Error : Create fragment memory allocation");
            break;
        }
        // Copy all the user data into fragment payloads
        do
        {
            p_current_fragment = fragments + (count_of_fragments * MAX_PAYLOAD_SIZE);

            //payload_len_to_copy = (remaining_payload_length > (MAX_PAYLOAD_SIZE - ((NULL != manifest_data->p_confidentiality) ? manifest_data->p_confidentiality->enc_params.mac_size : 0))) ? max_payload_fragment_size : remaining_payload_length;
            payload_len_to_copy = (remaining_payload_length > max_payload_fragment_size) ? max_payload_fragment_size : remaining_payload_length;
            memcpy(p_current_fragment, manifest_data->payload + (max_payload_fragment_size * count_of_fragments), payload_len_to_copy);

            count_of_fragments++;

            remaining_payload_length -= payload_len_to_copy;

            p_cbor_manifest->fragments_length += payload_len_to_copy;

        } while (remaining_payload_length != 0);

        // Index to start hashing last fragment
        index_for_hashing = count_of_fragments-1;

        // Start adding hash to fragment
        do
        {
            if (0 != index_for_hashing)
            {
                p_current_fragment = fragments + (index_for_hashing * MAX_PAYLOAD_SIZE);

                length_to_digest = ((count_of_fragments - index_for_hashing) == 1) ? payload_len_to_copy  : MAX_PAYLOAD_SIZE;

                // Calculate hash on the current fragment and add to previous fragment
                pal_crypt_hash( NULL,
                                (uint8_t)eSHA_256,
                                (const uint8_t  *)p_current_fragment,
                                (uint32_t)length_to_digest,
                                digest);
                memcpy( p_current_fragment - FRAGMENT_DIGEST_LENGTH,
                        digest,
                        sizeof(digest));

                p_cbor_manifest->fragments_length += sizeof(digest);

                index_for_hashing--;
            }

        } while (index_for_hashing != 0);

        p_cbor_manifest->fragments = fragments;
        // Below variable can be deleted
        p_cbor_manifest->actual_memory_allocated = max_memory_required;
        status = 0;
    } while (0);
    return status;
}

/**
* @}
*/
