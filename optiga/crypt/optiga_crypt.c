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
* \file optiga_crypt.c
*
* \brief   This file implements the OPTIGA Crypt module.
*
* \ingroup  grOptigaCrypt
*
* @{
*/


#include "optiga/optiga_crypt.h"
#include "optiga/common/optiga_lib_logger.h"
#include "optiga/common/optiga_lib_common_internal.h"
#include "optiga/pal/pal_os_memory.h"

/// ECDSA FIPS 186-3 without hash
#define OPTIGA_CRYPT_ECDSA_FIPS_186_3_WITHOUT_HASH                  (0x11)


#if defined (OPTIGA_LIB_ENABLE_LOGGING) && defined (OPTIGA_LIB_ENABLE_CRYPT_LOGGING)

// Logs the message provided from Crypt layer
#define OPTIGA_CRYPT_LOG_MESSAGE(msg) \
{\
    optiga_lib_print_message(msg,OPTIGA_CRYPT_SERVICE,OPTIGA_CRYPT_SERVICE_COLOR);\
}

// Logs the byte array buffer provided from Crypt layer in hexadecimal format
//lint --e{750} suppress "The unused OPTIGA_CRYPT_LOG_HEX_DATA macro is kept for future enhancements"
#define OPTIGA_CRYPT_LOG_HEX_DATA(array,array_len) \
{\
    optiga_lib_print_array_hex_format(array,array_len,OPTIGA_UNPROTECTED_DATA_COLOR);\
}

//Logs the status info provided from Crypt layer
//lint --e{750} suppress "The unused OPTIGA_CRYPT_LOG_STATUS macro is kept for future enhancements"
#define OPTIGA_CRYPT_LOG_STATUS(return_value) \
{ \
    if (OPTIGA_LIB_SUCCESS != return_value) \
    { \
        optiga_lib_print_status(OPTIGA_CRYPT_SERVICE,OPTIGA_ERROR_COLOR,return_value); \
    } \
    else\
    { \
        optiga_lib_print_status(OPTIGA_CRYPT_SERVICE,OPTIGA_CRYPT_SERVICE_COLOR,return_value); \
    } \
}
#else

#define OPTIGA_CRYPT_LOG_MESSAGE(msg) {}
//lint --e{750} suppress "The unused OPTIGA_CRYPT_LOG_HEX_DATA macro is kept for future enhancements"
#define OPTIGA_CRYPT_LOG_HEX_DATA(array, array_len) {}
//lint --e{750} suppress "The unused OPTIGA_CRYPT_LOG_STATUS macro is kept for future enhancements"
#define OPTIGA_CRYPT_LOG_STATUS(return_value) {}

#endif

_STATIC_H void optiga_crypt_generic_event_handler(void * p_ctx,
                                                  optiga_lib_status_t event)
{
    optiga_crypt_t * me = (optiga_crypt_t *)p_ctx;

    me->instance_state = OPTIGA_LIB_INSTANCE_FREE;
    me->handler(me->caller_context, event);
}

_STATIC_H void optiga_crypt_reset_protection_level(optiga_crypt_t * me)
{
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
    if (NULL != me)
#endif
    {
        OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL(me, OPTIGA_COMMS_DEFAULT_PROTECTION_LEVEL);
    }
}

extern void optiga_cmd_set_shielded_connection_option(optiga_cmd_t * me,
                                                      uint8_t value,
                                                      uint8_t shielded_connection_option);

#ifdef OPTIGA_CRYPT_ECC_GENERATE_KEYPAIR_ENABLED
_STATIC_H optiga_lib_status_t optiga_crypt_generate_keypair(optiga_crypt_t * me,
                                                            uint8_t cmd_param,
                                                            uint8_t key_usage,
                                                            bool_t export_private_key,
                                                            void * p_private_key,
                                                            uint8_t * p_public_key,
                                                            uint16_t * p_public_key_length)
{
    optiga_lib_status_t return_value = OPTIGA_CRYPT_ERROR;
    optiga_gen_keypair_params_t * p_params;

    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == me) || (NULL == me->my_cmd) ||
            (NULL == p_public_key) || (NULL == p_private_key) || (NULL == p_public_key_length))
        {
            return_value = OPTIGA_CRYPT_ERROR_INVALID_INPUT;
            break;
        }
