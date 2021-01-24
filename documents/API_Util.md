# OPTIGA Trust Charge Util API
<a name="crypt_api"></a>
<a name="top"></a>

[tocstart]: # (toc start)

  * optiga_util.h
    * [optiga_util_create](#optiga_util_create)
    * [optiga_util_destroy](#optiga_util_destroy)
    * [optiga_util_open_application](#optiga_util_open_application)
    * [optiga_util_close_application](#optiga_util_close_application)
    * [optiga_util_read_data](#optiga_util_read_data)
    * [optiga_util_read_metadata](#optiga_util_read_metadata)
    * [optiga_util_write_data](#optiga_util_write_data)
    * [optiga_util_write_metadata](#optiga_util_write_metadata)
    * [optiga_util_protected_update_start](#optiga_util_protected_update_start)
    * [optiga_util_protected_update_continue](#optiga_util_protected_update_continue)
    * [optiga_util_protected_update_final](#optiga_util_protected_update_final)
    * [optiga_util_update_count](#optiga_util_update_count)
    * [OPTIGA_UTIL_SET_COMMS_PROTECTION_LEVEL](#OPTIGA_UTIL_SET_COMMS_PROTECTION_LEVEL)
    * [OPTIGA_UTIL_SET_COMMS_PROTOCOL_VERSION](#OPTIGA_UTIL_SET_COMMS_PROTOCOL_VERSION)

[tocend]: # (toc end)

# <a name="optiga_util_create"></a>optiga_util_create

* **Prototype**
	```c
    optiga_util_t * optiga_util_create(uint8_t optiga_instance_id,
                                       callback_handler_t handler,
                                       void * caller_context);
	```
* **Description:**  Create an instance of `optiga_util_t`.
	* Stores the callers context and callback handler.
	* Allocate memory for `optiga_util_t`.
* **Pre Conditions:**  None
* **Arguments:**
	* `optiga_instance_id`:  Indicates the OPTIGA instance to be associated with `optiga_util_t`. Value should be defined as below
	  * `OPTIGA_INSTANCE_ID_0`: Indicate created instance will be part of OPTIGA with slave address 0x30.
	* `handler`: Valid pointer to callback function
	* `caller_context`: Pointer to upper layer context, contains user context data
* **Returns:** 
	* `optiga_util_t` On success function will return pointer of `optiga_util_t`
	* `NULL` Either of one:
	  * Input arguments are NULL.
	  * OPTIGA_CMD_MAX_REGISTRATIONS number of instances are already created.
	  * Low layer function fails.
* **Example:**
  ```c
        void example_optiga_util_create(void)
        {
             optiga_util_t * me = NULL;
             
             me = optiga_util_create(0, optiga_util_callback, NULL);

             if (NULL == me)
             {
                 printf("optiga_util_create() failed.");
                 break;
             }
        }
  ```
* **Note:** This API is implemented in synchronous mode.

[Top](#top)

# <a name="optiga_util_destroy"></a>optiga_util_destroy

* **Prototype**
	```c
    optiga_lib_status_t optiga_util_destroy(optiga_util_t * me);
	```
* **Description:**  De-Initializes the `optiga_util_t` instance.
    * De-allocate the memory of the `optiga_util_t` instance.
* **Pre Conditions:**  An instance of optiga_crypt using [`optiga_crypt_create`](#optiga_crypt_create) must be available.
* **Arguments:**
	* `me`:  Valid instance of `optiga_util_t`.
* **Returns:** 
    * `OPTIGA_CRYPT_SUCCESS` Successful invocation.
	* `OPTIGA_CRYPT_ERROR_INVALID_INPUT`  Wrong Input arguments provided.
	* `OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE` The previous operation with the same instance is not complete.
* **Example:** N/A
* **Note:** 
    * User must take care to nullify the instance pointer.
    * Invoke this API only after all the asynchronous process is completed otherwise the behavior of software stack is not defined.

[Top](#top)

# <a name="optiga_util_open_application"></a>optiga_util_open_application

* **Prototype**
	```c
    optiga_lib_status_t optiga_util_open_application(optiga_util_t * me,
                                                     bool_t perform_restore);
	```
* **Description:**  Initializes the communication with OPTIGA for the given instance.
    * Sets up optiga comms channel.
    * Initiate open application command to optiga.
    * Perform manage context , session restore operations and application restore.
* **Pre Conditions:**  For restoring OPTIGA application, the application should have been hibernated using [`optiga_util_close_application`](#optiga_util_close_application).
* **Arguments:**
    * `me`: Valid instance of `optiga_util_t` created using [optiga_util_create](#optiga_util_create).
	* `perform_restore`:  Restore application on OPTIGA from a previous hibernate state. The values must be as defined below
	  * `TRUE (1)` - Performs application restore. 
	  * `FALSE (0)` - Initialize a clean application context.
* **Returns:** 
    * `OPTIGA_CRYPT_SUCCESS` Successful invocation.
	* `OPTIGA_CRYPT_ERROR_INVALID_INPUT`  Wrong Input arguments provided.
	* `OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE` The previous operation with the same instance is not complete.
	* `OPTIGA_DEVICE_ERROR` Command execution failure in OPTIGA and the LSB indicates the error code.
	    * Refer to the [Solution Reference Manual](../raw/master/documents/OPTIGA_Trust_Charge_V1_Solution_Reference_Manual_v1.00.pdf)
* **Example:**  N/A
* **Note:**
    * For **protected I2C communication**, Refer to [OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL](#OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL)
    * Error codes from lower layers will be returned as it is.
    * If error in lower layer occurs while restoring OPTIGA application, then initialize a clean OPTIGA application context.

[Top](#top)
  
# <a name="optiga_util_close_application"></a>optiga_util_close_application

* **Prototype**
```c
    optiga_lib_status_t optiga_util_close_application(optiga_util_t * me,
                                                      bool_t perform_hibernate);
```
* **Description:**   Closes the communication with OPTIGA for the given instance.
    * Initiate close application command to optiga.
    * Perform manage context secure session save operations.
    * De-Initializes the OPTIGA and closes the communication channel.
    * Power downs the OPTIGA.
* **Pre Conditions:**  The application on OPTIGA must be opened using #optiga_util_open_application before using this API.
* **Arguments:**
    * `me`: Valid instance of `optiga_util_t` created using [optiga_util_create](#optiga_util_create).
	* `perform_hibernate`: Hibernates the application on OPTIGA. The values must be as defined below.
	    * `TRUE (1)` - Performs application hibernate.
	    * `FALSE (0)` - De-Initializes application context.
* **Returns:** 
	* `OPTIGA_CRYPT_SUCCESS` Successful invocation.
	* `OPTIGA_CRYPT_ERROR_INVALID_INPUT` Wrong Input arguments provided.
	* `OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE` The previous operation with the same instance is not complete.
	* `OPTIGA_DEVICE_ERROR` Command execution failure in OPTIGA and the LSB indicates the error code. Refer to the the [Solution Reference Manual](../raw/master/documents/OPTIGA_Trust_Charge_V1_Solution_Reference_Manual_v1.00.pdf)
* **Example:**  N/A
* **Notes:**
    * For **protected I2C communication**, Refer to [OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL](#OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL)
    * Error codes from lower layers will be returned as it is.

[Top](#top)

# <a name="optiga_util_read_data"></a>optiga_util_read_data

* **Prototype**
```c
    optiga_lib_status_t optiga_util_read_data(optiga_util_t * me,
                                              uint16_t optiga_oid,
                                              uint16_t offset,
                                              uint8_t * buffer,
                                              uint16_t * length);
```
* **Description:**    Retrieves the requested data that is stored in the user provided data object.
    * Invokes `optiga_cmd_get_data_object` API and based on the input arguments, read the data from the data object.
* **Pre Conditions:**  The application on OPTIGA must be opened using [`optiga_util_open_application`](#optiga_util_open_application) before using this API.
* **Arguments:**
    * `me`: Valid instance of `optiga_util_t` created using [optiga_util_create](#optiga_util_create).
	* `optiga_oid`:  OID of data object
	* `offset`:  Offset from within data object
        * It must be valid offset from within data object, otherwise OPTIGA returns an error.
    * `buffer`: Valid pointer to the buffer to which data is read
    * `length`: Valid pointer to the length of data to be read from data object.
        * When the data is successfully retrieved, it is updated with actual data length retrieved
* **Returns:** 
	* `OPTIGA_CRYPT_SUCCESS` Successful invocation.
	* `OPTIGA_CRYPT_ERROR_INVALID_INPUT` Wrong Input arguments provided.
	* `OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE` The previous operation with the same instance is not complete.
	* `OPTIGA_DEVICE_ERROR` Command execution failure in OPTIGA and the LSB indicates the error code. Refer to the the [Solution Reference Manual](../raw/master/documents/OPTIGA_Trust_Charge_V1_Solution_Reference_Manual_v1.00.pdf)
* **Example:**  Refer to the [example_optiga_util_read_data.c](../blob/master/examples/optiga/example_optiga_util_read_data.c)
* **Note:**
    * For **protected I2C communication**, Refer to [OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL](#OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL)
    * Error codes from lower layers will be returned as it is.
    * The maximum value of the **length** parameter must be the size of buffer **buffer**. In case the value is greater than buffer size, memory corruption can occur.
    * If any errors occur while retrieving the data, **length** parameter is set to 0.

[Top](#top)

# <a name="optiga_util_read_metadata"></a>optiga_util_read_metadata

* **Prototype**
```c
    optiga_lib_status_t optiga_util_read_metadata(optiga_util_t * me,
                                                  uint16_t optiga_oid,
                                                  uint8_t * buffer,
                                                  uint16_t * length);
```
* **Description:**    Reads the metadata of the user provided data object.
    * Invokes `optiga_cmd_get_data_object` API, based on the input arguments to read the metadata from the data object.
* **Pre Conditions:**  The application on OPTIGA must be opened using [`optiga_util_open_application`](#optiga_util_open_application) before using this API.
* **Arguments:**
    * `me`: Valid instance of `optiga_util_t` created using [optiga_util_create](#optiga_util_create).
	* `optiga_oid`:  OID of data object
	    * It should be a valid data object, otherwise OPTIGA returns an error.
    * `buffer`: Valid pointer to the buffer to which metadata is read
    * `length`: Valid pointer to the length of metadata to be read from data object
        * When the metadata is successfully retrieved, it is updated with actual metadata length retrieved
* **Returns:** 
	* `OPTIGA_CRYPT_SUCCESS` Successful invocation.
	* `OPTIGA_CRYPT_ERROR_INVALID_INPUT` Wrong Input arguments provided.
	* `OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE` The previous operation with the same instance is not complete.
	* `OPTIGA_DEVICE_ERROR` Command execution failure in OPTIGA and the LSB indicates the error code. Refer to the the [Solution Reference Manual](../raw/master/documents/OPTIGA_Trust_Charge_V1_Solution_Reference_Manual_v1.00.pdf)
* **Example:**  Refer to the [example_optiga_util_read_data.c](../blob/master/examples/optiga/example_optiga_util_read_data.c)
* **Note:**
    * For **protected I2C communication**, Refer to [OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL](#OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL)
    * Error codes from lower layers will be returned as it is.
    * The metadata returned will be in TLV format (See [Solution Reference Manual](../raw/master/documents/OPTIGA_Trust_Charge_V1_Solution_Reference_Manual_v1.00.pdf)).
    * The maximum value of the **length** parameter  must be the size of **buffer**. In case the value is greater than buffer size, memory corruption can occur.

[Top](#top)

# <a name="optiga_util_write_data"></a>optiga_util_write_data

* **Prototype**
```c
    optiga_lib_status_t optiga_util_write_data(optiga_util_t * me,
                                               uint16_t optiga_oid,
                                               uint8_t write_type,
                                               uint16_t offset,
                                               const uint8_t * buffer,
                                               uint16_t length);
```
* **Description:**    Writes the data provided by the user into the specified data object.
    * Invokes `optiga_cmd_set_data_object` API, based on the input arguments to write the data to the data object.
* **Pre Conditions:**  The application on OPTIGA must be opened using [`optiga_util_open_application`](#optiga_util_open_application) before using this API.
* **Arguments:**
    * `me`: Valid instance of `optiga_util_t` created using [optiga_util_create](#optiga_util_create).
	* `optiga_oid`:  OID of data object
	    * It should be a valid data object, otherwise OPTIGA returns an error.
    * `write_type`: Type of write must be either:
        * `OPTIGA_UTIL_WRITE_ONLY`
        * `OPTIGA_UTIL_ERASE_AND_WRITE`
    * `offset`: Offset from within data object
        * It must be valid offset from within data object, otherwise OPTIGA returns an error.
    * `buffer`: Valid pointer to the buffer with user data to write
    * `length`: Length of data to be written
* **Returns:** 
	* `OPTIGA_CRYPT_SUCCESS` Successful invocation.
	* `OPTIGA_CRYPT_ERROR_INVALID_INPUT` Wrong Input arguments provided.
	* `OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE` The previous operation with the same instance is not complete.
	* `OPTIGA_DEVICE_ERROR` Command execution failure in OPTIGA and the LSB indicates the error code. Refer to the the [Solution Reference Manual](../raw/master/documents/OPTIGA_Trust_Charge_V1_Solution_Reference_Manual_v1.00.pdf)
* **Example:**  Refer to the [example_optiga_util_write_data.c](../blob/master/examples/optiga/example_optiga_util_write_data.c)
* **Note:**
    * For **protected I2C communication**, Refer to [OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL](#OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL)
    * Error codes from lower layers will be returned as it is.
    * The maximum value of the **length parameter** is size of **buffer**. In case the value is greater than buffer size, incorrect values can get written into the data object in OPTIGA.
    * In case the write_type provided is other than **erase and write(0x00)** or **write only(0x40)**, the function returns `OPTIGA_UTIL_ERROR_INVALID_INPUT`.

[Top](#top)

# <a name="optiga_util_write_metadata"></a>optiga_util_write_metadata

* **Prototype**
```c
    optiga_lib_status_t optiga_util_write_metadata(optiga_util_t * me,
                                                   uint16_t optiga_oid,
                                                   const uint8_t * buffer,
                                                   uint8_t length);
```
* **Description:**    Writes metadata for the specified data object.
    * Invokes `optiga_cmd_set_data_object` API, based on the input arguments to write metadata to the data object.
* **Pre Conditions:**  The application on OPTIGA must be opened using [`optiga_util_open_application`](#optiga_util_open_application) before using this API.
* **Arguments:**
    * `me`: Valid instance of `optiga_util_t` created using [optiga_util_create](#optiga_util_create).
	* `optiga_oid`:  OID of data object
	    * It should be a valid data object, otherwise OPTIGA returns an error.
    * `buffer`: Valid pointer to the buffer with metadata to write
    * `length`: Length of metadata to be written
* **Returns:** 
	* `OPTIGA_CRYPT_SUCCESS` Successful invocation.
	* `OPTIGA_CRYPT_ERROR_INVALID_INPUT` Wrong Input arguments provided.
	* `OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE` The previous operation with the same instance is not complete.
	* `OPTIGA_DEVICE_ERROR` Command execution failure in OPTIGA and the LSB indicates the error code. Refer to the the [Solution Reference Manual](../raw/master/documents/OPTIGA_Trust_Charge_V1_Solution_Reference_Manual_v1.00.pdf)
* **Example:**  Refer to the [example_optiga_util_write_data.c](../blob/master/examples/optiga/example_optiga_util_write_data.c)
* **Note:**
    * For **protected I2C communication**, Refer to [OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL](#OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL)
    * Error codes from lower layers will be returned as it is.
    * The maximum value of the **length** parameter is size of **buffer**. In case the value is greater than buffer size, incorrect values can get written into the data object in OPTIGA.
    * The metadata to be written must be in TLV format (See [Solution Reference Manual](../raw/master/documents/OPTIGA_Trust_Charge_V1_Solution_Reference_Manual_v1.00.pdf)).

[Top](#top)

# <a name="optiga_util_protected_update_start"></a>optiga_util_protected_update_start

* **Prototype**
```c
    optiga_lib_status_t optiga_util_protected_update_start(optiga_util_t * me,
                                                           uint8_t manifest_version,
                                                           const uint8_t * manifest,
                                                           uint16_t manifest_length);
```
* **Description:**    Initiates the start of protected update of object.
    * It marks the beginning of a strict sequence. None of the service layer API will be processed until the strict sequence is terminated.
    * Invokes `optiga_cmd_set_object_protected` API, based on the input arguments to send manifest to OPTIGA.
* **Pre Conditions:**  The application on OPTIGA must be opened using [`optiga_util_open_application`](#optiga_util_open_application) before using this API.
* **Arguments:**
    * `me`: Valid instance of `optiga_util_t` created using [optiga_util_create](#optiga_util_create).
	* `manifest_version`:  Version of manifest to be written
    * `manifest`: Valid pointer to the buffer which contains manifest
        * It should be a valid manifest, otherwise OPTIGA returns an error.
    * `manifest_length`: Length of manifest to be written
* **Returns:** 
	* `OPTIGA_CRYPT_SUCCESS` Successful invocation.
	* `OPTIGA_CRYPT_ERROR_INVALID_INPUT` Wrong Input arguments provided.
	* `OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE` The previous operation with the same instance is not complete.
	* `OPTIGA_DEVICE_ERROR` Command execution failure in OPTIGA and the LSB indicates the error code. Refer to the the [Solution Reference Manual](../raw/master/documents/OPTIGA_Trust_Charge_V1_Solution_Reference_Manual_v1.00.pdf)
* **Example:**  Refer to the [example_optiga_util_protected_update.c](../blob/master/examples/optiga/example_optiga_util_protected_update.c)
* **Note:**
    * For **protected I2C communication**, Refer to [OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL](#OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL)
    * Error codes from lower layers will be returned as it is.
    * The strict sequence is terminated 
        * In case of an error from lower layer.
        * Same instance is used for other service layer APIs (except [`optiga_util_protected_update_continue`](#optiga_util_protected_update_continue)).
    * Any subsequent call to this API with same instance is accepted, provided the previous once is asynchronously completed.
    * For protected update data set generation refer to  [/tools/protected_update_data_set](../blob/master/examples/tools/protected_update_data_set)

[Top](#top)

# <a name="optiga_util_protected_update_continue"></a>optiga_util_protected_update_continue

* **Prototype**
```c
    optiga_lib_status_t optiga_util_protected_update_continue(optiga_util_t * me,
                                                              const uint8_t * fragment,
                                                              uint16_t fragment_length);
```
* **Description:**    Sends a fragment of data to be written to OPTIGA.
    * Invokes `optiga_cmd_set_object_protected` API, to send the fragment of data to write to OPTIGA.
* **Pre Conditions:**  
    * The application on OPTIGA must be opened using [`optiga_util_open_application`](#optiga_util_open_application) before using this API.
    * The manifest must be written to OPTIGA using [`optiga_util_protected_update_start`](#optiga_util_protected_update_start), otherwise lower layer returns an error.
* **Arguments:**
    * `me`: Valid instance of `optiga_util_t` created using [optiga_util_create](#optiga_util_create).
	* `fragment`:  Valid pointer to the buffer which contains fragment
    * `fragment_length`: Length of fragment to be written
* **Returns:** 
	* `OPTIGA_CRYPT_SUCCESS` Successful invocation.
	* `OPTIGA_CRYPT_ERROR_INVALID_INPUT` Wrong Input arguments provided.
	* `OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE` The previous operation with the same instance is not complete.
	* `OPTIGA_DEVICE_ERROR` Command execution failure in OPTIGA and the LSB indicates the error code. Refer to the the [Solution Reference Manual](../raw/master/documents/OPTIGA_Trust_Charge_V1_Solution_Reference_Manual_v1.00.pdf)
* **Example:**  Refer to the [example_optiga_util_protected_update.c](../blob/master/examples/optiga/example_optiga_util_protected_update.c)
* **Note:**
    * For **protected I2C communication**, Refer to [OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL](#OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL)
    * Error codes from lower layers will be returned as it is.
    * For writing 'n' fragment (s) ,
        * if n > 1 : Send 1 to 'n - 1' fragments using [`optiga_util_protected_update_continue`](#optiga_util_protected_update_continue) and and the final fragment using [`optiga_util_protected_update_final`](#optiga_util_protected_update_final).
        * if n == 1 : Send the fragment using [`optiga_util_protected_update_final`](#optiga_util_protected_update_final).
    * Chaining of fragments and the sequence of sending fragments must be handled externally to this API.
    * The strict sequence is terminated in case of an error from lower layer

[Top](#top)

# <a name="optiga_util_protected_update_final"></a>optiga_util_protected_update_final

* **Prototype**
```c
    optiga_lib_status_t optiga_util_protected_update_final(optiga_util_t * me,
                                                           const uint8_t * fragment,
                                                           uint16_t fragment_length);
```
* **Description:**    Sends the last fragment to finalize the protected update of data object.
    * Invokes `optiga_cmd_set_object_protected` API, based on the input arguments to write the final fragment to data object.
    * It terminates the strict sequence.
* **Pre Conditions:**  
    * The application on OPTIGA must be opened using [`optiga_util_open_application`](#optiga_util_open_application) before using this API.
    * The manifest must be written to OPTIGA using [`optiga_util_protected_update_start`](#optiga_util_protected_update_start), otherwise lower layer returns an error.
* **Arguments:**
    * `me`: Valid instance of `optiga_util_t` created using [optiga_util_create](#optiga_util_create).
	* `fragment`:   Valid pointer to the buffer which contains the last fragment.
	    * If NULL, only strict sequence is released and no APDU is sent to OPTIGA
    * `fragment_length`: Length of fragment to be written
* **Returns:** 
	* `OPTIGA_CRYPT_SUCCESS` Successful invocation.
	* `OPTIGA_CRYPT_ERROR_INVALID_INPUT` Wrong Input arguments provided.
	* `OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE` The previous operation with the same instance is not complete.
	* `OPTIGA_DEVICE_ERROR` Command execution failure in OPTIGA and the LSB indicates the error code. Refer to the the [Solution Reference Manual](../raw/master/documents/OPTIGA_Trust_Charge_V1_Solution_Reference_Manual_v1.00.pdf)
* **Example:**  Refer to the [example_optiga_util_protected_update.c](../blob/master/examples/optiga/example_optiga_util_protected_update.c)
* **Note:**
    * For **protected I2C communication**, Refer to [OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL](#OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL)
    * Error codes from lower layers will be returned as it is.
    * The strict sequence is terminated in case of an error from lower layer.

[Top](#top)

# <a name="optiga_util_update_count"></a>optiga_util_update_count

* **Prototype**
```c
    optiga_lib_status_t optiga_util_update_count(optiga_util_t * me,
                                                 uint16_t optiga_counter_oid,
                                                 uint8_t count);
```
* **Description:**   Increments the counter object by a value specified by user.
    * Invokes `optiga_cmd_set_data_object` API, based on the input arguments to update the count to the specified data object.
* **Pre Conditions:**  
    * The application on OPTIGA must be opened using [`optiga_util_open_application`](#optiga_util_open_application) before using this API.
    * Initial count and threshold value must be set using [`optiga_util_write_data`](#optiga_util_write_data).
* **Arguments:**
    * `me`: Valid instance of `optiga_util_t` created using [optiga_util_create](#optiga_util_create).
	* `optiga_counter_oid`:   OID of counter data object
	    * It should be a valid data object, otherwise OPTIGA returns an error.
    * `count`: Counter value to be updated
* **Returns:** 
	* `OPTIGA_CRYPT_SUCCESS` Successful invocation.
	* `OPTIGA_CRYPT_ERROR_INVALID_INPUT` Wrong Input arguments provided.
	* `OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE` The previous operation with the same instance is not complete.
	* `OPTIGA_DEVICE_ERROR` Command execution failure in OPTIGA and the LSB indicates the error code. Refer to the the [Solution Reference Manual](../raw/master/documents/OPTIGA_Trust_Charge_V1_Solution_Reference_Manual_v1.00.pdf)
* **Example:**  Refer to the [example_optiga_util_update_count.c](../blob/master/examples/optiga/example_optiga_util_update_count.c)
* **Note:**
    * For **protected I2C communication**, Refer to [OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL](#OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL)
    * Error codes from lower layers will be returned as it is.

[Top](#top)

# <a name="OPTIGA_UTIL_SET_COMMS_PROTECTION_LEVEL"></a>OPTIGA_UTIL_SET_COMMS_PROTECTION_LEVEL

* **Prototype**
```c
    #define OPTIGA_UTIL_SET_COMMS_PROTECTION_LEVEL(p_instance, protection_level) \
    { \
        optiga_util_set_comms_params(p_instance, \
                                            OPTIGA_COMMS_PROTECTION_LEVEL, \
                                            protection_level); \
    }
```
* **Description:**   Enables the protected I2C communication with OPTIGA
    * Sets the protection mode for the supplied instance.
    * Call this function before calling the service layer API which requires protection.
* **Pre Conditions:**  
    * `OPTIGA_COMMS_SHIELDED_CONNECTION` macro must be defined.
    * `OPTIGA_UTIL_SET_COMMS_PROTOCOL_VERSION` function must be called once to set the required protocol version
        * Currently only Pre-shared Secret based version is supported.
* **Arguments:**
    * `p_instance`: Valid pointer to an instance.
	* `protection_level`:  Required protection mode
	    * `OPTIGA_COMMS_NO_PROTECTION` : Command and response is unprotected
	    * `OPTIGA_COMMS_COMMAND_PROTECTION` : Command is protected and response is unprotected
	    * `OPTIGA_COMMS_RESPONSE_PROTECTION` : Command is unprotected and response is protected
	    * `OPTIGA_COMMS_FULL_PROTECTION` : Both command and response is protected
	    * To re-establish secure channel, bitwise-OR protection_level with `OPTIGA_COMMS_RE_ESTABLISH`
* **Returns:** None
* **Example:**  Refer to the [example_optiga_util_write_data.c](../blob/master/examples/optiga/example_optiga_util_write_data.c)
* **Note:**
    * The protection mode for the instance is reset to #OPTIGA_COMMS_NO_PROTECTION once the service layer API returns synchronously.

[Top](#top)

# <a name="OPTIGA_UTIL_SET_COMMS_PROTOCOL_VERSION"></a>OPTIGA_UTIL_SET_COMMS_PROTOCOL_VERSION

* **Prototype**
```c
    #define OPTIGA_UTIL_SET_COMMS_PROTOCOL_VERSION(p_instance, version) \
    { \
        optiga_util_set_comms_params(p_instance, \
                                            OPTIGA_COMMS_PROTOCOL_VERSION, \
                                            version);\
    }
```
* **Description:**   Select the protocol version required for the I2C protected communication
    * Select the protocol version for the supplied instance.
* **Pre Conditions:**  
    * `OPTIGA_COMMS_SHIELDED_CONNECTION` macro must be defined.
* **Arguments:**
    * `p_instance`: Valid pointer to an instance.
	* `version`:  Required protocol version
	    * `OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET` : Pre-shared Secret based protocol version
* **Returns:** None
* **Example:**  Refer to the [example_optiga_util_write_data.c](../blob/master/examples/optiga/example_optiga_util_write_data.c)
* **Note:** N/A

[Top](#top)
