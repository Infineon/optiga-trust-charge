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
* \file pal_crypt_mbedtls.c
*
* \brief   This file implements APIs, types and data structures used for protected update pal crypt.
*
* \ingroup  grProtectedUpdateTool
*
* @{
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mbedtls\sha256.h"
#include "mbedtls\rsa.h"
#include "mbedtls\pk.h"
#include "mbedtls\entropy.h"
#include "mbedtls\ctr_drbg.h"
#include "pal\pal_logger.h"
#include "mbedtls\ssl.h"
#include "pal\pal_crypt.h"
#include "protected_update_data_set.h"
#include "pal\pal_logger.h"

static uint16_t pal_crypt_calculate_sha256_hash(    const uint8_t * message,
                                            uint16_t message_len,
                                            uint8_t * digest)
{
    uint16_t status = 1;
    mbedtls_sha256_context sha256_ctx;
    do
    {
        mbedtls_sha256_init(&sha256_ctx);

        if (0 != mbedtls_sha256_starts_ret(&sha256_ctx, 0))
        {
            pal_logger_print_message(" Error : Failed in mbedtls_sha256_starts_ret\n");
            break;
        }
        if (0 != mbedtls_sha256_update_ret(&sha256_ctx, message, message_len))
        {
            pal_logger_print_message(" Error : Failed in mbedtls_sha256_update_ret\n");
            break;
        }

        if (0 != mbedtls_sha256_finish_ret(&sha256_ctx, digest))
        {
            pal_logger_print_message(" Error : Failed in mbedtls_sha256_finish_ret\n");
            break;
        }
        mbedtls_sha256_free(&sha256_ctx);
        status = 0;
    } while (0);
    return status;
}

//lint --e{715} suppress "argument "p_pal_crypt" is not used in the implementation but kept for future use"
pal_status_t pal_crypt_hash( pal_crypt_t* p_pal_crypt,
                    uint8_t hash_algorithm,
                    const uint8_t * p_message,
                    uint32_t message_length,
                    uint8_t * p_digest)
{
    pal_status_t status = 1;

    if((uint8_t)eSHA_256 == hash_algorithm) // SHA-256
    {
        status = pal_crypt_calculate_sha256_hash(p_message, (uint16_t)message_length, p_digest);
    }

    return status;
}

pal_status_t pal_crypt_get_signature_length(uint8_t * p_private_key, uint16_t * sign_len, signature_algo_t sign_algo)
{
    pal_status_t status = 1;
    mbedtls_pk_context ctx;
    mbedtls_ecp_keypair * ecc_ctx;
    mbedtls_rsa_context * rsa_ctx;
    mbedtls_pk_type_t key_type;
    
    mbedtls_pk_init(&ctx);
    do
    {
        if (0 != mbedtls_pk_parse_keyfile(&ctx, (const int8_t*)p_private_key, ""))
        {
            pal_logger_print_message(" Error : Failed in mbedtls_pk_parse_keyfile\n");
            break;
        }

        key_type = mbedtls_pk_get_type(&ctx);
        if(eES_SHA == sign_algo)
        {
            if(MBEDTLS_PK_ECKEY == key_type)
            {
                ecc_ctx = (mbedtls_ecp_keypair *)ctx.pk_ctx;
                // Signature length is (n/8)*2 hence to get the length of R & S component, 8/2 = 4
                // Mod operation is performed to get non divisible key size in case of secp521r1.
                *sign_len = (uint16_t)((ecc_ctx->grp.nbits)/4 + ((ecc_ctx->grp.nbits)%4?2:0)); 
            }
            else
            {
                pal_logger_print_message(" Error : Key type not mismatch\n");
                break;
            }
        }
        else if(eRSA_SSA_PKCS1_V1_5_SHA_256 == sign_algo)
        {
            if(MBEDTLS_PK_RSA == key_type)
            {
                rsa_ctx = (mbedtls_rsa_context *)ctx.pk_ctx;
                // Signature length is (n/8)*2 hence to get the length of R & S component, 8/2 = 4
                // Mod operation is performed to get non divisible key size in case of secp521r1.
                *sign_len = (uint16_t)rsa_ctx->len; 
            }
            else
            {
                pal_logger_print_message(" Error : Key type not mismatch\n");
                break;
            }
        }
        else
        {
            pal_logger_print_message(" Error : Invalid sign algo\n");
            break;
        }
        status = 0;
    }while(0);

    mbedtls_pk_free(&ctx);
    return status;
}

//lint --e{715} suppress "argument "p_pal_crypt", "private_key_length" is not used in the implementation but kept for future use"
pal_status_t pal_crypt_sign( pal_crypt_t* p_pal_crypt,
                    uint8_t * p_digest, 
                    uint16_t digest_length,
                    uint8_t * p_signature,
                    uint16_t * signature_length,
                    const uint8_t * p_private_key,
                    uint16_t private_key_length)
{
    pal_status_t status = 1;
    mbedtls_pk_context ctx;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    uint8_t hash[32];
    const int8_t *pers = (const int8_t *)"mbedtls_pk_sign";
    size_t signature_buff_len;

    do
    {
            mbedtls_entropy_init(&entropy);
            mbedtls_ctr_drbg_init(&ctr_drbg);
            mbedtls_pk_init(&ctx);

            if(0 != mbedtls_ctr_drbg_seed(  &ctr_drbg, mbedtls_entropy_func, 
                                            &entropy, 
                                            (const uint8_t *)pers, 
                                            strlen(pers)))
            {
                pal_logger_print_message(" Error : Failed in mbedtls_ctr_drbg_seed\n");
                break;
            }
            //Read key into an array
            if (0 != mbedtls_pk_parse_keyfile(&ctx, (const int8_t*)p_private_key, ""))
            {
                pal_logger_print_message(" Error : Failed in mbedtls_pk_parse_keyfile\n");
                break;
            }

            // generate hash
            if (0 != pal_crypt_hash(NULL, (uint8_t)eSHA_256, p_digest, digest_length, hash))
            {
                pal_logger_print_message(" Error : Failed in pal_crypt_hash\n");
                break;
            }

            if (0 != mbedtls_pk_sign(&ctx, MBEDTLS_MD_SHA256, hash, 0, p_signature, &signature_buff_len,
                mbedtls_ctr_drbg_random, &ctr_drbg))
            {
                pal_logger_print_message(" Error : Failed in mbedtls_pk_sign\n");
                break;
            }
            *signature_length = (uint16_t)signature_buff_len;
            mbedtls_pk_free(&ctx);
            mbedtls_ctr_drbg_free(&ctr_drbg);
            mbedtls_entropy_free(&entropy);
            status = 0;
    } while (0);
    return status;
}

/** 
* @}
*/