#endif
        if (OPTIGA_LIB_INSTANCE_BUSY == me->instance_state)
        {
            return_value = OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE;
            break;
        }

        me->instance_state = OPTIGA_LIB_INSTANCE_BUSY;
        p_params = (optiga_gen_keypair_params_t *)&(me->params.optiga_gen_keypair_params);
        pal_os_memset(&me->params,0x00,sizeof(optiga_crypt_params_t));

        p_params->key_usage = key_usage;
        p_params->export_private_key = export_private_key;
        p_params->public_key_length = p_public_key_length;
        OPTIGA_PROTECTION_ENABLE(me->my_cmd, me);
        OPTIGA_PROTECTION_SET_VERSION(me->my_cmd, me);

        if (export_private_key)
        {
            p_params->private_key = (uint8_t *)p_private_key;
        }
        else
        {
            p_params->private_key_oid = (optiga_key_id_t)(* ((uint16_t* )p_private_key));
        }

        p_params->public_key = p_public_key;
        p_params->public_key_length = p_public_key_length;

        return_value = optiga_cmd_gen_keypair(me->my_cmd,
                                              (uint8_t )cmd_param,
                                              (optiga_gen_keypair_params_t *)p_params);
    } while (FALSE);
    optiga_crypt_reset_protection_level(me);

    return (return_value);
}
#endif // (OPTIGA_CRYPT_ECC_GENERATE_KEYPAIR_ENABLED)

#ifdef OPTIGA_CRYPT_ECDSA_SIGN_ENABLED
//lint --e{715} suppress "The salt_length argument is kept for future use"
_STATIC_H optiga_lib_status_t optiga_crypt_sign(optiga_crypt_t * me,
                                                uint8_t signature_scheme,
                                                const uint8_t * p_digest,
                                                uint8_t digest_length,
                                                optiga_key_id_t private_key,
                                                uint8_t * p_signature,
                                                uint16_t * p_signature_length,
                                                uint16_t salt_length)
{
    optiga_lib_status_t return_value = OPTIGA_CRYPT_ERROR;
    optiga_calc_sign_params_t * p_params;

    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == me) || (NULL == me->my_cmd) || (NULL == p_digest) ||
            (NULL == p_signature) || (NULL == p_signature_length))
        {
            return_value = OPTIGA_CRYPT_ERROR_INVALID_INPUT;
            break;
        }
#endif

        if (OPTIGA_LIB_INSTANCE_BUSY == me->instance_state)
        {
            return_value = OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE;
            break;
        }

        me->instance_state = OPTIGA_LIB_INSTANCE_BUSY;

        p_params = (optiga_calc_sign_params_t *)&(me->params.optiga_calc_sign_params);
        pal_os_memset(&me->params,0x00,sizeof(optiga_crypt_params_t));

        p_params->p_digest = p_digest;
        p_params->digest_length = digest_length;
        p_params->private_key_oid = private_key;
        p_params->p_signature = p_signature;
        p_params->p_signature_length = p_signature_length;
        OPTIGA_PROTECTION_ENABLE(me->my_cmd, me);
        OPTIGA_PROTECTION_SET_VERSION(me->my_cmd, me);

        return_value = optiga_cmd_calc_sign(me->my_cmd,
                                            signature_scheme,
                                            (optiga_calc_sign_params_t *)p_params);
        if (OPTIGA_LIB_SUCCESS != return_value)
        {
            me->instance_state = OPTIGA_LIB_INSTANCE_FREE;
        }
    } while (FALSE);
    optiga_crypt_reset_protection_level(me);

    return (return_value);
}
#endif // (OPTIGA_CRYPT_ECDSA_SIGN_ENABLED)

