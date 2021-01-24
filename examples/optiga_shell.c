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
* \file optiga_shell.c
*
* \brief   This file provides the shell prompt implementation.
*
* \ingroup grOptigaExamples
*
* @{
*/

#include <DAVE.h>
#include "optiga/optiga_crypt.h"
#include "optiga/optiga_util.h"
#include "optiga/common/optiga_lib_logger.h"
#include "optiga/pal/pal_os_event.h"
#include "optiga/pal/pal.h"
#include "optiga/pal/pal_os_timer.h"
#include "optiga_example.h"
#include "optiga/pal/pal_logger.h"

#define OPTIGA_SHELL_MODULE "[optiga shell]  : "
#define OPTIGA_SHELL_LOG_MESSAGE(msg) \
                            optiga_lib_print_message(msg, OPTIGA_SHELL_MODULE, OPTIGA_LIB_LOGGER_COLOR_LIGHT_GREEN);
                            
#define OPTIGA_SHELL_LOG_ERROR_MESSAGE(msg) \
                            optiga_lib_print_message(msg, OPTIGA_SHELL_MODULE, OPTIGA_LIB_LOGGER_COLOR_YELLOW);

extern void example_optiga_crypt_hash (void);
extern void example_optiga_crypt_ecc_generate_keypair(void);
extern void example_optiga_crypt_ecdsa_sign(void);
extern void example_optiga_crypt_ecdsa_verify(void);
extern void example_optiga_crypt_random(void);
extern void example_optiga_util_read_data(void);
extern void example_optiga_util_write_data(void);
extern void example_optiga_util_update_count(void);
extern void example_optiga_util_protected_update(void);
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
extern void example_pair_host_and_optiga_using_pre_shared_secret(void);
#endif
extern void example_optiga_util_hibernate_restore(void);
extern void example_read_coprocessor_id(void);
extern void example_optiga_crypt_hash_data(void);

extern pal_logger_t logger_console;
/**
 * Callback when optiga_util_xxxx operation is completed asynchronously
 */
static volatile optiga_lib_status_t optiga_lib_status;
//lint --e{818,715} suppress "argument "context" is not used in the sample provided"
static void optiga_util_callback(void * context, optiga_lib_status_t return_status)
{
    optiga_lib_status = return_status;
}

optiga_util_t * me_util = NULL;

typedef struct optiga_example_cmd
{
    const char_t * cmd_description;
    const char_t * cmd_options;
    void (*cmd_handler)(void);
}optiga_example_cmd_t;

static void optiga_shell_init(void)
{
    optiga_lib_status_t return_status = !OPTIGA_LIB_SUCCESS;
    uint16_t optiga_oid = 0xE0C4;
    uint8_t required_current = 15;

    do
    {
        if (NULL == me_util)
        {
            //Create an instance of optiga_util to open the application on OPTIGA.
            me_util = optiga_util_create(0, optiga_util_callback, NULL);
        }

        OPTIGA_EXAMPLE_LOG_MESSAGE("Initializing OPTIGA for example demonstration...\n");
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
            return_status = optiga_lib_status;
            //optiga util open application failed
            break;
        }

        OPTIGA_SHELL_LOG_MESSAGE("Initializing OPTIGA completed...\n\n");
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION         
        OPTIGA_SHELL_LOG_MESSAGE("Begin pairing of host and OPTIGA...");
        // Usercase: Generate the pre-shared secret on host and write it to OPTIGA
        example_pair_host_and_optiga_using_pre_shared_secret();
        OPTIGA_SHELL_LOG_MESSAGE("Pairing of host and OPTIGA completed...");
#endif        
        // Setting current limitation to maximum supported value(15)
        return_status = optiga_util_write_data(me_util,
                                               optiga_oid,
                                               OPTIGA_UTIL_ERASE_AND_WRITE,
                                               0,
                                               &required_current,
                                               1);

        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }

        while (OPTIGA_LIB_BUSY == optiga_lib_status)
        {
            return_status = optiga_lib_status;            
            //Wait until the optiga_util_write_data operation is completed
        }
        OPTIGA_SHELL_LOG_MESSAGE("Setting current limitation to maximum...");
        OPTIGA_SHELL_LOG_MESSAGE("Starting OPTIGA example demonstration..\n");
    }while(FALSE);
}

