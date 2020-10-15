# Which document do I need?

* **[Infineon_I2C_Protocol_v2.02.pdf](https://github.com/Infineon/optiga-trust-charge/raw/master/documents/Infineon_I2C_Protocol_v2.02.pdf)** - Infineon I2C protocol specification. In this document you can find all information related to the communication protocol between a Host MCU and the Secure Element, for instance how shielded connection works.
* **[License.pdf](https://github.com/Infineon/optiga-trust-charge/raw/master/documents/License.pdf)** - MIT License
* **[OPTIGA_Trust_Charge_V1_Datasheet_v1.30.pdf](https://github.com/Infineon/optiga-trust-charge/raw/master/documents/OPTIGA_Trust_Charge_V1_Datasheet_v1.30.pdf)** - Datasheet for the OPTIGA Trust M revision 1 product. This is a high level description of the hardware side of the product. You can find there the following information, though not limited to
    * Electrical Characteristics
    * Crypto Performance
    * Hardware integration guide including a reference schematics
    * External Interface which the security chip exposes to the host (not the host library)
    * Test Vectors for the security chip (on I2C level)
    * Compliance infomration
* **[OPTIGA_Trust_Charge_V1_Host_Library_Documentation.chm](https://github.com/Infineon/optiga-trust-charge/raw/master/documents/OPTIGA_Trust_Charge_V1_Host_Library_Documentation.chm)** - A Windows Help file with the API description
* **[OPTIGA_Trust_Charge_V1_Release_Notes_v1.30.pdf](https://github.com/Infineon/optiga-trust-charge/raw/master/documents/OPTIGA_Trust_Charge_V1_Release_Notes_v1.30.pdf)** - A history of changes for all the releases the product had
* **[OPTIGA_Trust_Charge_V1_Solution_Reference_Manual_v1.00.pdf](https://github.com/Infineon/optiga-trust-charge/raw/master/documents/OPTIGA_Trust_Charge_V1_Solution_Reference_Manual_v1.00.pdf)** - A document which describes in more details:
    * Features and properties of the security chip in more details
    * Use cases
    * Application areas
    * External and host side API
    * And many more other details related to the product and its application

# Other helping information

* [Crypo Performance](Crypo_Performance.md)
* [Code Footprint](Code_Footprint.md)
* API Description:
  * [Crypt API](API_Crypt.md)
  * [Util API](API_Util.md)
* [Protected Update for Data and Certificate Objects](Protected_Update_for_Data_Objects.md)
* [Data format examples](https://github.com/Infineon/optiga-trust-charge/blob/master/documents/Data_format_examples.md). How a signature or a public key accepted by the chip look like.
* [Error Codes](Error_Codes.md). If you see an error returnd from the host library, it might be one of the mentioned in this document.