#ifdef OPTIGA_CRYPT_ECDSA_VERIFY_ENABLED
//lint --e{715} suppress "The salt_length argument is kept for future use"
_STATIC_H optiga_lib_status_t optiga_crypt_verify(optiga_crypt_t * me,
                                                  uint8_t cmd_param,
                                                  const uint8_t * p_digest,
                                                  uint8_t digest_length,
                                                  const uint8_t * p_signature,
                                                  uint16_t signature_length,
                                                  uint8_t public_key_source_type,
                                                  const void * p_public_key,
                                                  uint16_t salt_length)
{
    optiga_verify_sign_params_t * p_params;
    optiga_lib_status_t return_value = OPTIGA_CRYPT_ERROR;

    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == me) || (NULL == me->my_cmd) ||
            (NULL == p_digest) || (NULL == p_signature) || (NULL == p_public_key))
        {
            return_value = OPTIGA_CRYPT_ERROR_INVALID_INPUT;
            break;
        }
#endif

        if (OPTIGA_LIB_INSTANCE_BUSY == me->instance_state)
        {
            return_value = OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE;
            break;
        }

        me->instance_state = OPTIGA_LIB_INSTANCE_BUSY;
        p_params = (optiga_verify_sign_params_t *)&(me->params.optiga_verify_sign_params);
        pal_os_memset(&me->params,0x00,sizeof(optiga_crypt_params_t));

        OPTIGA_PROTECTION_ENABLE(me->my_cmd, me);
        OPTIGA_PROTECTION_SET_VERSION(me->my_cmd, me);
        p_params->p_digest = p_digest;
        p_params->digest_length = digest_length;
        p_params->p_signature = p_signature;
        p_params->signature_length = signature_length;
        p_params->public_key_source_type = public_key_source_type;

        if (OPTIGA_CRYPT_OID_DATA == public_key_source_type)
        {
            p_params->certificate_oid = *((uint16_t *)p_public_key);
        }
        else
        {
            p_params->public_key = (public_key_from_host_t *)p_public_key;
        }

        return_value = optiga_cmd_verify_sign(me->my_cmd,
                                             (uint8_t)cmd_param,
                                             (optiga_verify_sign_params_t *)p_params);
        if (OPTIGA_LIB_SUCCESS != return_value)
        {
            me->instance_state = OPTIGA_LIB_INSTANCE_FREE;
        }
    } while (FALSE);
    optiga_crypt_reset_protection_level(me);

    return (return_value);
}
#endif //(OPTIGA_CRYPT_ECDSA_VERIFY_ENABLED)

#ifdef OPTIGA_CRYPT_RANDOM_ENABLED
_STATIC_H optiga_lib_status_t optiga_crypt_get_random(optiga_crypt_t * me,
                                                      uint8_t cmd_param,
                                                      uint8_t * p_random_data,
                                                      uint16_t random_data_length,
                                                      const uint8_t * p_optional_data,
                                                      uint16_t optional_data_length)
{
    optiga_lib_status_t return_value = OPTIGA_CRYPT_ERROR;
    optiga_get_random_params_t * p_params;
    do
    {
        if (OPTIGA_LIB_INSTANCE_BUSY == me->instance_state)
        {
            return_value = OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE;
            break;
        }

        me->instance_state = OPTIGA_LIB_INSTANCE_BUSY;
        p_params = (optiga_get_random_params_t *)&(me->params.optiga_get_random_params);
        pal_os_memset(&me->params,0x00,sizeof(optiga_crypt_params_t));

        p_params->optional_data = p_optional_data;
        p_params->optional_data_length = optional_data_length;
        p_params->random_data = p_random_data;
        p_params->random_data_length = random_data_length;
        OPTIGA_PROTECTION_ENABLE(me->my_cmd, me);
        OPTIGA_PROTECTION_SET_VERSION(me->my_cmd, me);

        return_value = optiga_cmd_get_random(me->my_cmd,
                                             cmd_param,
                                             (optiga_get_random_params_t *)p_params);
        if (OPTIGA_LIB_SUCCESS != return_value)
        {
            me->instance_state = OPTIGA_LIB_INSTANCE_FREE;
        }
    } while (FALSE);
    optiga_crypt_reset_protection_level(me);

    return (return_value);
}
#endif //(OPTIGA_CRYPT_RANDOM_ENABLED)

