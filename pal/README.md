# Porting guide

1. [Port Platfrom Abstraction Layer](#port-platfrom-abstraction-layer)
2. [Initialisation Flow](#initialisation-flow)
3. [Port Crypto module for Platfrom Abstraction Layer](#port-crypto-module-for-platfrom-abstraction-layer)
4. [Port Event module for Platform Abstraction Layer](#port-event-module-for-platform-abstraction-layer)
    * [No timer callbacks are possible, what to do?](#no-timer-callbacks-are-possible-what-to-do)  

## Port Platfrom Abstraction Layer

To port the Software Framework on you Host platfrom please follow the guidance below. As a basis for your new PAL implementation, you can take this [template](NEW_PAL_TEMPLATE)

**Note: If you have a strict requirements on type of memmory allocation, consider changing pal_os_malloc(), pal_os_calloc(), pal_os_free() functions in the [pal_os_memory.h](https://github.com/Infineon/optiga-trust-charge/blob/master/optiga/include/optiga/pal/pal_os_memory.h) file**

The implementation of Platform Abstraction Layer (PAL) needs to be updated in order to migrate to a new
target platform.

The PAL reference code for the xmc4700 IoT connectivity kit is provided as part of package which can be used.
The implementation can be found in “pal/xmc4700” and the header files are available in
“optiga/include” with the required APIs used by upper layers. The header files are platform
agnostic and would not require any changes. The low level drivers used by PAL for xmc4700 are configured and
generated using DAVE™.

### Communication with OPTIGA™ Trust Charge

The hardware/platform resource configuration with respect to I2C master and GPIOs (Vdd and Reset) are to be
updated in pal_ifx_i2c_config.c. These configurations are used by the IFX I2C implementation to communicate
with OPTIGA™ Trust Charge.

### Update I2C master platform specific context[e.g. (void)&i2c_master_0]

```c
/*
* \brief PAL I2C configuration for OPTIGA
*/
pal_i2c_t optiga_pal_i2c_context_0 =
{
    /// Pointer to I2C master platform specific context
    (void)&i2c_master_0,
    /// Slave address
    0x30,
    /// Upper layer context
    NULL,
    /// Callback event handler
    NULL
};
```

### Update platform specific context for GPIOs (Vdd and Reset) [e.g. (void)&pin_3_4]
```c
/*
* \brief Vdd pin configuration for OPTIGA
*/
pal_gpio_t optiga_vdd_0 =
{
    // Platform specific GPIO context for the pin used to toggle Vdd
    (void*)&vdd_pin
};

/**
* \brief Reset pin configuration for OPTIGA
*/
 pal_gpio_t optiga_reset_0 =
{
    // Platform specific GPIO context for the pin used to toggle Reset
    (void*)&reset_pin
};
```

### Update PAL I2C APIs [pal_i2c.c] to communicate with OPTIGA™ Trust Charge

The `pal_i2c` is expected to provide the APIs for I2C driver initialization, de-initialization, read, write and set
bitrate kind of operations

1. `pal_i2c_init`
2. `pal_i2c_deinit`
3. `pal_i2c_read`
4. `pal_i2c_write`
5. `pal_i2c_set_bitrate`

A few target platforms, the I2C master driver initialization (`pal_i2c_init`) is done during the platform start up. In
such an environment, there is no need to implement pal_i2c_init and `pal_i2c_deinit` functions. Otherwise,
these (`pal_i2c_init` & `pal_i2c_deinit`) functions must be implemented as per the upper layer expectations based
on the need. The details of these expectations are available in the [Host library API documentation (chm, download link)](https://github.com/Infineon/optiga-trust-charge/raw/master/documents/OPTIGA_Trust_Charge_V1_Host_Library_Documentation.chm).

The reference implementation of PAL I2C based on xmc4700 IoT connectivity kit does not need to have the
platform I2C driver initialization explicitly done as part of pal_i2c_init as it is taken care by the DAVE™ library
initialization. Hence pal_i2c_init & pal_i2c_deinit are not implemented.

In addition to the above specified APIs, the PAL I2C must handle the events from the low level I2C driver and
invoke the upper layer handlers registered with PAL I2C context for the respective transaction as shown in the
below example.

```c
//I2C driver callback function when the transmit is completed successfully
void i2c_master_end_of_transmit_callback(void)
{
    invoke_upper_layer_callback(gp_pal_i2c_current_ctx, (uint8_t)PAL_I2C_EVENT_TX_SUCCESS);
}
```

In above example the I2C driver callback, when transmission is successful invokes the handler to inform the
result.

### Update PAL GPIO [pal_gpio.c] to power on and reset the OPTIGA™ Trust Charge

1. `pal_gpio_set_high`
2. `pal_gpio_set_low`

### Update PAL Timer [pal_os_timer.c] to enable timer

1. `pal_os_timer_get_time_in_milliseconds`
2 `pal_os_timer_delay_in_milliseconds`

### Update Event management for the asynchronous interactions for IFX I2C [pal_os_event.c]

1. `pal_os_event_register_callback_oneshot`
2. `pal_os_event_trigger_registered_callback`

The `pal_os_event_register_callback_oneshot` function is expected to register the handler and context
provided as part of input parameters and triggers the timer for the requested time. The `p_pal_os_event` is an
event instance created using `pal_os_event_create`.

```c
void pal_os_event_register_callback_oneshot(pal_os_event_t * p_pal_os_event,
                                            register_callback callback,
                                            void* callback_args,
                                            uint32_t time_us)
{
    p_pal_os_event->callback_registered = callback;
    p_pal_os_event->callback_ctx = callback_args;
    //lint --e{534} suppress "Return value is not required to be checked"
    TIMER_SetTimeInterval(&scheduler_timer, (time_us*100));
    TIMER_Start(&scheduler_timer);
}
```

The handler registered must be invoked once the timer has elapsed as shown in
`pal_os_event_trigger_registered_callback`. The `pal_os_event_trigger_registered_callback` is to be
registered with event timer interrupt to get trigerred when the timer expires. The `pal_os_event_0` is the
instance in the `pal_os_event` used store the registered callback and context.

```c
void pal_os_event_trigger_registered_callback(void)
{
    register_callback callback;

    TIMER_ClearEvent(&scheduler_timer);
    //lint --e{534} suppress "Return value is not required to be checked"
    TIMER_Stop(&scheduler_timer);
    TIMER_Clear(&scheduler_timer);

    if (pal_os_event_0.callback_registered)
    {
        Callback = pal_os_event_0.callback_registered;
        callback ((void*)callback_ctx);
    }
}
```

## Reference code on xmc4700 for communicating with OPTIGA™ Trust Charge
```c
static volatile uint32_t optiga_pal_event_status;
static void optiga_pal_i2c_event_handler(void* upper_layer_ctx, uint8_t event);

pal_i2c_t optiga_pal_i2c_context_0 =
{
    /// Pointer to I2C master platform specific context
    (void*)&i2c_master_0,
    /// Slave address
    0x30,
    /// Upper layer context
    NULL,
    /// Callback event handler
    NULL,
};

// OPTIGA pal i2c event handler
static void optiga_pal_i2c_event_handler(void* upper_layer_ctx, uint8_t event)
{
    optiga_pal_event_status = event;
}

/* Function to verify I2C communication with OPTIGA */
pal_status_t test_optiga_communication(void)
{
    pal_status_t pal_return_status;
    uint8_t data_buffer[10] = {0x82};
    
    // set callback handler for pal i2c
    optiga_pal_i2c_context_0.upper_layer_event_handler =
    optiga_pal_i2c_event_handler;
    
    // Send 0x82 to read I2C_STATE from optiga
    do
    {
        optiga_pal_event_status = PAL_I2C_EVENT_BUSY;
        pal_return_status = pal_i2c_write(&optiga_pal_i2c_context_0, data_buffer, 1);
        if (PAL_STATUS_FAILURE == pal_return_status)
        {
            // Pal I2C write failed due to I2C busy is in busy
            // state or low level driver failures
            break;
        }

    // Wait until writing to optiga is completed
    } while (PAL_I2C_EVENT_SUCCESS != optiga_pal_event_status);


    // Read the I2C_STATE from OPTIGA
    do
    {
        optiga_pal_event_status = PAL_I2C_EVENT_BUSY;
        pal_return_status = pal_i2c_read(&optiga_pal_i2c_context_0, data_buffer, 4);
        // Pal I2C read failed due to I2C busy is in busy
        // state or low level driver failures
        if (PAL_STATUS_FAILURE == pal_return_status)
        {
            break;
        }
        // Wait until reading from optiga is completed
    } while (PAL_I2C_EVENT_SUCCESS != optiga_pal_event_status);

    return pal_return_status;
}

/* Main Function */
int32_t main(void)
{
    DAVE_STATUS_t status;
    pal_status_t pal_return_status;
    
    // Initialisation of DAVE Apps
    status = DAVE_Init();
    
    // Stop if DAVE init fails
    if (DAVE_STATUS_FAILURE == status)
    {
        while (1U)
        {;}
    }
     pal_return_status = test_optiga_communication();
    
     return (int32_t)pal_return_status;
}
```


## Initialisation flow

To test whether your PAL is working, you might call the following routine. It does open a communication channel with the chip.
```c
#include "optiga/optiga_util.h"
#include "optiga/pal/pal_os_event.h"
#include "optiga/pal/pal.h"
#include "optiga/pal/pal_os_timer.h"
/**
 * Callback when optiga_util_xxxx operation is completed asynchronously
 */
static volatile optiga_lib_status_t optiga_lib_status;
static void optiga_util_callback(void * context, optiga_lib_status_t return_status)
{
    optiga_lib_status = return_status;
}

int32_t main(void)
{
    uint8_t return_value = 0;

    optiga_lib_status_t return_status;

    optiga_util_t * me_util;

    do
    {
        //Create an instance of optiga_util to open the application on OPTIGA.
        me_util = optiga_util_create(0, optiga_util_callback, NULL);

        /**
         * Open the application on OPTIGA which is a precondition to perform any other operations
         * using optiga_util_open_application
         */        
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_open_application(me_util, 0);

        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }
        while (optiga_lib_status == OPTIGA_LIB_BUSY)
        {
            //Wait until the optiga_util_open_application is completed
        }
        if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
        {
            //optiga util open application failed
            break;
        }
        
        /*
         Paste your code is here
        */

                /**
         * Close the application on OPTIGA after all the operations are executed
         * using optiga_util_close_application
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_close_application(me_util, 0);
        
        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }

        while (optiga_lib_status == OPTIGA_LIB_BUSY)
        {
            //Wait until the optiga_util_close_application is completed
        }
        
        if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
        {
            //optiga util close application failed
            break;
        }

        // destroy util and crypt instances
        optiga_util_destroy(me_util);
    }while (FALSE);

    return return_value;
}
```

## Port Crypto module for Platfrom Abstraction Layer

The Crypto PAL helps a Host MCU to perfrom shielded communication (protected Infineon I2C protocol) between the Host and the Trust Charge

If you don't use shielded connection you can skip this module

Currently three Crypto PALs are supported via third-party libraries (should be provided at compilation/linking time)
1. mbed TLS Crypto Library
1. OpenSSL Crypto Library
1. WolfSSL Crypto Library

There are three functions required to be implemented by the Crypto PAL, these are:
1. `pal_crypt_tls_prf_sha256`    
    <details> <summary> Test Vectors </summary> 

    ```c
    **********************************************************************************************

    static const uint8_t example_secret[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };

    static const uint8_t example_label[] = {
        0x42, 0x61, 0x62, 0x79, 0x6c, 0x6f, 0x6e, 0x20,
        0x50, 0x52, 0x46, 0x20, 0x41, 0x70, 0x70, 0x4e,
        0x6f, 0x74, 0x65
    };

    static const uint8_t example_seed[] = {
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
        0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f
    };

    static const uint8_t example_result[] = {
        0xbf, 0x88, 0xeb, 0xde, 0xfa, 0x78, 0x46, 0xa1,
        0x10, 0x55, 0x91, 0x88, 0xd4, 0x22, 0xf3, 0xf7,
        0xfa, 0xfe, 0xf4, 0xa5, 0x49, 0xbd, 0xaa, 0xce,
        0x37, 0x39, 0xc9, 0x44, 0x65, 0x7f, 0x2d, 0xd9,
        0xbc, 0x30, 0x83, 0x14, 0x47, 0xd0, 0xed, 0x1c,
        0x89, 0xf6, 0x58, 0x23, 0xb2, 0xec, 0xe0, 0x52,
        0xf3, 0xb7, 0x95, 0xed, 0xe8, 0x6c, 0xad, 0x59,
        0xca, 0x47, 0x3b, 0x3a, 0x78, 0x98, 0x63, 0x69,
        0x44, 0x65, 0x62, 0xc9, 0xa4, 0x0d, 0x6a, 0xac,
        0x59, 0xa2, 0x04, 0xfa, 0x0e, 0x44, 0xb7, 0xd7
    };

    **********************************************************************************************

    // Test Vector from website www.ietf.org/mail-archive/web/tls/current/msg03416.html
    // Secret
    static const uint8_t tls_at_ietf_secret[] = 
    {
        0x9b,0xbe,0x43,0x6b,0xa9,0x40,0xf0,0x17,    
        0xb1,0x76,0x52,0x84,0x9a,0x71,0xdb,0x35      
    };

    // Seed
    static const uint8_t tls_at_ietf_seed[] = 
    {
        0xa0,0xba,0x9f,0x93,0x6c,0xda,0x31,0x18,    
        0x27,0xa6,0xf7,0x96,0xff,0xd5,0x19,0x8c       
    };

    // Label test label (0x74,0x65,0x73,0x74,0x20,0x6c,0x61,0x62,0x65,0x6c)
    static const uint8_t tls_at_ietf_label[] = 
    {
        0x74,0x65,0x73,0x74,0x20,0x6c,0x61,0x62,
        0x65,0x6c
    };

    // Output
    static const uint8_t tls_at_ietf_output[] = 
    {
        0xe3,0xf2,0x29,0xba,0x72,0x7b,0xe1,0x7b,    
        0x8d,0x12,0x26,0x20,0x55,0x7c,0xd4,0x53,    
        0xc2,0xaa,0xb2,0x1d,0x07,0xc3,0xd4,0x95,    
        0x32,0x9b,0x52,0xd4,0xe6,0x1e,0xdb,0x5a,    
        0x6b,0x30,0x17,0x91,0xe9,0x0d,0x35,0xc9,    
        0xc9,0xa4,0x6b,0x4e,0x14,0xba,0xf9,0xaf,    
        0x0f,0xa0,0x22,0xf7,0x07,0x7d,0xef,0x17,    
        0xab,0xfd,0x37,0x97,0xc0,0x56,0x4b,0xab,    
        0x4f,0xbc,0x91,0x66,0x6e,0x9d,0xef,0x9b,    
        0x97,0xfc,0xe3,0x4f,0x79,0x67,0x89,0xba,    
        0xa4,0x80,0x82,0xd1,0x22,0xee,0x42,0xc5,    
        0xa7,0x2e,0x5a,0x51,0x10,0xff,0xf7,0x01,    
        0x87,0x34,0x7b,0x66    
    };
    ```

    </details>

1. `pal_crypt_encrypt_aes128_ccm`
    <details> <summary> Test Vectors </summary> 

    ```c
    /* key */
    static const uint8_t key[] =
    {
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
        0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf
    };

    /* nonce */
    static const uint8_t nonce[] =
    {
        0x00, 0x00, 0x00, 0x03, 0x02, 0x01, 0x00, 0xa0,
        0xa1, 0xa2, 0xa3, 0xa4, 0xa5
    };

    /* plaintext */
    static const uint8_t plainText[] =
    {
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e
    };

    //Associated Data
    static const uint8_t aData[] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
    };

    //Cipher text
    static const uint8_t cipherText[] =
    {
        0x58, 0x8c, 0x97, 0x9a, 0x61, 0xc6, 0x63, 0xd2,
        0xf0, 0x66, 0xd0, 0xc2, 0xc0, 0xf9, 0x89, 0x80,
        0x6d, 0x5f, 0x6b, 0x61, 0xda, 0xc3, 0x84
    };

    //MAC
    static const uint8_t mac[] =
    {
    0x17, 0xe8, 0xd1, 0x2c, 0xfd, 0xf9, 0x26, 0xe0
    };
    ```

    </details>
    
1. `pal_crypt_decrypt_aes128_ccm`
    <details> <summary> Test Vectors </summary> 

    ```c
    /* key */
    static const uint8_t key[] =
    {
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
        0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf
    };

    /* nonce */
    static const uint8_t nonce[] =
    {
        0x00, 0x00, 0x00, 0x03, 0x02, 0x01, 0x00, 0xa0,
        0xa1, 0xa2, 0xa3, 0xa4, 0xa5
    };

    /* plaintext */
    static const uint8_t plainText[] =
    {
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e
    };

    //Associated Data
    static const uint8_t aData[] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
    };

    //Cipher text
    static const uint8_t cipherText[] =
    {
        0x58, 0x8c, 0x97, 0x9a, 0x61, 0xc6, 0x63, 0xd2,
        0xf0, 0x66, 0xd0, 0xc2, 0xc0, 0xf9, 0x89, 0x80,
        0x6d, 0x5f, 0x6b, 0x61, 0xda, 0xc3, 0x84
    };

    //MAC
    static const uint8_t mac[] =
    {
    0x17, 0xe8, 0xd1, 0x2c, 0xfd, 0xf9, 0x26, 0xe0
    };
    ```

    </details>
    
    
A simple test suite can look like following

<details><summary> Simple Test suite </summary>

```c
    
static int32_t testTLSPRF256(void)
{
    int32_t ret = 1;
    static uint8_t example_secret[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };

    static uint8_t example_label[] = {
        0x42, 0x61, 0x62, 0x79, 0x6c, 0x6f, 0x6e, 0x20,
        0x50, 0x52, 0x46, 0x20, 0x41, 0x70, 0x70, 0x4e,
        0x6f, 0x74, 0x65
    };

    static uint8_t example_seed[] = {
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
        0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f
    };

    static uint8_t example_result[] = {
        0xbf, 0x88, 0xeb, 0xde, 0xfa, 0x78, 0x46, 0xa1,
        0x10, 0x55, 0x91, 0x88, 0xd4, 0x22, 0xf3, 0xf7,
        0xfa, 0xfe, 0xf4, 0xa5, 0x49, 0xbd, 0xaa, 0xce,
        0x37, 0x39, 0xc9, 0x44, 0x65, 0x7f, 0x2d, 0xd9,
        0xbc, 0x30, 0x83, 0x14, 0x47, 0xd0, 0xed, 0x1c,
        0x89, 0xf6, 0x58, 0x23, 0xb2, 0xec, 0xe0, 0x52,
        0xf3, 0xb7, 0x95, 0xed, 0xe8, 0x6c, 0xad, 0x59,
        0xca, 0x47, 0x3b, 0x3a, 0x78, 0x98, 0x63, 0x69,
        0x44, 0x65, 0x62, 0xc9, 0xa4, 0x0d, 0x6a, 0xac,
        0x59, 0xa2, 0x04, 0xfa, 0x0e, 0x44, 0xb7, 0xd7
    };
    uint8_t p_derived_key[sizeof(example_result)];
    memset(p_derived_key, 0x00, sizeof(example_result));

    do 
    {
        pal_crypt_tls_prf_sha256(NULL,
                                 example_secret,
                                 sizeof(example_secret),
                                 example_label,
                                 sizeof(example_label),
                                 example_seed,
                                 sizeof(example_seed),
                                 p_derived_key,
                                 sizeof(example_result));
                                 
        if (memcmp(p_derived_key, example_result, sizeof(example_result)) != 0)
        {
            output_result("Derived Key: ", p_derived_key, sizeof(p_derived_key));
            output_result("Expected Key: ", example_result, sizeof(example_result));
            break;
        }
        
        ret = 0;
    }while(0);
    
    return ret;
}

#define NB_TESTS 3
#define CCM_SELFTEST_PT_MAX_LEN 24
#define CCM_SELFTEST_CT_MAX_LEN 32
/*
 * The data is the same for all tests, only the used length changes
 */
static const uint8_t key[] = {
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
    0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f
};

static const uint8_t iv[] = {
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b
};

static const uint8_t ad[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13
};

static uint8_t msg[CCM_SELFTEST_PT_MAX_LEN] = {
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
};

static const size_t iv_len [NB_TESTS] = { 7, 8,  12 };
static const size_t add_len[NB_TESTS] = { 8, 16, 20 };
static const size_t msg_len[NB_TESTS] = { 4, 16, 24 };
static const size_t tag_len[NB_TESTS] = { 4, 6,  8  };

static  uint8_t res[NB_TESTS][CCM_SELFTEST_CT_MAX_LEN] = {
    {   0x71, 0x62, 0x01, 0x5b, 0x4d, 0xac, 0x25, 0x5d },
    {   0xd2, 0xa1, 0xf0, 0xe0, 0x51, 0xea, 0x5f, 0x62,
        0x08, 0x1a, 0x77, 0x92, 0x07, 0x3d, 0x59, 0x3d,
        0x1f, 0xc6, 0x4f, 0xbf, 0xac, 0xcd },
    {   0xe3, 0xb2, 0x01, 0xa9, 0xf5, 0xb7, 0x1a, 0x7a,
        0x9b, 0x1c, 0xea, 0xec, 0xcd, 0x97, 0xe7, 0x0b,
        0x61, 0x76, 0xaa, 0xd9, 0xa4, 0x42, 0x8a, 0xa5,
        0x48, 0x43, 0x92, 0xfb, 0xc1, 0xb0, 0x99, 0x51 }
};


static int32_t testAES128CCMEncrypt(void)
{
    int32_t ret = 1;
    uint8_t plaintext[CCM_SELFTEST_PT_MAX_LEN];
    uint8_t ciphertext[CCM_SELFTEST_CT_MAX_LEN];

    do 
    {
        for( int i = 0; i < NB_TESTS; i++ )
        {
            memset( plaintext, 0, CCM_SELFTEST_PT_MAX_LEN );
            memset( ciphertext, 0, CCM_SELFTEST_CT_MAX_LEN );
            memcpy( plaintext, msg, msg_len[i] );
            
            pal_crypt_encrypt_aes128_ccm(NULL,
                                         plaintext,
                                         CCM_SELFTEST_PT_MAX_LEN,
                                         key,
                                         iv,
                                         iv_len[i],
                                         ad,
                                         add_len[i],
                                         tag_len[i],
                                         ciphertext);
            if (memcmp(ciphertext, res[i], msg_len[i] + tag_len[i])!= 0)
            {
                output_result("Result Cipher: ", ciphertext, msg_len[i] + tag_len[i]);
                output_result("Expected Cipher: ", res[i], msg_len[i] + tag_len[i]);
                break;
            }
        }
        
        ret = 0;
    }while(0);
    
    return ret;
}

static int32_t testAES128CCMDecrypt(void)
{
    int32_t ret = 1;
    uint8_t plaintext[CCM_SELFTEST_PT_MAX_LEN];
    uint8_t ciphertext[CCM_SELFTEST_CT_MAX_LEN];

    do 
    {
        for( int i = 0; i < NB_TESTS; i++ )
        {
            memset( plaintext, 0, CCM_SELFTEST_PT_MAX_LEN );
            memset( ciphertext, 0, CCM_SELFTEST_CT_MAX_LEN );
            memcpy( plaintext, msg, msg_len[i] );
            
            pal_crypt_decrypt_aes128_ccm( NULL,
                                      res[i],
                                      CCM_SELFTEST_PT_MAX_LEN,
                                      key,
                                      iv,
                                      iv_len[i],
                                      ad,
                                      add_len[i],
                                      tag_len[i],
                                      plaintext);
            if (memcmp(plaintext, msg, msg_len[i]) != 0)
            {
                output_result("Result Plain Text: ", plaintext, msg_len[i]);
                output_result("Expected Plain Text: ", msg, msg_len[i]);
                break;
            }
        }
        
        ret = 0;
    }while(0);
    
    return ret;
}
```
</details>


## Port Event module for Platform Abstraction Layer

To better port the pal_os_event.c file to the target platform it is important to udnerstand how the OPTIGA finites state machine makes use of it. Below is the pseudo code which should help in that. 

File `pseudo_code_optiga.c`:

```c
    optiga_finite_state_machine_callback()
    {
        optiga_function_send_or_receive_one_i2c_frame()
        pal_os_event_register_callback_oneshot(for_example_1000usec, optiga_finite_state_machine_callback, some_arguments)
    }
```


File `target_pal_os_event.c`:

```c
    pal_os_event_register_callback_oneshot(usec_delay, callback, callback_args)
    {
        target_interrupt_service_routine_for_timers_set_callback(usec_delay, pal_os_event_trigger_registered_callback)
        pal_os_event_0.callback_registered = callback;
        pal_os_event_0.callback_ctx = callback_args;
    }


    pal_os_event_trigger_registered_callback()
    {
        register_callback callback;

        if (pal_os_event_0.callback_registered)
        {
            callback = pal_os_event_0.callback_registered;
            // From this place the optiga_finite_state_machine_callback() will be called again do its job and call the pal_os_event_register_callback_oneshot once again to do a next portion of the job
            callback((void * )pal_os_event_0.callback_ctx); 
        }

    }

```

so the sequence to send and receive multiple I2C Frames would be the following:

```
|Pseudo timestamp in usec | Function                                                                                                          |
|-------------------------|-------------------------------------------------------------------------------------------------------------------|
|         0               | optiga_finite_state_machine_callback()                                                                            |
|         1               |   optiga_function_send_one_i2c_frame()                                                                            |
|         11              |   pal_os_event_register_callback_oneshot(1000, optiga_finite_state_machine_callback, NULL)                        |
|         12              |     target_interrupt_service_routine_for_timers_set_callback(1000, pal_os_event_trigger_registered_callback)      |
|         ...             |     ...                                                                                                           |
|         1012            | target_interrupt_service_routine_for_timers()                                                                     |
|         1013            |   pal_os_event_trigger_registered_callback()                                                                      | 
|         1014            |     optiga_finite_state_machine_callback()                                                                        |
|         1015            |       optiga_function_receive_one_i2c_frame()                                                                     |
|         1016            |       pal_os_event_register_callback_oneshot(1000, optiga_finite_state_machine_callback, NULL)                    |
|         1017            |         target_interrupt_service_routine_for_timers_set_callback(1000, pal_os_event_trigger_registered_callback)  |
|         ...             |     ...                                                                                                           |
|         2017            | target_interrupt_service_routine_for_timers()                                                                     |
|         2018            |   pal_os_event_trigger_registered_callback()                                                                      | 
|         2019            |     optiga_finite_state_machine_callback()                                                                        |
|         2020            |       optiga_function_send_one_i2c_frame()                                                                        |
|         2021            |       pal_os_event_register_callback_oneshot(1000, optiga_finite_state_machine_callback, NULL)                    |
```
and so one and so forth


### No timer callbacks are possible, what to do?

Sometimes timer callbacks are not available or for instance it is not allowed to call functions from an interrupt service routine (like timestamp 1013, 2018 from the table above).
In this case there is a possibility to move the finite state machine still. But it requires some modifications.

Below is a pseudo implementation for the `target_pal_os_event.c`, please note that not all functions are shown to save space on the screen:

```c
static uint32_t pal_os_ts_0 = 0;

void pal_os_event_trigger_registered_callback(void)
{
    register_callback callback;

    if ((pal_os_ts_0 != 0) && (pal_os_ts_0 < target_function_to_return_system_clock_in_usecs()()) && pal_os_event_0.callback_registered)
    {
        pal_os_ts_0 = 0;
        callback = pal_os_event_0.callback_registered;
        callback((void * )pal_os_event_0.callback_ctx);
    }
}


void pal_os_event_register_callback_oneshot(pal_os_event_t * p_pal_os_event,
                                            register_callback callback,
                                            void* callback_args,
                                            uint32_t time_us)
{
	pal_os_event_0.callback_registered = callback;
	pal_os_event_0.callback_ctx = callback_args;

	pal_os_ts_0 = target_function_to_return_system_clock_in_usecs() + time_us;
}

```

Now we need to regularly poll the pal_os_event_trigger_registered_callback() to ensure that the optiga finite state machine moves forward. 
This can be done in many different ways, for example the implementation of the `qi_auth_ptx_crypt_wocmd.c` file can be changed as following:

```c
int transceive_to_chip( optiga_comms_t * p_comms,
                        const uint8_t * p_tx_data,
                        uint16_t tx_data_length,
                        uint8_t * p_rx_data,
                        uint16_t * p_rx_data_len)
{
    // transceive data to chip using comms tranceive
    optiga_comms_status = OPTIGA_COMMS_BUSY;
    if(OPTIGA_COMMS_SUCCESS != optiga_comms_transceive(p_comms, p_tx_data, tx_data_length, p_rx_data ,p_rx_data_len))
    {
        return CRYPT_LIB_ERROR;
    }
    //async wait
    while(OPTIGA_COMMS_SUCCESS != optiga_comms_status)
    {
        pal_os_event_trigger_registered_callback();
    };

    return 0;
}
```

if you don't use a reduced PTx implementation, the polling can be integrated differently, for instance in the file `qi_auth_ptx_crypt.c`:

```c

#define WAIT_FOR_COMPLETION(ret) \
    if (OPTIGA_LIB_SUCCESS != ret)\
    {\
        break;\
    }\
    while (optiga_lib_status == OPTIGA_LIB_BUSY) \
    {\
        pal_os_event_trigger_registered_callback();\
    } \
    \
    if (OPTIGA_LIB_SUCCESS != optiga_lib_status)\
    { \
            ret = optiga_lib_status;\
            /*printf("Error: 0x%02X \r\n", optiga_lib_status);*/ \
            optiga_util_destroy(p_util);\
            optiga_crypt_destroy(p_crypt);\
            pal_os_event_destroy(NULL);\
            break; \
    }
#endif

```