static void optiga_shell_deinit(void)
{
    optiga_lib_status_t return_status = !OPTIGA_LIB_SUCCESS;
    do
    {
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
            return_status = optiga_lib_status;            
            //optiga util close application failed
            break;
        }

        if (NULL != me_util)
        {
            // destroy util and crypt instances
            //lint --e{534} suppress "Error handling is not required so return value is not checked"
            optiga_util_destroy(me_util);
            me_util = NULL;
        }
    }while(FALSE);
}

static void optiga_shell_util_read_data(void)
{
    OPTIGA_SHELL_LOG_MESSAGE("Starting Read Data/Metadata Example");
    OPTIGA_SHELL_LOG_MESSAGE("1 Step: Read Certificate ");
    OPTIGA_SHELL_LOG_MESSAGE("2 Step: Read Certificate Metadata");
    example_optiga_util_read_data();
}

static void optiga_shell_util_write_data(void)
{
    OPTIGA_SHELL_LOG_MESSAGE("Starting Write Data/Metadata Example");
    OPTIGA_SHELL_LOG_MESSAGE("1 Step: Write Sample Certificate in Trust Anchor Data Object ");
    OPTIGA_SHELL_LOG_MESSAGE("2 Step: Write new Metadata");
    example_optiga_util_write_data();
}

static void optiga_shell_util_read_coprocessor_id(void)
{
    OPTIGA_SHELL_LOG_MESSAGE("Starting reading of Coprocessor ID and displaying it's individual components Example");
    OPTIGA_SHELL_LOG_MESSAGE("1 Step: Read Coprocessor UID from OID(0xE0C2) ");
    example_read_coprocessor_id();
}
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION 
static void optiga_shell_pair_host_optiga(void)
{
    OPTIGA_SHELL_LOG_MESSAGE("Starting Pairing of Host and Trust M Example");
    OPTIGA_SHELL_LOG_MESSAGE("1 Step: Read and Check existing Metadata for the Binding Secret");
    OPTIGA_SHELL_LOG_MESSAGE("2 Step: Generate Random for the new Binding Secret");
    OPTIGA_SHELL_LOG_MESSAGE("3 Step: Write new Binding Secret");
    OPTIGA_SHELL_LOG_MESSAGE("4 Step: Store new Binding Secret on the Host");
    example_pair_host_and_optiga_using_pre_shared_secret();
}
#endif
static void optiga_shell_util_hibernate_restore(void)
{
    OPTIGA_SHELL_LOG_MESSAGE("Starting Hibernate and Restore Example");
    OPTIGA_SHELL_LOG_MESSAGE("1 Step: Open Application on the security chip");
    OPTIGA_SHELL_LOG_MESSAGE("2 Step: Pair the host and the security chip");
    OPTIGA_SHELL_LOG_MESSAGE("3 Step: Select Protected I2C Connection");
    OPTIGA_SHELL_LOG_MESSAGE("4 Step: Generate ECC NIST P-256 Key pair and store it in Session Data Object, export the public key");
    OPTIGA_SHELL_LOG_MESSAGE("5 Step: Check Security Event Counter and wait till it reaches 0");
    OPTIGA_SHELL_LOG_MESSAGE("6 Step: Perform Close application with Hibernate parameter set to True");
    OPTIGA_SHELL_LOG_MESSAGE("7 Step: Open Application on the security chip");
    OPTIGA_SHELL_LOG_MESSAGE("8 Step: Sign prepared data with private key stored in Session Data Object");
    OPTIGA_SHELL_LOG_MESSAGE("9 Step: Verify the signature with the public key generated previously");
    OPTIGA_SHELL_LOG_MESSAGE("10 Step: Close Application on the chip");
    OPTIGA_SHELL_LOG_MESSAGE("Important note: To continue with other examples you need to call the init parameter once again");
    example_optiga_util_hibernate_restore();
}

