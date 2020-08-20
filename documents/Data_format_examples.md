# Data format examples

## Table of content

* [ECC Public Key](#ECC-Public-Key)
* [ECC Private Key](#ECC-Private-Key)
* [ECDSA Signature](#ECDSA-Signature)

## ECC Public Key

The examples for the format of ECC Public Key (referred by Generate Key Pair, Verify signature and ECDH operations) are given below.

* Example for ECC NIST P-256 Public Key [Values are in hex]

```
    // Bit String tag
    03
        // Length of Bit string tag
        42
            // Unused bits
            00
            // Compression format. Supports only 04 [uncompressed]
            04
            // Public Key (e.g. ECC NIST P-256, 0x40 Bytes)
            // Qx - 0x20 Bytes
            8B 88 9C 1D D6 07 58 2E D6 F8 2C C2 D9 BE D0 FE
            6D F3 24 5E 94 7D 54 CD 20 DC 58 98 CF 51 31 44
            // Qy - 0x20 Bytes
            22 EA 01 D4 0B 23 B2 45 7C 42 DF 3C FB 0D 33 10
            B8 49 B7 AA 0A 85 DE E7 6A F1 AC 31 31 1E 8C 4B
```

* Example for ECC NIST P-384 Public Key [Values are in hex]

```
    // Bit String tag
    03
        // Length of Bit string tag
        62
            // Unused bits
            00
            // Compression format. Supports only 04 [uncompressed]
            04
            // Public Key (e.g. ECC NIST P-384, 0x60 Bytes)
            // Qx - 0x30 Bytes
            1F 94 EB 6F 43 9A 38 06 F8 05 4D D7 91 24 84 7D
            13 8D 14 D4 F5 2B AC 93 B0 42 F2 EE 3C DB 7D C9
            E0 99 25 C2 A5 FE E7 0D 4C E0 8C 61 E3 B1 91 60
            // Qy - 0x30 Bytes
            1C 4F D1 11 F6 E3 33 03 06 94 21 DE B3 1E 87 31
            26 BE 35 EE B4 36 FE 20 34 85 6A 3E D1 E8 97 F2
            6C 84 6E E3 23 3C D1 62 40 98 9A 79 90 C1 9D 8C
```


## ECC Private Key

The examples for format of ECC Private Key exported as part of Generate Key Pair are given below.

* Example for ECC NIST P-256 Private Key [Values are in hex]

```
    // DER OCTET String Tag (Private Key)
    04
        // Length of Tag
        20
            // Private Key
            20 DC 58 98 CF 51 31 44 22 EA 01 D4 0B 23 B2 45
            7C 42 DF 3C FB 0D 33 10 B8 49 B7 AA 0A 85 DE E7
```

* Example for ECC NIST P-384 Private Key [Values are in hex]

```
    // DER OCTET String Tag (Private Key)
    04
        // Length of Tag
        30
            // Private Key
            53 94 F7 97 3E A8 68 C5 2B F3 FF 8D 8C EE B4 DB
            90 A6 83 65 3B 12 48 5D 5F 62 7C 3C E5 AB D8 97
            8F C9 67 3D 14 A7 1D 92 57 47 93 16 62 49 3C 37
```

## ECDSA Signature

The examples for format of ECDSA Signature (referred by Signature generation and verification operations) are given below.

* Example for signature in case of ECC NIST P-256 key [Values are in hex]

```
    // Integer tag for R component
    02
        // Length
        20
            // R Component
            2B 82 6F 5D 44 E2 D0 B6 DE 53 1A D9 6B 51 E8 F0
            C5 6F DF EA D3 C2 36 89 2E 4D 84 EA CF C3 B7 5C
    // Integer tag for S component
    02
        // Length
        21
            // S Component
            00
            A2 24 8B 62 C0 3D B3 5A 7C D6 3E 8A 12 0A 35 21
            A8 9D 3D 2F 61 FF 99 03 5A 21 48 AE 32 E3 A2 48
```

* Example for signature in case of ECC NIST P-384 key [Values are in hex]

```
    //Integer tag for R component
    02
        // Length
        31
            //R Component
            00
            C3 6E 5F 0D 3D E7 14 11 E6 E5 19 F6 3E 0F 56 CF
            F4 32 33 0A 04 FE FE F2 99 3F DB 56 34 3E 49 F2
            F7 DB 5F CA B7 72 8A CC 1E 33 D4 69 25 53 C0 2E
    //Integer tag for S component
    02
        // Length
        30
            //S Component
            0D 40 64 39 9D 58 CD 77 1A B9 42 0D 43 87 57 F5
            93 6C 38 08 E9 70 81 E4 57 BC 86 2A 0C 90 52 95
            DC A6 0E E9 4F 45 37 59 1C 6C 7D 21 74 53 90 9B
```

Notes:

* The size of R and S components is based on the key size chosen. (e.g. in case of ECC NIST P256, size is 32 bytes and for ECC NIST P384, size is 48 bytes)
* If the first byte of R or S component is greater than 0x7F (negative integer), then the respective component gets prepended with 0x00.
* The caller must consider the length of buffer for signature accordingly considering the additional encoding information.
