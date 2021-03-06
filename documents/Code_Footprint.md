## Setup

Final code footprint depends on many factors, such as target compiler, compilation flags; e.g. with or without code optimization, used applications, third-party crypto library to support the shielded communication, usage of the shielded communication itself, or I<sup>2</sup>C frame buffer size. 
For all measurements optimization level "-Os" has been selected. The setup without optimization might take up to 100% of the showed here code footprint

## Results

<table class="tg">
  <tr>
    <th class="tg-fymr">Generic module</th>
    <th class="tg-fymr">FLASH/RAM in kB</th>
    <th class="tg-fymr">Part of the module</th>
    <th class="tg-fymr">FLASH in kB</th>
    <th class="tg-fymr">RAM<sup>1</sup> in kB</th>
  </tr>
  <tr>
    <td class="tg-c3ow" rowspan="4">OPTIGA™ Trust Charge hostlib</td>
    <td class="tg-0pky" rowspan="4">min: 13.4/2.9 </br> max: 25.6/11.5 <sup>2</sup></td>
    <td class="tg-0pky">Core Functions</td>
    <td class="tg-0pky">8.4</td>
    <td class="tg-0pky" rowspan="2">2.9</td>
  </tr>
  <tr>
    <td class="tg-0pky">Infineon I2C protocol</td>
    <td class="tg-0pky">3.7</td>
  </tr>
  <tr>
    <td class="tg-0pky">WPC Qi AppNote </td>
    <td class="tg-0pky">1.3</td>
  <td class="tg-0pky">~0</td>
  </tr>
  <tr>
    <td class="tg-0pky">(optional) Shielded Connection + 3rd party crypto</td>
    <td class="tg-0pky">4 + 8.2</td>
    <td class="tg-0pky">8.6</td>
  </tr>
</table>

<sup>1</sup> Compile time memory allocation (.bss and .data sections, no heap included if it's used)

<sup>2</sup> Values include all features and mbed TLS as the third-party crypto provider

## How to optimize

Decrease ammount of maximum context (for usage in parallel). Find it [here](https://github.com/Infineon/optiga-trust-charge/blob/36b24764f328c6782caa8ecd6ddefe8b6f5c667c/optiga/include/optiga/optiga_lib_config.h#L65)
```c
#define OPTIGA_CMD_MAX_REGISTRATIONS                (0x06)
```

Decrese maxmum communicaiton buffer size. Find it [here](https://github.com/Infineon/optiga-trust-charge/blob/36b24764f328c6782caa8ecd6ddefe8b6f5c667c/optiga/include/optiga/optiga_lib_config.h#L67)
```c
#define OPTIGA_MAX_COMMS_BUFFER_SIZE                (0x615) //1557 in decimal
```

The size (-Os) optimisation

Also...

## Shielded Communication

The [OPTIGA_COMMS_SHIELDED_CONNECTION](https://github.com/Infineon/optiga-trust-charge/blob/master/optiga/include/optiga/optiga_lib_config.h#L96) macro enables or disables the shielded communication. Don't forget to excluded the pal_crypt provider; e.g. [pal_crypt_mbedtls.c](https://github.com/Infineon/optiga-trust-charge/blob/master/pal/pal_crypt_mbedtls.c), from your build together with the crypto library; e.g.[mbedtls](https://github.com/Infineon/optiga-trust-m/tree/master/externals/mbedtls), if it's not used in your project.

## ECC Related Functions

* [OPTIGA_CRYPT_ECC_GENERATE_KEYPAIR_ENABLED](https://github.com/Infineon/optiga-trust-charge/blob/master/optiga/include/optiga/optiga_lib_config.h#L54) OPTIGA CRYPT ECC generate keypair feature enable/disable macro 
* [OPTIGA_CRYPT_ECDSA_SIGN_ENABLED](https://github.com/Infineon/optiga-trust-charge/blob/master/optiga/include/optiga/optiga_lib_config.h#L56) OPTIGA CRYPT ECDSA signature feature enable/disable macro
* [OPTIGA_CRYPT_ECDSA_VERIFY_ENABLED](https://github.com/Infineon/optiga-trust-charge/blob/master/optiga/include/optiga/optiga_lib_config.h#L58) OPTIGA CRYPT verify ECDSA signature feature enable/disable macro

## Other Functions

* [OPTIGA_CRYPT_RANDOM_ENABLED](https://github.com/Infineon/optiga-trust-charge/blob/master/optiga/include/optiga/optiga_lib_config.h#L50)  OPTIGA CRYPT random number generation feature enable/disable macro 
* [OPTIGA_CRYPT_HASH_ENABLED](https://github.com/Infineon/optiga-trust-charge/blob/master/optiga/include/optiga/optiga_lib_config.h#L52)  OPTIGA CRYPT hash feature enable/disable macro