#ifdef OPTIGA_CRYPT_HASH_ENABLED
_STATIC_H optiga_lib_status_t optiga_crypt_hash_generic(optiga_crypt_t * me,
                                                        uint8_t hash_algorithm,
                                                        uint8_t hash_sequence,
                                                        uint8_t source_of_data_to_hash,
                                                        optiga_hash_context_t * hash_ctx,
                                                        uint32_t context_length,                                                        
                                                        const void * data_to_hash,
                                                        bool_t export_intermediate_ctx,
                                                        uint8_t * hash_output)
{
    optiga_lib_status_t return_value = OPTIGA_CRYPT_ERROR;
    optiga_calc_hash_params_t * p_params;
    do
    {
        if (OPTIGA_LIB_INSTANCE_BUSY == me->instance_state)
        {
            return_value = OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE;
            break;
        }

        me->instance_state = OPTIGA_LIB_INSTANCE_BUSY;
        p_params = (optiga_calc_hash_params_t *)&(me->params.optiga_calc_hash_params);
        pal_os_memset(&me->params,0x00,sizeof(optiga_crypt_params_t));

        OPTIGA_PROTECTION_ENABLE(me->my_cmd, me);
        OPTIGA_PROTECTION_SET_VERSION(me->my_cmd, me);
        p_params->hash_sequence = hash_sequence;
        p_params->p_hash_context = hash_ctx;
        p_params->apparent_context_size = context_length;
        p_params->p_hash_data = NULL;
        p_params->p_hash_oid = NULL;
        p_params->p_out_digest = hash_output;
        p_params->export_hash_ctx = export_intermediate_ctx;
        
        if ((OPTIGA_CRYPT_HASH_CONTINUE == hash_sequence) || (OPTIGA_CRYPT_HASH_START_FINAL == hash_sequence))
        {
            if (source_of_data_to_hash)
            {
                p_params->p_hash_data = (hash_data_from_host_t *)data_to_hash;
            }
            else
            {
                p_params->hash_sequence |= OPTIGA_CRYPT_HASH_FOR_OID;
                p_params->p_hash_oid = (hash_data_in_optiga_t *)data_to_hash;
            }
        }

        return_value = optiga_cmd_calc_hash(me->my_cmd,
                                            hash_algorithm,
                                            (optiga_calc_hash_params_t *)p_params);
        if (OPTIGA_LIB_SUCCESS != return_value)
        {
            me->instance_state = OPTIGA_LIB_INSTANCE_FREE;
        }
    } while (FALSE);
    optiga_crypt_reset_protection_level(me);

    return (return_value);
}
#endif //(OPTIGA_CRYPT_HASH_ENABLED)

#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
void optiga_crypt_set_comms_params(optiga_crypt_t * me,
                                   uint8_t parameter_type,
                                   uint8_t value)
{
    switch (parameter_type)
    {
        case OPTIGA_COMMS_PROTECTION_LEVEL:
        {
            me->protection_level = value;
            break;
        }
        case OPTIGA_COMMS_PROTOCOL_VERSION:
        {
            me->protocol_version = value;
            break;
        }
        default:
        {
            break;
        }
    }
}
#endif

optiga_crypt_t * optiga_crypt_create(uint8_t optiga_instance_id,
                                     callback_handler_t handler,
                                     void * caller_context)
{
    optiga_crypt_t * me = NULL;

    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if (NULL == handler)
        {
            break;
        }
#endif
        me = (optiga_crypt_t *)pal_os_calloc(1, sizeof(optiga_crypt_t));
        if (NULL == me)
        {
            break;
        }

        me->handler = handler;
        me->caller_context = caller_context;
        me->instance_state = OPTIGA_LIB_SUCCESS;
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
        me->protocol_version = OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET;
        me->protection_level = OPTIGA_COMMS_DEFAULT_PROTECTION_LEVEL;
#endif
        me->my_cmd = optiga_cmd_create(optiga_instance_id,
                                       optiga_crypt_generic_event_handler,
                                       me);
        if (NULL == me->my_cmd)
        {
            pal_os_free(me);
            me = NULL;
        }

    } while (FALSE);

    return (me);
}

