# OPTIGA Trust Charge Crypt API

<a name="crypt_api"></a>
<a name="top"></a>

[tocstart]: # (toc start)

  * optiga_crypt.h
    * [optiga_crypt_create](#optiga_crypt_create)
    * [optiga_crypt_destroy](#optiga_crypt_destroy)
    * [optiga_crypt_random](#optiga_crypt_random)
	* [optiga_crypt_hash](#optiga_crypt_hash)
    * [optiga_crypt_hash_start](#optiga_crypt_hash_start)
    * [optiga_crypt_hash_update](#optiga_crypt_hash_update)
    * [optiga_crypt_hash_finalize](#optiga_crypt_hash_finalize)
    * [optiga_crypt_ecc_generate_keypair](#optiga_crypt_ecc_generate_keypair)
    * [optiga_crypt_ecdsa_sign](#optiga_crypt_ecdsa_sign)
    * [optiga_crypt_ecdsa_verify](#optiga_crypt_ecdsa_verify)
    * [OPTIGA_CRYPT_SET_COMMS_PROTOCOL_VERSION](#OPTIGA_CRYPT_SET_COMMS_PROTOCOL_VERSION)
    * [OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL](#OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL)

[tocend]: # (toc end)

# <a name="optiga_crypt_create"></a>optiga_crypt_create

* **Prototype**
	```c
    optiga_crypt_t* optiga_crypt_create(uint8_t optiga_instance_id,
                                        callback_handler_t handler,
                                        void * caller_context);
	```
* **Description:**  Create an instance of `optiga_crypt_t`.
	* Stores the callers context and callback handler.
	* Allocate memory for `optiga_crypt_t`.
* **Pre Conditions:**  None
* **Arguments:**
	* `optiga_instance_id`:  Indicates the OPTIGA instance to be associated with `optiga_crypt_t`. Value should be defined as below
	  * `OPTIGA_INSTANCE_ID_0`: Indicate created instance will be part of OPTIGA with slave address 0x30.
	* `handler`: Valid pointer to callback function
	* `caller_context`: Pointer to upper layer context, contains user context data
* **Returns:** 
	* `optiga_crypt_t` On success function will return pointer of `optiga_crypt_t`
	* `NULL` Either of one:
	  * Input arguments are NULL.
	  * OPTIGA_CMD_MAX_REGISTRATIONS number of instances are already created.
	  * Low layer function fails.
* **Example:**
  ```c
        void example_optiga_crypt_create(void)
        {
             optiga_crypt_t * me = NULL;
             
             me = optiga_crypt_create(0, optiga_crypt_callback, NULL);

             if (NULL == me)
             {
                 printf("optiga_crypt_create() failed.");
                 break;
             }
        }
  ```
* **Note:** This API is implemented in synchronous mode.

[Top](#top)

# <a name="optiga_crypt_destroy"></a>optiga_crypt_destroy

* **Prototype**
	```c
    optiga_lib_status_t optiga_crypt_destroy(optiga_crypt_t * me);
	```
* **Description:**  Destroys the `optiga_crypt_t` instance. De-allocate the memory of the `optiga_crypt_t` instance.
* **Pre Conditions:**  An instance of optiga_crypt using [`optiga_crypt_create`](#optiga_crypt_create) must be available.
* **Arguments:**
	* `me`:  Valid instance of `optiga_crypt_t`.
* **Returns:** 
    * `OPTIGA_CRYPT_SUCCESS` Successful invocation.
	* `OPTIGA_CRYPT_ERROR_INVALID_INPUT`  Wrong Input arguments provided.
	* `OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE` The previous operation with the same instance is not complete.
* **Example:** N/A
* **Note:** 
    * User must take care to nullify the instance pointer.
    * Invoke this API only after all the asynchronous process is completed otherwise the behavior of software stack is not defined.

[Top](#top)

# <a name="optiga_crypt_random"></a>optiga_crypt_random

* **Prototype**
	```c
	optiga_lib_status_t optiga_crypt_random(optiga_crypt_t * me,
                                            optiga_rng_type_t rng_type,
                                            uint8_t * random_data,
                                            uint16_t random_data_length);
	```
* **Description:**  Generates the requested random stream of data for the user provided length. Invokes `optiga_cmd_get_random` API, based on the input arguments to retrieve random data. 
* **Pre Conditions:**  The application on OPTIGA must be opened using `optiga_util_open_application` before using this API.
* **Arguments:**
    * `me`: Valid instance of `optiga_crypt_t` created using [optiga_crypt_create](#optiga_crypt_create).
	* `rng_type`:  Type of random number generator
	  * The input must be from `optiga_rng_type`.
	    * `OPTIGA_RNG_TYPE_TRNG`: Using true random number generator
	    * `OPTIGA_RNG_TYPE_DRNG`: Using true deterministic number generator
	  * Argument check for rng_type is not done since OPTIGA will provide an error for invalid rng_type.
	* `random_data`: Pointer to the buffer into which random data is stored, must not be NULL.
	* `random_data_length`: Length of random data to be generated.
	    *  Range should be 8 - 256 bytes.
	    *  Length validation is not done, since OPTIGA will provide an error for invalid random_data_length.
* **Returns:** 
    * `OPTIGA_CRYPT_SUCCESS` Successful invocation.
	* `OPTIGA_CRYPT_ERROR_INVALID_INPUT`  Wrong Input arguments provided.
	* `OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE` The previous operation with the same instance is not complete.
	* `OPTIGA_DEVICE_ERROR` Command execution failure in OPTIGA and the LSB indicates the error code.
	    * Refer to the [Solution Reference Manual](../raw/master/documents/OPTIGA_Trust_Qi_V1_Solution_Reference_Manual_v1.00.pdf)
* **Example:**  See [example_optiga_crypt_random.c](../blob/master/examples/optiga/example_optiga_crypt_random.c)
* **Note:**
    * For **protected I2C communication**, Refer to [OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL](#OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL)
    * Error codes from lower layers will be returned as it is.
    * The maximum value of the `random_data_length` parameter is size of buffer `random_data`. In case the value is greater than buffer size, memory corruption can occur.

[Top](#top)

# <a name="optiga_crypt_hash_start"></a>optiga_crypt_hash_start

* **Prototype**
```c
    optiga_lib_status_t optiga_crypt_hash(optiga_crypt_t * me,
                                          optiga_hash_type_t hash_algorithm,
                                          uint8_t source_of_data_to_hash,
                                          const void * data_to_hash, 
                                          uint8_t * hash_output);;
```
* **Description:**   Updates a hashing for input data and returns digest.
* **Pre Conditions:**  The application on OPTIGA must be opened using [`optiga_util_open_application`](#optiga_util_open_application) before using this API.
* **Arguments:**
    * `me`: Valid instance of `optiga_crypt_t` created using [optiga_crypt_create](#optiga_crypt_create).
	* `hash_algorithm`: Hash algorithm of `optiga_hash_type_t`.
	* `source_of_data_to_hash`: Data from host / Data in OPTIGA. Must be one of the below
        * `OPTIGA_CRYPT_HOST_DATA` or Non-Zero value ,if source of data is from Host.
		* `OPTIGA_CRYPT_OID_DATA`, if the source of data is from OPTIGA.
	* `data_to_hash`: Data for hashing either in `hash_data_from_host_t` or in `hash_data_in_optiga_t`
	* ´hash_output` Pointer to the valid buffer to store hash output
	* `hash_ctx`: Pointer to `optiga_hash_context_t` to store the hash context from OPTIGA. The input **hash_algo** in  **hash_ctx** must be from `optiga_hash_type`.
* **Returns:** 
	* `OPTIGA_CRYPT_SUCCESS` Successful invocation.
	* `OPTIGA_CRYPT_ERROR_INVALID_INPUT` Wrong Input arguments provided.
	* `OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE` The previous operation with the same instance is not complete.
	* `OPTIGA_DEVICE_ERROR` Command execution failure in OPTIGA and the LSB indicates the error code. Refer to the the [Solution Reference Manual](../raw/master/documents/OPTIGA_Trust_Qi_V1_Solution_Reference_Manual_v1.00.pdf)
* **Example:**  Refer to the [example_optiga_crypt_hash.c](../blob/master/examples/optiga/example_optiga_crypt_hash.c)
* **Notes:**
    * For **protected I2C communication**, Refer to [OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL](#OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL)
    * Error codes from lower layers will be returned as it is.
    * User must save the output hash context for further usage as OPTIGA does not store it internally.

[Top](#top)

# <a name="optiga_crypt_hash_start"></a>optiga_crypt_hash_start

* **Prototype**
```c
    optiga_lib_status_t optiga_crypt_hash_start(optiga_crypt_t * me,
                                            optiga_hash_context_t * hash_ctx);
```
* **Description:**   Updates hashing for the given data and hash context then export the updated hash context. Update the input hash context. Exports the hash context to caller.
* **Pre Conditions:**  The application on OPTIGA must be opened using [`optiga_util_open_application`](#optiga_util_open_application) before using this API. `optiga_hash_context_t` from [`optiga_crypt_hash_start`](#optiga_crypt_hash_start) or [`optiga_crypt_hash_update`](#optiga_crypt_hash_update) must be available.
* **Arguments:**
    * `me`: Valid instance of `optiga_crypt_t` created using [optiga_crypt_create](#optiga_crypt_create).
	* `hash_ctx`: Pointer to `optiga_hash_context_t` to store the hash context from OPTIGA. The input **hash_algo** in  **hash_ctx** must be from `optiga_hash_type`.
* **Returns:** 
	* `OPTIGA_CRYPT_SUCCESS` Successful invocation.
	* `OPTIGA_CRYPT_ERROR_INVALID_INPUT` Wrong Input arguments provided.
	* `OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE` The previous operation with the same instance is not complete.
	* `OPTIGA_DEVICE_ERROR` Command execution failure in OPTIGA and the LSB indicates the error code. Refer to the the [Solution Reference Manual](../raw/master/documents/OPTIGA_Trust_Qi_V1_Solution_Reference_Manual_v1.00.pdf)
* **Example:**  Refer to the [example_optiga_crypt_hash.c](../blob/master/examples/optiga/example_optiga_crypt_hash.c)
* **Notes:**
    * For **protected I2C communication**, Refer to [OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL](#OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL)
    * Error codes from lower layers will be returned as it is.
    * User must save the output hash context for further usage as OPTIGA does not store it internally.

[Top](#top)

# <a name="optiga_crypt_hash_update"></a>optiga_crypt_hash_update

* **Prototype**
```c
    optiga_lib_status_t optiga_crypt_hash_update(optiga_crypt_t * me,
                                                 optiga_hash_context_t * hash_ctx,
                                                 uint8_t source_of_data_to_hash,
                                                 const void * data_to_hash);
```
* **Description:**    Updates hashing for the given data and hash context then export the updated hash context. Update the input hash context. Exports the hash context to caller.
* **Pre Conditions:**  The application on OPTIGA must be opened using [`optiga_util_open_application`](#optiga_util_open_application) before using this API. `optiga_hash_context_t` from [`optiga_crypt_hash_start`](#optiga_crypt_hash_start) or [`optiga_crypt_hash_update`](#optiga_crypt_hash_update) must be available. 
* **Arguments:**
    * `me`: Valid instance of `optiga_crypt_t` created using [optiga_crypt_create](#optiga_crypt_create).
	* `hash_ctx`:  Pointer to #optiga_hash_context_t that contains hash context and algorithm information.
	* `source_of_data_to_hash`: Data from host / Data in optiga. Must be one of the below
        * `OPTIGA_CRYPT_HOST_DATA` or Non-Zero value ,if source of data is from Host.
        * `OPTIGA_CRYPT_OID_DATA`, if the source of data is from OPITGA.
    * `data_to_hash`: Data for hashing either in `hash_data_from_host` or in `hash_data_in_optiga`
* **Returns:** 
	* `OPTIGA_CRYPT_SUCCESS` Successful invocation.
	* `OPTIGA_CRYPT_ERROR_INVALID_INPUT` Wrong Input arguments provided.
	* `OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE` The previous operation with the same instance is not complete.
	* `OPTIGA_DEVICE_ERROR` Command execution failure in OPTIGA and the LSB indicates the error code. Refer to the the [Solution Reference Manual](../raw/master/documents/OPTIGA_Trust_Qi_V1_Solution_Reference_Manual_v1.00.pdf)
* **Example:**  Refer to the [example_optiga_crypt_hash.c](../blob/master/examples/optiga/example_optiga_crypt_hash.c)
* **Note:**
    * For **protected I2C communication**, Refer to [OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL](#OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL)
    * Error codes from lower layers will be returned as it is.
    * User must save the output hash context for further usage as OPTIGA does not store it internally.

[Top](#top)

# <a name="optiga_crypt_hash_finalize"></a>optiga_crypt_hash_finalize

* **Prototype**
```c
    optiga_lib_status_t optiga_crypt_hash_finalize(optiga_crypt_t * me,
                                                   optiga_hash_context_t * hash_ctx,
                                                   uint8_t * hash_output);
```
* **Description:**  Finalizes the hash context and returns hash as output. Finalize the hash from the input hash context. Exports the finalized hash.
  * **Pre Conditions:**  The application on OPTIGA must be opened using [`optiga_util_open_application`](#optiga_util_open_application) before using this API. `optiga_hash_context_t` from [`optiga_crypt_hash_start`](#optiga_crypt_hash_start) or [`optiga_crypt_hash_update`](#optiga_crypt_hash_update) must be available. 
* **Arguments:**
    * `me`: Valid instance of `optiga_crypt_t` created using [optiga_crypt_create](#optiga_crypt_create).
	* `hash_ctx`:  Pointer to `optiga_hash_context_t` containing hash context from OPTIGA, must not be NULL.
	* `hash_output`:   Pointer to the buffer which holds the finalized hash.
* **Returns:** 
	* `OPTIGA_CRYPT_SUCCESS` Successful invocation.
	* `OPTIGA_CRYPT_ERROR_INVALID_INPUT` Wrong Input arguments provided.
	* `OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE` The previous operation with the same instance is not complete.
	* `OPTIGA_DEVICE_ERROR` Command execution failure in OPTIGA and the LSB indicates the error code. Refer to the the [Solution Reference Manual](../raw/master/documents/OPTIGA_Trust_Qi_V1_Solution_Reference_Manual_v1.00.pdf)
* **Example:**  Refer to the [example_optiga_crypt_hash.c](../blob/master/examples/optiga/example_optiga_crypt_hash.c)
* **Note:**
    * For **protected I2C communication**, Refer to [OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL](#OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL)
    * Error codes from lower layers will be returned as it is.
    * Hash context is not updated by this API. This can be used later to fulfill intermediate hash use-cases.
    * User must save the output hash context for further usage as OPTIGA does not store it internally.

[Top](#top)

# <a name="optiga_crypt_ecc_generate_keypair"></a>optiga_crypt_ecc_generate_keypair

* **Prototype**
```c
    optiga_lib_status_t optiga_crypt_ecc_generate_keypair(optiga_crypt_t * me,
                                                          optiga_ecc_curve_t curve_id,
                                                          uint8_t key_usage,
                                                          bool_t export_private_key,
                                                          void * private_key,
                                                          uint8_t * public_key,
                                                          uint16_t * public_key_length);
```
* **Description:**     Generates an ECC key-pair based on the type of the key. Generate an ECC key pair using OPTIGA. If export is requested, Exports the private key else stores it in the input private key OID. Always exports the public key.
* **Pre Conditions:**  The application on OPTIGA must be opened using [`optiga_util_open_application`](#optiga_util_open_application) before using this API.
* **Arguments:**
    * `me`: Valid instance of `optiga_crypt_t` created using [optiga_crypt_create](#optiga_crypt_create).
	* `curve_id`:              ECC curve id.
		* `OPTIGA_ECC_CURVE_NIST_P_256`
		* `OPTIGA_ECC_CURVE_NIST_P_384`
	* `key_usage`:             Key usage defined by `optiga_key_usage_t`. Values from #optiga_key_usage can be logically ORed and passed. It is ignored if export_private_key is TRUE (1).
		* `OPTIGA_KEY_USAGE_AUTHENTICATION`:
		* `OPTIGA_KEY_USAGE_SIGN`
		* `OPTIGA_KEY_USAGE_KEY_AGREEMENT`
		* `OPTIGA_KEY_USAGE_ENCRYPTION`
	* `export_private_key`:   
		* `TRUE (1)` Exports both private key and public key to the host. The input key_usage is ignored.
		* `FALSE (0)` Exports only public key to the host. 
	* `private_key`:           Buffer to store private key or private key OID of OPTIGA, must not be NULL.  If export_private_key is TRUE, assign pointer to a buffer to store private key. The size of the buffer must be sufficient enough to accommodate the key type and additional DER encoding formats. Private Key can either be a non-volatile Device Private Key OR a Session Context for volatile Device Private Key, in which case the generated Key has to be stored in the respective Session Context and can later be addressed, i.e. Private Key OIDs: 0xE0F0 (usually ocupied by a preinstalled private key), 0xE0F1, 0xE0F2, 0xE0F3; Session Context OIDs: 0xE100, 0xE101, 0xE102, 0xE103  
	* `public_key`:            Buffer to store public key, must not be NULL.
	* `public_key_length`:     Pointer ot the public key length, initially set as size of `public_key` buffer, later updated as actual length of `public_key`.
* **Returns:** 
	* `OPTIGA_CRYPT_SUCCESS` Successful invocation.
	* `OPTIGA_CRYPT_ERROR_INVALID_INPUT` Wrong Input arguments provided.
	* `OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE` The previous operation with the same instance is not complete.
	* `OPTIGA_DEVICE_ERROR` Command execution failure in OPTIGA and the LSB indicates the error code. Refer to the the [Solution Reference Manual](../raw/master/documents/OPTIGA_Trust_Qi_V1_Solution_Reference_Manual_v1.00.pdf)
* **Example:**  Refer to the [example_optiga_crypt_ecc_generate_keypair.c](../blob/master/examples/optiga/example_optiga_crypt_ecc_generate_keypair.c)
* **Note:**
    * For **protected I2C communication**, Refer to [OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL](#OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL)
    * Error codes from lower layers will be returned as it is.
	* For Data Formats refer to the corresponding Wiki [section](Data-format-examples)

[Top](#top)
	
# <a name="optiga_crypt_ecdsa_sign"></a>optiga_crypt_ecdsa_sign

* **Prototype**
```c
    optiga_lib_status_t optiga_crypt_ecdsa_sign(optiga_crypt_t * me,
                                                const uint8_t * digest,
                                                uint8_t digest_length,
                                                optiga_key_id_t private_key,
                                                uint8_t * signature,
                                                uint16_t * signature_length);
```
* **Description:**     Generates a signature for the given digest using private key stored in OPTIGA. Generates signature for the input digest. Exports the generated signature. The private key oid can be either from the session (volatile) or key store (non volatile). If the private key is not available in the respective OID, this would lead to an error.
* **Pre Conditions:**  The application on OPTIGA must be opened using [`optiga_util_open_application`](#optiga_util_open_application). before using this API.
* **Arguments:**
    * `me`:                  Valid instance of `optiga_crypt_t` created using [optiga_crypt_create](#optiga_crypt_create).
	* `digest`:              Digest on which signature is generated.
	* `digest_length`:       Length of the input digest.
	* `private_key`:         Private key OID to generate signature.
	* `signature`:           Pointer to store generated signature, must not be NULL. The size of the buffer must be sufficient enough to accommodate the additional. DER encoding formatting for R and S components of signature.
	* `signature_length`:    Length of signature. Initial value set as length of buffer, later updated as the actual length of generated signature.
* **Returns:** 
	* `OPTIGA_CRYPT_SUCCESS` Successful invocation.
	* `OPTIGA_CRYPT_ERROR_INVALID_INPUT` Wrong Input arguments provided.
	* `OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE` The previous operation with the same instance is not complete.
	* `OPTIGA_DEVICE_ERROR` Command execution failure in OPTIGA and the LSB indicates the error code. Refer to the the [Solution Reference Manual](../raw/master/documents/OPTIGA_Trust_Qi_V1_Solution_Reference_Manual_v1.00.pdf)
* **Example:** Refer to the [example_optiga_crypt_ecdsa_sign.c](../blob/master/examples/optiga/example_optiga_crypt_ecdsa_sign.c)
* **Note:**
    * For **protected I2C communication**, Refer to [OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL](#OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL)
    * Error codes from lower layers will be returned as it is.
    * Tries to access session OID without acquiring session in [`optiga_crypt_ecc_generate_keypair`](#optiga_crypt_ecc_generate_keypair) throws `OPTIGA_CMD_ERROR_INVALID_INPUT` error.
    * For Data Formats refer to the corresponding Wiki [section](Data-format-examples).
	* The signature has the following form:
        ```
        0x000000: 02 20 38 0f 56 c8 90 53 18 9d 8f 58 b4 46 35 a0 . 8.V..S...X.F5.
        0x000010: d7 07 63 ef 9f a2 30 64 93 e4 3d bf 7b db 57 a1 ..c...0d..=.{.W.
        0x000020: b6 d7 02 20 4f 5e 3a db 6b 1a eb ac 66 9a 15 69 ... O^:.k...f..i
        0x000030: 0d 7d 46 5b 44 72 40 06 a5 7b 06 84 0f d7 6e 0f .}F[Dr@..{....n.
        0x000040: 4b 45 7f 50                                     KE.P 
        ```

        Which has two DER encoded integers:
        **r:**
        ```
        0x000000: 02 20 38 0f 56 c8 90 53 18 9d 8f 58 b4 46 35 a0 . 8.V..S...X.F5.
        0x000010: d7 07 63 ef 9f a2 30 64 93 e4 3d bf 7b db 57 a1 ..c...0d..=.{.W.
        0x000020: b6 d7
        ```
        **s:**
         ```
        0x000020:       02 20 4f 5e 3a db 6b 1a eb ac 66 9a 15 69 ... O^:.k...f..i
        0x000030: 0d 7d 46 5b 44 72 40 06 a5 7b 06 84 0f d7 6e 0f .}F[Dr@..{....n.
        0x000040: 4b 45 7f 50   
        ```
        
        Different crypto libraries work with signatures differently, in general it's required to append a couple of bytes to this signature: SEQUENCE tag which is 0x30 and a length of the sequence, which is in **this case** is 0x44

[Top](#top)

# <a name="optiga_crypt_ecdsa_verify"></a>optiga_crypt_ecdsa_verify

* **Prototype**
```c
    optiga_lib_status_t optiga_crypt_ecdsa_verify(optiga_crypt_t * me,
                                                  const uint8_t * digest,
                                                  uint8_t digest_length,
                                                  const uint8_t * signature,
                                                  uint16_t signature_length,
                                                  uint8_t public_key_source_type,
                                                  const void * public_key);
```
* **Description:**     Verifies the signature over a given digest provided with the input data. Verifies the signature over the given provided with the input data using public key. It invokes the callback handler of the instance, when it is asynchronously completed.
* **Pre Conditions:**  The application on OPTIGA must be opened using [`optiga_util_open_application`](#optiga_util_open_application).
* **Arguments:**
    * `me`:                  Valid instance of `optiga_crypt_t` created using [optiga_crypt_create](#optiga_crypt_create).
	* `digest`:              Pointer to a given digest buffer, must not be NULL.
	* `digest_length`:       Length of digest.
	* `signature`:           Pointer to a given signature buffer, must not be NULL.
	* `signature_length`:    Length of signature
	* `public_key_source_type`:    Public key from host / public key of certificate OID from OPTIGA. Value must be one of the below
		* `OPTIGA_CRYPT_OID_DATA`: if the public key is to used from the certificate data object from OPTIGA.
		* `OPTIGA_CRYPT_HOST_DATA`: if the public key is provided by host.
	* `public_key`:    Public key from host / OID of certificate object. Value must be one of the below
		* For certificate OID, pointer OID value must be passed (pointer to the uint16 type with the OID of the certificate).
		* For Public key from host, pointer to `public_key_from_host_t` instance.
* **Returns:** 
	* `OPTIGA_CRYPT_SUCCESS` Successful invocation.
	* `OPTIGA_CRYPT_ERROR_INVALID_INPUT` Wrong Input arguments provided.
	* `OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE` The previous operation with the same instance is not complete.
	* `OPTIGA_DEVICE_ERROR` Command execution failure in OPTIGA and the LSB indicates the error code. Refer to the the [Solution Reference Manual](../raw/master/documents/OPTIGA_Trust_Qi_V1_Solution_Reference_Manual_v1.00.pdf)
* **Example:** Refer to the [example_optiga_crypt_ecdsa_verify.c](../blob/master/examples/optiga/example_optiga_crypt_ecdsa_verify.c)
* **Note:**
    * For **protected I2C communication**, Refer to [OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL](#OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL)
    * Error codes from lower layers will be returned as it is.
	* For Data Formats refer to the corresponding Wiki [section](Data-format-examples)

[Top](#top)

# <a name="OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL"></a>OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL

* **Prototype**
```c
    #define OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL(p_instance, protection_level) \
    { \
        optiga_crypt_set_comms_params(p_instance, \
                                             OPTIGA_COMMS_PROTECTION_LEVEL, \
                                             protection_level);\
    }
```
* **Description:**     Enables the protected I2C communication with OPTIGA.
    * Sets the protection mode for the supplied instance.
    * Call this function before calling the service layer API which requires protection.
* **Pre Conditions:**  
    * `OPTIGA_COMMS_SHIELDED_CONNECTION` macro must be defined.
    * `OPTIGA_CRYPT_SET_COMMS_PROTOCOL_VERSION` function must be called prior to set the required protocol version
        * Currently only Pre-shared Secret based version is supported.
* **Arguments:**
    * `p_instance`:   Valid pointer to an instance
	* `protection_level`:      Required protection mode
	    * `OPTIGA_COMMS_NO_PROTECTION` : Command and response is unprotected
	    * `OPTIGA_COMMS_COMMAND_PROTECTION` : Command is protected and response is unprotected
	    * `OPTIGA_COMMS_RESPONSE_PROTECTION` : Command is unprotected and response is protected
	    * `OPTIGA_COMMS_FULL_PROTECTION` : Both command and response is protected
	    * To re-establish secure channel, bitwise-OR protection_level with `OPTIGA_COMMS_RE_ESTABLISH` 

* **Returns:** None
* **Example:** Refer to the [example_optiga_util_read_data.c](../blob/master/examples/optiga/example_optiga_util_read_data.c)
* **Note:** None

[Top](#top)

# <a name="OPTIGA_CRYPT_SET_COMMS_PROTOCOL_VERSION"></a>OPTIGA_CRYPT_SET_COMMS_PROTOCOL_VERSION

* **Prototype**
```c
    #define OPTIGA_CRYPT_SET_COMMS_PROTOCOL_VERSION(p_instance, version) \
    { \
        optiga_crypt_set_comms_params(p_instance, \
                                             OPTIGA_COMMS_PROTOCOL_VERSION, \
                                             version);\
    }
```
* **Description:**     Select the protocol version required for the I2C protected communication.
    * Select the protocol version for the supplied instance.
* **Pre Conditions:**  
    * `OPTIGA_COMMS_SHIELDED_CONNECTION` macro must be defined.
* **Arguments:**
    * `p_instance`:   Valid pointer to an instance
	* `version`:      Required protocol version
	    * `OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET` : Pre-shared Secret based protocol version
* **Returns:** None
* **Example:** Refer to the [example_optiga_util_read_data.c](../blob/master/examples/optiga/example_optiga_util_read_data.c)
* **Note:** None

[Top](#top)
