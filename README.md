# OPTIGA&trade; Trust Charge Software Framework

## Quick navigation

<a href="https://github.com/Infineon/optiga-trust-charge#get-started"><img src="https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_x_gitrepo_tile_3.jpg" width="192" height="144"></a>
<a href="https://github.com/Infineon/optiga-trust-charge#documentation"><img src="https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_x_gitrepo_tile_4.jpg" width="192" height="144"></a> <a href="https://github.com/Infineon/optiga-trust-charge/wiki/Crypto-Performance"><img src="https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_x_gitrepo_tile_5.jpg" width="192" height="144"></a>  <a href="https://github.com/Infineon/optiga-trust-charge/wiki/Trust-Qi-Crypt-API"><img src="https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_x_gitrepo_tile_6.jpg" width="192" height="144"></a>

## Description

The OPTIGA™ Trust Charge is an **authentication** security solution based on a secure microntroller. Each device is shipped with a unique elliptic-curve keypair and a corresponding WPC Qi Authentication certificate chain.

The main purpose of this software framework is to make an integration of the secure element into WPC Qi 1.3 Authentication Protocol implementation easier.
The figure below depicts what this solution offers, and what other components of the wireless charging system are required.

![](https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_charge_integration_overview.png)

## Key Features and Benefits

* High-end security controller
* **Wireless Power Consortium(WPC) Qi Authentication**
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
* Hibernate for zero power consumption1
* Lifetime for Industrial Automation and Infrastructure is 20 years and 15 years for other Application Profiles

## Get Started

First steps can include the following:
1. Getting Started Guide for OPTIGA™ Trust Charge. Use one of [OPTIGA™ Trust Charge Evaluation Kits (link is pending)]() and this [guidance](https://github.com/Infineon/optiga-trust-charge/raw/master/documents/OPTIGA_Trust_Charge_V1_Getting_Started_Guide_v1.30.pdf) for this
2. Run test vectors for the WPC Qi 1.3 Authentication Protocol. You can take the same Evalation Kit, and the dedicated [Application Note](https://github.com/Infineon/wpcqi-optiga-trust-charge)
3. Exercise the [Porting Guide](https://github.com/Infineon/optiga-trust-charge/tree/master/pal) to bring this Software Framework to your target Wireless Charging System
4. Follow a defined process to get unique samples. Please contact for this either your distribution channel or directly Infineon.


### Examples

* [Getting Started Guide (Wiki)]()

### Software Framework overview

### Evaluation and developement kits

External links, open in the same tab.
* [OPTIGA™ Trust Charge Evaluation Kit (Link Pending)]()

## Documentation

For high level description and some important excerpts from the documentation please refer to [Wiki page](https://github.com/Infineon/optiga-trust-charge/wiki)

Other downloadable PDF documents can be found below:
1. [OPTIGA™ Trust Charge Datasheet v1.30](https://github.com/Infineon/optiga-trust-charge/raw/master/documents/OPTIGA_Trust_Charge_V1_Datasheet_v1.30.pdf) (PDF)
2. [OPTIGA™ Trust Charge Solution Reference Manual v1.00](https://github.com/Infineon/optiga-trust-charge/raw/master/documents/OPTIGA_Trust_Charge_V1_Solution_Reference_Manual_v1.00.pdf) (PDF)
4. [Infineon I2C protocol specification v2.02](https://github.com/Infineon/optiga-trust-charge/raw/master/documents/Infineon_I2C_Protocol_v2.02.pdf) (PDF)

### Board assembly recommendations

If you are planning to integrate OPTIGA™ Trust Charge in your PCB design have a look at the recommendations found [here (external, opens in the same tab)](https://www.infineon.com/dgdl/Infineon-Recommendations_for_Board_Assembly_xQFN-P-v01_00-EN.pdf?fileId=5546d462580663ef015806ab383a05bf).

## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details