static void optiga_shell_util_update_count(void)
{
    OPTIGA_SHELL_LOG_MESSAGE("Starting Update Counter Example");
    OPTIGA_SHELL_LOG_MESSAGE("1 Step: Write Initial Counter Value");
    OPTIGA_SHELL_LOG_MESSAGE("2 Step: Increase Counter Object");
    example_optiga_util_update_count();
}

static void optiga_shell_util_protected_update(void)
{
    OPTIGA_SHELL_LOG_MESSAGE("Starting Protected Update Example");
    OPTIGA_SHELL_LOG_MESSAGE("1 Step: Update Metadata for the Object to be updated and the Trust Anchor used to verify the update");
    OPTIGA_SHELL_LOG_MESSAGE("2 Step: Write Trust Anchor used by the Trust Charge to verify the update");
    OPTIGA_SHELL_LOG_MESSAGE("3 Step: Start Protected update with prepared manifest and fragments");
    example_optiga_util_protected_update();
}

static void optiga_shell_crypt_hash(void)
{
#ifdef OPTIGA_CRYPT_HASH_ENABLED
    OPTIGA_SHELL_LOG_MESSAGE("Starting Hash Example");
    OPTIGA_SHELL_LOG_MESSAGE("1 Step: Hash given data with Start, Update and Finalize calls");
    example_optiga_crypt_hash();
#else
     OPTIGA_SHELL_LOG_ERROR_MESSAGE("This option is disabled, to use this option enable macro OPTIGA_CRYPT_HASH_ENABLED");
#endif
}

static void optiga_shell_crypt_hash_data(void)
{
#ifdef OPTIGA_CRYPT_HASH_ENABLED
    OPTIGA_SHELL_LOG_MESSAGE("Starting generation of digest Example");
    OPTIGA_SHELL_LOG_MESSAGE("1 Step: Generate hash of given user data ");
    example_optiga_crypt_hash_data();
#else
     OPTIGA_SHELL_LOG_ERROR_MESSAGE("This option is disabled, to use this option enable macro OPTIGA_CRYPT_HASH_ENABLED");
#endif
}


static void optiga_shell_crypt_random(void)
{
    OPTIGA_SHELL_LOG_MESSAGE("Starting Generate Random Example");
    OPTIGA_SHELL_LOG_MESSAGE("1 Step: Generate 32 bytes random");
    example_optiga_crypt_random();
}

static void optiga_shell_crypt_ecc_generate_keypair(void)
{
#ifdef OPTIGA_CRYPT_ECC_GENERATE_KEYPAIR_ENABLED
    OPTIGA_SHELL_LOG_MESSAGE("Starting generate ECC Key Example");
    OPTIGA_SHELL_LOG_MESSAGE("1 Step: Generate ECC NIST P-256 Key Pair and export the public key");
    example_optiga_crypt_ecc_generate_keypair();
#else
     OPTIGA_SHELL_LOG_ERROR_MESSAGE("This option is disabled, to use this option enable macro OPTIGA_CRYPT_ECC_GENERATE_KEYPAIR_ENABLED");
#endif
}

static void optiga_shell_crypt_ecdsa_sign(void)
{
#ifdef OPTIGA_CRYPT_ECDSA_SIGN_ENABLED
    OPTIGA_SHELL_LOG_MESSAGE("Starting signing example for Elliptic-curve Digital Signature Algorithm (ECDSA)");
    OPTIGA_SHELL_LOG_MESSAGE("1 Step: Sign prepared Data and export the signature");
    example_optiga_crypt_ecdsa_sign();
#else
     OPTIGA_SHELL_LOG_ERROR_MESSAGE("This option is disabled, to use this option enable macro OPTIGA_CRYPT_ECDSA_SIGN_ENABLED");
#endif
}

static void optiga_shell_crypt_ecdsa_verify(void)
{
#ifdef OPTIGA_CRYPT_ECDSA_VERIFY_ENABLED
    OPTIGA_SHELL_LOG_MESSAGE("Starting verification example for Elliptic-curve Digital Signature Algorithm (ECDSA)");
    OPTIGA_SHELL_LOG_MESSAGE("1 Step: Verify prepared signature, with prepared public key and digest");
    example_optiga_crypt_ecdsa_verify();
#else
     OPTIGA_SHELL_LOG_ERROR_MESSAGE("This option is disabled, to use this option enable macro OPTIGA_CRYPT_ECDSA_VERIFY_ENABLED");
#endif
}