optiga_lib_status_t optiga_crypt_destroy(optiga_crypt_t * me)
{
    optiga_lib_status_t return_value;

    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if (NULL == me)
        {
            return_value = OPTIGA_CRYPT_ERROR_INVALID_INPUT;
            break;
        }
#endif
        if (OPTIGA_LIB_INSTANCE_BUSY == me->instance_state)
        {
            return_value = OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE;
            break;
        }
        return_value = optiga_cmd_destroy(me->my_cmd);
        pal_os_free(me);

    } while (FALSE);
    return (return_value);
}

#ifdef OPTIGA_CRYPT_RANDOM_ENABLED
optiga_lib_status_t optiga_crypt_random(optiga_crypt_t * me,
                                        optiga_rng_type_t rng_type,
                                        uint8_t * random_data,
                                        uint16_t random_data_length)
{
    optiga_lib_status_t return_value = OPTIGA_CRYPT_ERROR;
    OPTIGA_CRYPT_LOG_MESSAGE(__FUNCTION__);
    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == me) || (NULL == me->my_cmd) || (NULL == random_data))
        {
            return_value = OPTIGA_CRYPT_ERROR_INVALID_INPUT;
            break;
        }
#endif
        return_value = optiga_crypt_get_random(me,
                                               (uint8_t)rng_type,
                                               random_data,
                                               random_data_length,
                                               NULL,
                                               0x00);
    } while (FALSE);

    return (return_value);
}
#endif //OPTIGA_CRYPT_RANDOM_ENABLED

#ifdef OPTIGA_CRYPT_HASH_ENABLED
optiga_lib_status_t optiga_crypt_hash_start(optiga_crypt_t * me,
                                            optiga_hash_context_t * hash_ctx)
{
    optiga_lib_status_t return_value = OPTIGA_CRYPT_ERROR;
    OPTIGA_CRYPT_LOG_MESSAGE(__FUNCTION__);
    do  
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == me) || (NULL == me->my_cmd) || (NULL == hash_ctx))
        {
            return_value = OPTIGA_CRYPT_ERROR_INVALID_INPUT;
            break;
        }
#endif
        return_value = optiga_crypt_hash_generic(me,
                                                 hash_ctx->hash_algo,
                                                 OPTIGA_CRYPT_HASH_START,
                                                 0,
                                                 hash_ctx,
                                                 0,
                                                 NULL,
                                                 TRUE,
                                                 NULL);
    } while (FALSE);

    return (return_value);
}

optiga_lib_status_t optiga_crypt_hash_update(optiga_crypt_t * me,
                                             optiga_hash_context_t * hash_ctx,
                                             uint8_t source_of_data_to_hash,
                                             const void * data_to_hash)
{
    optiga_lib_status_t return_value = OPTIGA_CRYPT_ERROR;
    OPTIGA_CRYPT_LOG_MESSAGE(__FUNCTION__);
    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == me) || (NULL == me->my_cmd) ||
            (NULL == hash_ctx) || (NULL == data_to_hash))
        {
            return_value = OPTIGA_CRYPT_ERROR_INVALID_INPUT;
            break;
        }
#endif
        return_value = optiga_crypt_hash_generic(me,
                                                 hash_ctx->hash_algo,
                                                 OPTIGA_CRYPT_HASH_CONTINUE,
                                                 source_of_data_to_hash,
                                                 hash_ctx,
                                                 hash_ctx->context_buffer_length,
                                                 data_to_hash,
                                                 TRUE,
                                                 NULL);
    } while (FALSE);

    return (return_value);
}

optiga_lib_status_t optiga_crypt_hash_finalize(optiga_crypt_t * me,
                                               optiga_hash_context_t * hash_ctx,
                                               uint8_t * hash_output)
{
    optiga_lib_status_t return_value = OPTIGA_CRYPT_ERROR;
    OPTIGA_CRYPT_LOG_MESSAGE(__FUNCTION__);
    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == me) || (NULL == me->my_cmd) ||
            (NULL == hash_ctx) || (NULL == hash_output))
        {
            return_value = OPTIGA_CRYPT_ERROR_INVALID_INPUT;
            break;
        }
