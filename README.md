# OPTIGA&trade; Trust Charge Software Framework

## Quick navigation

<a href="https://github.com/Infineon/optiga-trust-charge#get-started"><img src="https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_x_gitrepo_tile_3.jpg" width="192" height="144"></a>
<a href="https://github.com/Infineon/optiga-trust-charge#documentation"><img src="https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_x_gitrepo_tile_4.jpg" width="192" height="144"></a> <a href="https://github.com/Infineon/optiga-trust-charge#crypto-performance"><img src="https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_x_gitrepo_tile_5.jpg" width="192" height="144"></a>  <a href="https://github.com/Infineon/optiga-trust-charge/blob/master/documents/API_Crypt.md"><img src="https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_x_gitrepo_tile_6.jpg" width="192" height="144"></a>

## Description

Infineon’s OPTIGA™ Trust Charge is a turnkey solution providing secured device authentication for inductive wireless charging.

The main purpose of this software framework is to support the integration of the OPTIGA™ Trust Charge solution into a target wireless charging system. Be aware that this software comes without any warranty claims and shall be used for evaluation purposes.

The figure below shows the offering of this solution and other third party components needed to build a wireless charging system. 

![](https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_charge_integration_overview.png)

## Key Features and Benefits

* High-end security controller
* **Wireless Power Consortium(WPC) Authentication**
* Common Criteria Certified EAL6+ (high) hardware
* Turnkey solution
* Compliant with the USB Type-C™ Authentication standard
* Up to 10kB user memory
* PG-USON-10-2,-4 package (3 x 3 mm)
* Standard & Extended temperature ranges
* I2C interface with Shielded Connection (encrypted communication)
* Cryptographic support: ECC NIST P256/P384, SHA-256, TRNG, DRNG
* Crypto ToolBox commands for SHA-256, ECC NIST P256/P384 (sign, verify and key generation)
* Device Security Monitor
* 4 Monotonic up counters
* Protected (integrity) update of data objects
* Hibernate for zero power consumption
* Lifetime for Industrial Automation and Infrastructure is 20 years and 15 years for other Application Profiles

## Getting Started

The following steps will support in familiarizing with this software framework and adapting it to your target system:
1. Get in touch with this software framework by going through the [Getting Started Guide for OPTIGA™ Trust Charge](https://github.com/Infineon/optiga-trust-charge/raw/master/documents/OPTIGA_Trust_Charge_V1_Getting_Started_Guide_v1.30.pdf) while using one of the [OPTIGA™ Trust Charge Evaluation Kits](https://www.infineon.com/cms/en/product/evaluation-boards/optiga-trust-ch-eval-kit/).
2. Follow the [Porting Guide](https://github.com/Infineon/optiga-trust-charge/tree/master/pal) to bring this software framework to your target Wireless Charging System.
3. Also have a look at our [WPC Qi 1.3 Application Note](https://github.com/Infineon/wpcqi-optiga-trust-charge).
4. To order engineering samples with your unique configuration, please contact your distribution partner or Infineon representative.

## Documentation

For a high level description and some important excerpts from the documentation please refer to [documents](documents)

The most frequently used documents in PDF can be found below:
1. [OPTIGA™ Trust Charge Datasheet v1.30](https://github.com/Infineon/optiga-trust-charge/raw/master/documents/OPTIGA_Trust_Charge_V1_Datasheet_v1.30.pdf) (PDF)
2. [OPTIGA™ Trust Charge Solution Reference Manual v1.00](https://github.com/Infineon/optiga-trust-charge/raw/master/documents/OPTIGA_Trust_Charge_V1_Solution_Reference_Manual_v1.00.pdf) (PDF)
4. [Infineon I2C protocol specification v2.02](https://github.com/Infineon/optiga-trust-charge/raw/master/documents/Infineon_I2C_Protocol_v2.02.pdf) (PDF)

## Crypto Performance

I2C Fast Mode (400KHz)
* Without power limitation
* @ 25°C
* VCC = 3.3V
* ECDSA Signature scheme: ECDSA FIPS 186-3 without hashing
* Hash scheme: SHA256
* ECC Key size: 256 bits (NIST P-256)

<table class="tg">
  <tr>
    <th class="tg-fymr">Scheme</th>
    <th class="tg-fymr">Algorithm</th>
    <th class="tg-fymr">Performance in ms<sup>1</sup></th>
    <th class="tg-fymr">Performance with Shielded Connection in ms<sup>1</sup></th>
    <th class="tg-fymr">Notes</th>
  </tr>
  <tr>
    <td class="tg-c3ow">Calcualte Signature</td>
    <td class="tg-0pky">ECDSA</td>
    <td class="tg-0pky">~60</td>
    <td class="tg-0pky">~65</td>
    <td class="tg-0pky" rowspan="2">Doesn't include message hashing before calling a toolbox funciton</td>
  </tr>
  <tr>
    <td class="tg-c3ow">Verify signature</td>
    <td class="tg-0pky">ECDSA</td>
    <td class="tg-0pky">~85</td>
    <td class="tg-0pky">~90</td>
  </tr>
  <tr>
    <td class="tg-c3ow">Key pair generation </td>
    <td class="tg-0pky">ECC</td>
    <td class="tg-0pky">~75</td>
    <td class="tg-0pky">~80</td>
    <td class="tg-0pky">Generate 256 bit ECC key pair</td>
  </tr>
  <tr>
    <td class="tg-c3ow">Hash calculation</td>
    <td class="tg-0pky">SHA256</td>
    <td class="tg-0pky">~5 Kbyte/s</td>
    <td class="tg-0pky">~4.5 Kbyte/s</td>
    <td class="tg-0pky">In blocks of 500 bytes</td>
  </tr>
  <tr>
    <td class="tg-c3ow">Write Data</td>
    <td class="tg-0pky">N/A</td>
    <td class="tg-0pky">18</td>
    <td class="tg-0pky">-</td>
    <td class="tg-0pky">100 bytes</td>
  </tr>
  <tr>
    <td class="tg-c3ow">Read Data</td>
    <td class="tg-0pky">N/A</td>
    <td class="tg-0pky">9</td>
    <td class="tg-0pky">-</td>
    <td class="tg-0pky">100 bytes</td>
  </tr>
</table>

<sup>1</sup> Minimum Execution of the entire sequence in milliseconds, except the External World timings

## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 3rd party software

This project uses the following projects:
* mbedTLS - An open source, portable, easy to use, readable and flexible TLS library
* LUFA - Lightweight USB Framework for AVRs
* CMSIS MPU API for Armv7-M MPU
* STROMX - A library for near real time processing of images with a focus on industrial applications

For more information on the relevant licenses, please see [LICENSE-3RD-PARTY](LICENSE-3RD-PARTY).