void print_performance_results(void (*test_case)(void)) 
{
    char buffer_string[30];
    unsigned int timestamp = pal_os_timer_get_time_in_milliseconds(); 
    test_case(); 
    //lint --e{713} suppress "Due to function parameter typecasting is done from unsigned int to int "
    sprintf(buffer_string, "Example takes %d msec", (int) (pal_os_timer_get_time_in_milliseconds() - timestamp));
    OPTIGA_SHELL_LOG_MESSAGE(buffer_string); 
    optiga_lib_print_string_with_newline(""); 
    pal_os_timer_delay_in_milliseconds(2000);
}

static void optiga_shell_selftest(void)
{
    print_performance_results(optiga_shell_init);
    print_performance_results(optiga_shell_util_read_data);
    print_performance_results(optiga_shell_util_write_data);
    print_performance_results(optiga_shell_util_read_coprocessor_id);
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION     
    print_performance_results(optiga_shell_pair_host_optiga);
#endif    
    print_performance_results(optiga_shell_util_update_count);
    print_performance_results(optiga_shell_util_protected_update);

    print_performance_results(optiga_shell_crypt_hash);
    print_performance_results(optiga_shell_crypt_hash_data);
    print_performance_results(optiga_shell_crypt_random);

    print_performance_results(optiga_shell_crypt_ecc_generate_keypair);
    print_performance_results(optiga_shell_crypt_ecdsa_sign);
    print_performance_results(optiga_shell_crypt_ecdsa_verify);
    print_performance_results(optiga_shell_deinit);
}


static void optiga_shell_show_usage(void);


optiga_example_cmd_t optiga_cmds [] =
{
        {"",                                            "help",            optiga_shell_show_usage},
        {"    initialize optiga                        : optiga --","init",            optiga_shell_init},
        {"    de-initialize optiga                     : optiga --","deinit",          optiga_shell_deinit},
        {"    run all tests at once                    : optiga --","selftest",        optiga_shell_selftest},
        {"    read data                                : optiga --","readdata",        optiga_shell_util_read_data},
        {"    write data                               : optiga --","writedata",       optiga_shell_util_write_data},
        {"    read coprocessor id                      : optiga --","coprocid",        optiga_shell_util_read_coprocessor_id},
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION        
        {"    binding host with optiga                 : optiga --","bind",            optiga_shell_pair_host_optiga},
#endif        
        {"    hibernate and restore                    : optiga --","hibernate",       optiga_shell_util_hibernate_restore},
        {"    update counter                           : optiga --","counter",         optiga_shell_util_update_count},
        {"    protected update                         : optiga --","protected",       optiga_shell_util_protected_update},

        {"    hashing of data                          : optiga --","hash",            optiga_shell_crypt_hash},
        {"    hash single function                     : optiga --","hashsha256",      optiga_shell_crypt_hash_data},
        {"    random number generation                 : optiga --","random",          optiga_shell_crypt_random},

        {"    ecc key pair generation                  : optiga --","ecckeygen",       optiga_shell_crypt_ecc_generate_keypair},
        {"    ecdsa sign                               : optiga --","ecdsasign",       optiga_shell_crypt_ecdsa_sign},
        {"    ecdsa verify sign                        : optiga --","ecdsaverify",     optiga_shell_crypt_ecdsa_verify},
};

#define OPTIGA_SIZE_OF_CMDS            (sizeof(optiga_cmds)/sizeof(optiga_example_cmd_t))

static void optiga_shell_show_usage()
{
    uint8_t number_of_cmds = OPTIGA_SIZE_OF_CMDS;
    uint8_t index;
    optiga_example_cmd_t * current_cmd;
    optiga_lib_print_string_with_newline("");
    optiga_lib_print_string_with_newline("    usage                : optiga --<cmd>");
    for(index = 0; index < number_of_cmds; index++)
    {
        current_cmd = &optiga_cmds[index];
        if(0 != strcmp("help",current_cmd->cmd_options))
        {
            optiga_lib_print_string(current_cmd->cmd_description);
            optiga_lib_print_string_with_newline(current_cmd->cmd_options);
        }
    }
}