#endif
        return_value = optiga_crypt_hash_generic(me,
                                                 hash_ctx->hash_algo,
                                                 OPTIGA_CRYPT_HASH_FINAL,
                                                 0,
                                                 hash_ctx,
                                                 hash_ctx->context_buffer_length,
                                                 NULL,
                                                 FALSE,
                                                 hash_output);
    } while (FALSE);

    return (return_value);
}

optiga_lib_status_t optiga_crypt_hash(optiga_crypt_t * me,
                                      optiga_hash_type_t hash_algorithm,
                                      uint8_t source_of_data_to_hash,
                                      const void * data_to_hash, 
                                      uint8_t * hash_output)
{
    optiga_lib_status_t return_value = OPTIGA_CRYPT_ERROR;
    OPTIGA_CRYPT_LOG_MESSAGE(__FUNCTION__);
    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == me) || (NULL == me->my_cmd) || (NULL == data_to_hash) || (NULL == hash_output))
        {
            return_value = OPTIGA_CRYPT_ERROR_INVALID_INPUT;
            break;
        }
#endif
        return_value = optiga_crypt_hash_generic(me,
                                                 (uint8_t)hash_algorithm,
                                                 OPTIGA_CRYPT_HASH_START_FINAL,
                                                 source_of_data_to_hash,
                                                 NULL,
                                                 0,
                                                 data_to_hash,
                                                 FALSE,
                                                 hash_output);
    } while (FALSE);

    return (return_value);
}
#endif //OPTIGA_CRYPT_HASH_ENABLED

#ifdef OPTIGA_CRYPT_ECC_GENERATE_KEYPAIR_ENABLED
optiga_lib_status_t optiga_crypt_ecc_generate_keypair(optiga_crypt_t * me,
                                                      optiga_ecc_curve_t curve_id,
                                                      uint8_t key_usage,
                                                      bool_t export_private_key,
                                                      void * private_key,
                                                      uint8_t * public_key,
                                                      uint16_t * public_key_length)
{
    OPTIGA_CRYPT_LOG_MESSAGE(__FUNCTION__);
    return (optiga_crypt_generate_keypair(me,
                                         (uint8_t)curve_id,
                                         key_usage,
                                         export_private_key,
                                         private_key,
                                         public_key,
                                         public_key_length));
}
#endif //OPTIGA_CRYPT_ECC_GENERATE_KEYPAIR_ENABLED

#ifdef OPTIGA_CRYPT_ECDSA_SIGN_ENABLED
optiga_lib_status_t optiga_crypt_ecdsa_sign(optiga_crypt_t * me,
                                            const uint8_t * digest,
                                            uint8_t digest_length,
                                            optiga_key_id_t private_key,
                                            uint8_t * signature,
                                            uint16_t * signature_length)
{
    OPTIGA_CRYPT_LOG_MESSAGE(__FUNCTION__);
    return (optiga_crypt_sign(me,
                              OPTIGA_CRYPT_ECDSA_FIPS_186_3_WITHOUT_HASH,
                              digest,
                              digest_length,
                              private_key,
                              signature,
                              signature_length,
                              0x0000));
}
#endif //OPTIGA_CRYPT_ECDSA_SIGN_ENABLED

#ifdef OPTIGA_CRYPT_ECDSA_VERIFY_ENABLED
optiga_lib_status_t optiga_crypt_ecdsa_verify(optiga_crypt_t * me,
                                              const uint8_t * digest,
                                              uint8_t digest_length,
                                              const uint8_t * signature,
                                              uint16_t signature_length,
                                              uint8_t public_key_source_type,
                                              const void * public_key)
{
    OPTIGA_CRYPT_LOG_MESSAGE(__FUNCTION__);
    return (optiga_crypt_verify(me,
                                OPTIGA_CRYPT_ECDSA_FIPS_186_3_WITHOUT_HASH,
                                digest,
                                digest_length,
                                signature,
                                signature_length,
                                public_key_source_type,
                                public_key,
                                0x0000));
}
#endif //OPTIGA_CRYPT_ECDSA_VERIFY_ENABLED

/**
* @}
*/