static void optiga_shell_trim_cmd(char_t * user_cmd)
{
    char_t* i = user_cmd;
    char_t* j = user_cmd;
    while(*j != 0)
    {
        *i = *j++;
        if(*i != ' ')
            i++;
    }
    *i = 0;
    if(strlen(user_cmd)>strlen("optiga --"))
    {
        strcpy(user_cmd,user_cmd+strlen("optiga --")-1);
    }
}

static void optiga_shell_execute_example(char_t * user_cmd)
{
    uint8_t number_of_cmds = OPTIGA_SIZE_OF_CMDS;
    uint8_t index,cmd_found = 0;
    char_t * optiga_cmd_option = "optiga --";
    optiga_example_cmd_t * current_cmd;

    
    do
    {
        if (0 != strncmp(user_cmd,optiga_cmd_option,9))
        {
            break;
        }
        optiga_shell_trim_cmd(user_cmd);
        for(index = 0; index < number_of_cmds; index++)
        {
            current_cmd = &optiga_cmds[index];
            if(0 == strcmp(user_cmd,current_cmd->cmd_options))
            {
                if(NULL != current_cmd->cmd_handler)
                {
                    unsigned int timestamp = pal_os_timer_get_time_in_milliseconds();
                    current_cmd->cmd_handler();
                    char buffer_string[30];
                    //lint --e{713,705,737} suppress "Due to function parameter typecasting is done from unsigned int to int "
                    sprintf(buffer_string, "Example takes %d msec", (int) pal_os_timer_get_time_in_milliseconds() - timestamp);
                    OPTIGA_EXAMPLE_LOG_MESSAGE(buffer_string);
                    optiga_lib_print_string_with_newline("");
                    cmd_found = 1;
                    break;
                }
                else
                {
                    optiga_lib_print_string_with_newline("No example exists for this request");
                    break;
                }
            }
        }
    } while (FALSE);
    if(!cmd_found)
    {
        optiga_lib_print_string_with_newline("");
        optiga_lib_print_string_with_newline("No example exists for this request chose below options");
        optiga_shell_show_usage();
    }

}

static void optiga_shell_show_prompt()
{
    optiga_lib_print_string("$");
}

void optiga_shell_begin(void)
{
    uint8_t ch = 0;
    char_t user_cmd[50];
    uint8_t index = 0;

    optiga_shell_show_prompt();
    optiga_shell_show_usage();
    optiga_shell_show_prompt();

    //lint --e{716} Suppress the infinite loop
    while(TRUE)
    {
        if (0 == pal_logger_read(&logger_console,&ch,1))
        {

            if(ch == 0x0d || ch == 0x0a)
            {
                user_cmd[index++] = 0;
                index = 0;
                optiga_lib_print_string_with_newline("");
                //start cmd parsing
                optiga_shell_execute_example((char_t * )user_cmd);
                optiga_shell_show_prompt();
            }
            else
            {
                //keep adding
                //lint --e{534,713} The return value is not used hence not checked*/
                pal_logger_write(&logger_console, &ch, 1);
                user_cmd[index++] = ch;
            }
        }
    }
}

void optiga_shell_wait_for_user(void)
{
    uint16_t bytes = 0;
    uint8_t ch = 0;
    //lint --e{716} Suppress the infinite loop
    while(TRUE)
    {
        bytes = USBD_VCOM_BytesReceived();
        if (bytes)
        {
            //lint --e{534} The return value is not used hence not checked*/
            pal_logger_read(&logger_console,&ch,1);
            break;
        }
        else
        {
            optiga_lib_print_string_with_newline("Press any key to start optiga mini shell");
            pal_os_timer_delay_in_milliseconds(2000);
        }
        bytes = 0;
        CDC_Device_USBTask(&USBD_VCOM_cdc_interface);
    }
}
