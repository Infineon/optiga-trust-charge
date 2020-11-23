# Protected Update for Data and Certificate Objects

## Content

* [Description](#Description)
* [Use case: Integrity Protected Update of Data Object ](#Use-case-Integrity-Protected-Update-of-Data-Object)
* [Manifest and Fragements](#Manifest-and-Fragements)
* [Manifest and Signature structures](#Manifest-and-Signature-structures)
  * [Manifest structures](#Manifest-structures)
  * [Signature structures](#Signature-structures)

## Description

This feature allows to make an integrity protected update of any Data Object (not Key Objects) on the OPTIGA™ Trust M rev. 1 except listed below:
* Life Cycle Status (Global/Application)
* Security Status (Global/Application)
* Coprocessor UID
* Sleep mode activation delay
* Current Limitation
* Security Event Counter
* Last Error Code
* Maximum Com Buffer Size

The User is expected to provide a CBOR encoded manifest along with the transmitted data for the update.
In layman terms the update works like following:
1. The Trust M chip has a public key CA certificate (ECC or RSA) of an Update Server stored in one of Trust Anchors
1. The Updated Server prepares the data; e.g. WiFi credentials
1. The Update Server prepares a **manifest** using a [protected_update_data_set](https://github.com/Infineon/optiga-trust-m/tree/master/examples/tools/protected_update_data_set) tool 
  * This Manifest may look like following
  <details> <summary> Sample Manifest </summary>  
  
```c
/**
 * Manifest for ECC-256
 */
static uint8_t manifest[] = 
{
    // COSE Sign1 Trust
    0x84,
        // Protected signed header trust
        0x43,
            // Trust Sign Alogorithm
            0xA1, 
            // ECDSA
            0x01, 0x26, 
        // Unprotected signed header trust
        0xA1, 
            // Root of trust
            0x04, 0x42, 
            // Trust Anchor OID
            0xE0, 0xE3, 
        // Payload info Byte string of single byte length
        0x58, 
            // Byte string length of manifest            
            0x3D, 
            // Trust manifest, array of 6 entries
            0x86, 
            // Version(Major Type 0)
            0x01,
            // NULL
            0xF6,
            // NULL
            0xF6,
            // Resources, array of 4 entries
            0x84, 
                // Trust Payload Type
                0x20,
                // 2 byte length
                0x19,
                // Payload Length
                0x02, 0x92, 
                // Trust Payload version
                0x03, 
                // Trust Add info data 
                0x82, 
                    // Offset( Major Type 0)
                    0x00, 
                    // Trust add info write type
                    0x01, 
                // Trust Processors, array of 2 entries 
                0x82, 
                    // Processing step integrity, array of 2 entries
                    0x82, 
                        // Process( Major Type 1)
                        0x20, 
                        // Parameters, byte string with single byte length
                        0x58, 
                        // Byte string length 
                        0x25, 
                        // IFX Digest info, array of 2 entries
                        0x82, 
                        // Digest Algorithm
                        0x18,
                        // SHA-256
                        0x29, 
                        // Digest
                        0x58, 
                        // Byte string length
                        0x20, 
                        // Digest data
                        0xA0, 0xAE, 0xD2, 0x75, 0x75, 0xB8, 0x77, 0xED, 
                        0x0F, 0xEA, 0xB6, 0x3C, 0x74, 0x35, 0x58, 0xEA,
                        0xE3, 0xA2, 0x26, 0x4C, 0x8C, 0xEC, 0xD5, 0x8F,
                        0x8F, 0x4E, 0x12, 0xAD, 0xA0, 0xDB, 0x73, 0x9A, 
                    // NULL
                    0xF6, 
                // Trsust Target
                0x82, 
                    // Component identifier
                    0x40, 
                    // Storage identifier
                    0x42, 
                    // Optiga target OID
                    0xE0, 0xE1, 
        // Signature info, byte string of single byte length
        0x58, 
            // Byte string length for ECC 256
            0x40, 
            // Signature data
            0x8B, 0x87, 0xAE, 0x23, 0x11, 0x4D, 0x44, 0xC4, 0xE8, 0x93, 0xFA, 0x70, 0x99, 0xD0, 0x32, 0xFE,
            0x70, 0x9D, 0xF9, 0x7C, 0x81, 0x98, 0x05, 0x73, 0xA9, 0x61, 0x8A, 0x3D, 0xD7, 0xCE, 0x8B, 0xA4,
            0xC8, 0xC2, 0x70, 0x19, 0x8E, 0x74, 0xE8, 0x58, 0xDC, 0x22, 0x63, 0x9E, 0x38, 0x52, 0x8C, 0x7D,
            0x95, 0xE2, 0x5E, 0x28, 0xC7, 0x71, 0xED, 0xDF, 0xFE, 0x79, 0xC4, 0x62, 0x77, 0xB8, 0xC6, 0x5C                     

};
  ```

</details>

4. As part of the manifest creation the Update Server needs to sign the manifest with the private key of the CA certificate. The same certificate is located on the Trust M (see step 1)
5. The Update Server sends this manifest along with data to the Host MCU (to which the Secure Element is attached)
6. (optional) The Host MCU parses the received data; e.g. by using this [reference code](https://github.com/Infineon/optiga-manifest-parser) written in C to extract required service information
7. The Host MCU initiates the Protected Update. This example demonstrate which functions should be called in order to successfully complete it can be found [here](https://github.com/Infineon/optiga-trust-m/blob/master/examples/optiga/example_optiga_util_protected_update.c)

## Use case: Integrity Protected Update of Data Object 

The Management Server intends to update a data object (e.g. a Trust Anchor) with integrity protected. The Management Server provides an update data set, which gets forwarded to the OPTIGA™. The OPTIGA™ checks and removes the protection and upon success updates the target data object. 
**Pre-condition(s):**
* The OPTIGA™ application is already launched 
* The Trust Anchor for management purpose is well formatted and available at the OPTIGA™. 
* The access conditions of the target data object allow protected updating. 

**Post-condition:**
* The target data object is updated.  

![](https://github.com/Infineon/Assets/blob/master/Pictures/optiga_trust_m_usecase_protected_update.jpg)

## Manifest and Fragements

This section provides the definition and some useful information of update data sets for data objects which are used to update those in a integrity protected way.
The figure below shows the high level structure of the update data set for data objects. It consists of a manifest and the connected binary data. The coding of the structure is according to CBOR as specified below. It provides more detailed information regarding the format of manifest consumed.
The Manifest is a top level construct that ties all other structures together and is signed by an authorized entity whose identity is represented by a trust anchor installed at the OPTIGA™. The trust anchor is addressed by its unique ID (OID), which is contained in the metadata of the manifest. Manifest consists of the metadata in plain text, the payload binding and the signature over metadata and payload binding.
The Metadata provide information enabling interpretation and manage the update data set by the OPTIGA™. It contains
* Version number
* Unique identifier (OID) of the trust anchor to be used for verifying the metadata signature.
* Unique identifier (OID) of the object to be updated
* Cipher suite specifying all cryptographic algorithms (signature, hash, ...) used during executing the update.
* Offset within the target object and length of the object data.

The Integrity Protection of the object data is based on the hash value of the first block of object data which is protected by the successful verification of the signature over the metadata and fragment binding hash value. Each block of object data, except the last block , carries the fragment binding hash value of the next block of object data.

![](https://github.com/Infineon/Assets/raw/master/Pictures/optiga_trust_m_manifest_fragements.png)

## Manifest and Signature structures
This section provides the CDDL coding considered for manifest and signature structures for the protected update feature.

### Manifest structures

```unformatted
; OPTIGA(TM) Trust (Family Trust X, Trust M...) Manifest (v1)
;
; This file contains a CDDL (CBOR Data Definition Language) definition for a Manifest used within
OPTIGA(TM) Trust Protected update feature.
; It describes the serialization format of a manifest as CBOR.
;
; The manifest contains metadata about an OPTIGA(TM) Trust data/key object image (Payload Resource)
and to which target object it applies.
; The manifest itself is protected via a COSE Signature Container.
; Optionally the data object image is encrypted via a COSE Encryption Container.
;
; The Processing Steps how to apply the Payload Resource to the Target are described as separate
Processing Array.
;
; Naming Conventions:
; (1) Existing CDDL Definitions profiled for OPTIGA(TM) Trust are enhanced
; at the END with _Trust
; e.g. COSE_Sign1_Trust
; (2) Definitions specific for OPTIGA(TM) Trust are defined with Trust_ in the
; beginning, the same ways as done in COSE (where COSE_ is at the beginning)
; e.g. Trust_PayloadVersion
; (3) Defintion used by OPTIGA(TM) Trust and OPTIGA(TM) TPM are defined with
; IFX_ in the beginning
; e.g. IFX_DigestSize
;
; Links:
; [CBOR]: https://tools.ietf.org/html/rfc7049
; [CDDL]: https://tools.ietf.org/html/draft-ietf-cbor-cddl-05
; [COSE]: https://tools.ietf.org/html/rfc8152
; [SUIT_DRAFTv2]: https://tools.ietf.org/html/draft-moran-suit-manifest-02
; [SUIT_DRAFTv3]: https://tools.ietf.org/html/draft-moran-suit-manifest-03
; [COSE_RSA]: https://tools.ietf.org/html/rfc8230
;
; The range of proprietary values to be used [-65700.....-65899].
; RSA-SSA-PKCS1-V1_5-SHA-256 = -65700
; IFX_KDF-TLS12 = -65720
;
; Untagged COSE Sign and know from Context, e.g. Trust implicitly knows
; input to update command MUST be a COSE-Sign1 Message Type
; signed_Trust = #6.18(COSE_Sign1_Trust); see Table 1 in [COSE]
; Define Signing Container with One Signer (COSE_Sign1)
COSE_Sign1_Trust = [
    protected: bstr .cbor protected-signed-header-Trust,
    unprotected: unprotected-signed-header-Trust,
    payload: bstr .cbor Trust_manifest,
    signature: bstr .size Trust_SignatureSize,
]
Trust_SignatureSize = &(
    ECC_256: 64,                               ;Raw signature: ECDSA (r|s) acc. to FIPS 186-4
                                               ;E.g. for ECC NIST P 256, size is 64 Bytes
                                               ;(the 0's must be prepended if r/s component is less than key size)
                                               ;ECC_384: 96, ;Raw signature: ECDSA (r|s) acc. to FIPS 186-4
                                               ;E.g. for ECC NIST P 384, size is 96 Bytes
                                               ;(the 0's must be prepended if r/s component is less than key size)
    RSA_1024_EXP: 128,                         ;RSA Raw Signature for the key length 1024 bits
    RSA_2048_EXP: 256,                         ;RSA Raw Signature for the key length 2048 bits
)
Trust_SignAlgorithms = &(
    ES-256: -7,                                ;[COSE]; ECDSA acc. to FIPS 186-4 and not deterministic version as suggested in Section 8.1 in COSE
    RSA-SSA-PKCS1-V1_5-SHA-256: -65700,        ;[TRUST_PROP]; RSASSA PKCS#1 V1.5 w/ SHA-256
)
IFX_DigestSize = &(
    SHA2-256-digest-size: 32,
)
DigestAlgorithms = &(
    SHA-256: 41,                               ;SHA2
) /uint .size 1
Trust_PayloadType = &(                         ;[TRUST_PROP]
    Payload_Data: -1,                          ;[TRUST_PROP]; To send the data to OPTIGA(TM) Trust Data object
)
Trust_AddInfo_WriteType = &(                   ;[TRUST_PROP]
    Write: 1,                                  ;Updates/writes the data in the target oid, starting from the offset specified
                                               ;(Used Length gets updated to (Offset + Length of the data provided),
                                               ;if this value is greater than the current value in the object metadata)
    EraseAndWrite: 2,                          ;Erases the complete data in the target oid, writes the data from the offset specified
	                                           ;(Used Length of the data object = Offset + Length of the data provided)
) /uint .size 1
protected-signed-header-Trust = {
    1 => Trust_SignAlgorithms,                 ;[COSE] and [TRUST_PROP]
}
unprotected-signed-header-Trust = {
    4 => bstr .size 2,                         ;[COSE]; key identifier:Root of Trust, Trust Anchor OID
}
Trust_manifest = [                             ;[TRUST_PROP]; OPTIGA(TM) Trust Manifest
    manifestVersion: 1,                        ;[SUIT_DRAFTv2]; Manifest data model version
    ;digestInfo: DigestInfo,                   ;[SUIT_DRAFTv2]; Digest algorithm used within manifest
    ;textReference: bstr,                      ;[SUIT_DRAFTv2]; Digest of the associated text map
    ;nonce: bstr .size 8,                      ;[SUIT_DRAFTv2]; Nonce 8 byte (IFX_PROP)
    ;sequence: uint,                           ;[SUIT_DRAFTv2]; SequenceNumber
	                                           ;current UTC time as unix epoch (Unix timestamp in seconds);
                                               ;Store as 8 byte value internally (IFX_PROP)
    preConditions: nil,                        ;[SUIT_DRAFTv2]; Array with preconditions, not used
    postConditions: nil,                       ;[SUIT_DRAFTv2]; Array with postconditions, not used
    ;directives: [],                           ;[SUIT_DRAFTv2]; Array with directives, not used
    resources: Trust_resource,                 ;[TRUST_PROP]; No Array as [SUIT_DRAFTv2], 1 Element for Data for Payload
    processors: Trust_processors,              ;[SUIT_DRAFTv2]; Array with 2 optional processing steps
    targets: Trust_target,                     ;[TRUST_PROP]; No Array as [SUIT_DRAFTv2] 1 Element for Target to update
    ;extensions : {},                          ;[SUIT_DRAFTv2]; Map with extensions, not used via empty list
]
IFX_DigestInfo = [
    digestAlgorithm: DigestAlgorithms,         ;[SUIT_DRAFTv3]; Digest algorithms
    digest: bstr .size IFX_DigestSize          ;[IFX_PROP]; size depending the IFX_DigestAlgorithm
]
Trust_resource = [ ;[TRUST_PROP]
    type: Trust_PayloadType,                   ;[SUIT_DRAFTv2]; Custom Types are indicated via negative Value
    ;indicator: {},                            ;[SUIT_DRAFTv2]; = UriList: where to find the resource;
    size: uint .size 2,                        ;[SUIT_DRAFTv2]; Size of the resource
	                                           ; Length of payload to be updated in the target object
    ;digest: DigestInfo,                       ;[SUIT_DRAFTv2]; Digest of complete payload. Not considered/supported
    
	;IFX Extensions
    Trust_PayloadVersion: uint .size 2,        ;[TRUST_PROP] Payload Version - up to (2^15 - 1 = 32767)
    
	;Additional Info has a different set of information, based on Trust_PayloadType chosen.
    AdditionalInfo: Trust_AddInfo_Data,        ;(TRUST_PROP); for Trust_PayloadType = Payload_Data
]
Trust_AddInfo_Data = [                         ;[TRUST_PROP] ; Additional information for the payload type = Payload_Data
    offset: uint .size 2,                      ;[TRUST_PROP] ; Offset from which the data to be updated
    write_type: Trust_AddInfo_WriteType        ;[TRUST_PROP] ; Specifies the type of update/write in the target
]
Trust_target = [                               ;[TRUST_PROP] based on [SUIT_DRAFTv2]
    componentIdentifier: bstr .size 0, ;[SUIT_DRAFTv2]; 0 Bytes for the Broadcast operations
    
	;Reserved for future use to enable uni-cast operations.
    storageIdentifier: bstr .size 2,           ;[SUIT_DRAFTv2]; Update Target of the Payload ; OPTIGA(TM) Object ID (IFX_PROP)
    ;encoding: bstr .size 1,                   ;[SUIT_DRAFTv2]; the format of the resource;
]
Trust_processors = [                           ;[SUIT_DRAFTv2]; = [ * ProcessingStep ]: Array of processing steps
    ProcessingStep1: ProcessingStep_integrity, ;[SUIT_DRAFTv2]; = first processing step is to check the Fragment Integrity
    ProcessingStep2: nil,                      ;[SUIT_DRAFTv2]; = second processing step is not defined. This is reserved for future use for confidentiality purpose.
]
ProcessingStep_integrity = [                   ;[SUIT_DRAFTv2]; = Custom ProcessingStep
    process: -1,                               ;[TRUST_PROP]  ; Check Integrity of first fragment
    parameters: bstr .cbor IFX_DigestInfo,     ;[TRUST_PROP]  ; Digest of first fragment
]

```

### Signature structures

```unformatted
; OPTIGA(TM) Trust (Family Trust X, Trust M...) Signature structure
;
; This file contains a CDDL (CBOR Data Definition Language) definition for a Manifest used within
OPTIGA(TM) Trust Protected update feature.
; It describes the serialization format of a manifest as CBOR.
;
;
; Naming Conventions:
; (1) Existing CDDL Definitions profiled for OPTIGA(TM) Trust are enhanced
; at the END with _Trust
; e.g. COSE_Sign1_Trust
; (2) Definitions specific for OPTIGA(TM) Trust are defined with Trust_ in the
; beginning, the same ways as done in COSE (where COSE_ is at the beginning)
; e.g. Trust_PayloadVersion
; (3) Defintion used by OPTIGA(TM) Trust and OPTIGA(TM) TPM are defined with
; IFX_ in the beginning
; e.g. IFX_DigestSize
;
; Links:
; [CBOR]: https://tools.ietf.org/html/rfc7049
; [CDDL]: https://tools.ietf.org/html/draft-ietf-cbor-cddl-05
; [COSE]: https://tools.ietf.org/html/rfc8152
; [SUIT_DRAFTv2]: https://tools.ietf.org/html/draft-moran-suit-manifest-02
; [SUIT_DRAFTv3]: https://tools.ietf.org/html/draft-moran-suit-manifest-03
; [COSE_RSA]: https://tools.ietf.org/html/rfc8230
;
; The range of proprietary values to be used [-65700.....-65899].
; RSA-SSA-PKCS1-V1_5-SHA-256 = -65700
; IFX_KDF-TLS12 = -65720
;
;
;
; [COSE] 4.4. Signing and Verification Process
; Define Container for the signature structure
Sig_structure = [
    context : "Signature1",
    body_protected : bstr .cbor protected-signed-header-Trust,
    external_aad : bstr .size 0,
    payload: bstr .cbor Trust_manifest,
]
Trust_SignatureSize = &(
    ECC_256: 64,                               ;Raw signature: ECDSA (r|s) acc. to FIPS 186-4
                                               ;E.g. for ECC NIST P 256, size is 64 Bytes
                                               ;(the 0's must be prepended if r/s component is less than key size)
    ECC_384: 96, ;Raw signature: ECDSA (r|s) acc. to FIPS 186-4
                                               ;E.g. for ECC NIST P 384, size is 96 Bytes
                                               ;(the 0's must be prepended if r/s component is less than key size)
    RSA_1024_EXP: 128,                         ;RSA Raw Signature for the key length 1024 bits
    RSA_2048_EXP: 256,                         ;RSA Raw Signature for the key length 2048 bits
)
Trust_SignAlgorithms = &(
    ES-256: -7,                                ;[COSE]; ECDSA acc. to FIPS 186-4 and not detereministic version as suggested in Section 8.1 in COSE
    RSA-SSA-PKCS1-V1_5-SHA-256: -65700,        ;[TRUST_PROP]; RSASSA PKCS#1 V1.5 w/ SHA-256
)
IFX_DigestSize = &(
    SHA2-256-digest-size: 32,
)
DigestAlgorithms = &(
    SHA-256: 41,                               ;SHA2
) /uint .size 1
Trust_PayloadType = &(                         ;[TRUST_PROP]
    Payload_Data: -1,                          ;[TRUST_PROP]; To send the data to OPTIGA(TM) Trust Data
object
)
Trust_AddInfo_WriteType = &(                   ;[TRUST_PROP]
    Write: 1,                                  ;Updates/writes the data in the target oid, starting from the offset specified
                                               ;(Used Length gets updated to (Offset + Length of the data provided),
                                               ;if this value is greater than the current value in the object metadata)
    EraseAndWrite: 2,                          ;Erases the complete data in the target oid, writes the data from the offset specified
                                               ;(Used Length of the data object = Offset + Length of the data provided)
) /uint .size 1
protected-signed-header-Trust = {
    1 => Trust_SignAlgorithms,                 ;[COSE] and [TRUST_PROP]
}
unprotected-signed-header-Trust = {
    4 => bstr .size 2,                         ;[COSE]; key identifier:Root of Trust, Trust Anchor OID
}
Trust_manifest = [                             ;[TRUST_PROP]; OPTIGA(TM) Trust Manifest
    manifestVersion: 1,                        ;[SUIT_DRAFTv2]; Manifest data model version
    ;digestInfo: DigestInfo,                   ;[SUIT_DRAFTv2]; Digest algorithm used within manifest
    ;textReference: bstr,                      ;[SUIT_DRAFTv2]; Digest of the associated text map
    ;nonce: bstr .size 8,                      ;[SUIT_DRAFTv2]; Nonce 8 byte (IFX_PROP)
    ;sequence: uint,                           ;[SUIT_DRAFTv2]; SequenceNumber;
                                               ;current UTC time as unix epoch (Unix timestamp in seconds);
                                               ;Store as 8 byte value internally (IFX_PROP)
    preConditions: nil,                        ;[SUIT_DRAFTv2]; Array with preconditions, not used
    postConditions: nil,                       ;[SUIT_DRAFTv2]; Array with postconditions, not used
    ;directives: [],                           ;[SUIT_DRAFTv2]; Array with directives, not used
    resources: Trust_resource,                 ;[TRUST_PROP]; No Array as [SUIT_DRAFTv2], 1 Element for Data for Payload
    processors: Trust_processors,              ;[SUIT_DRAFTv2]; Array with 2 optional processing steps
    targets: Trust_target,                     ;[TRUST_PROP]; No Array as [SUIT_DRAFTv2] 1 Element for
Target to update
    ;extensions : {},                          ;[SUIT_DRAFTv2]; Map with extensions, not used via empty
list
]
IFX_DigestInfo = [
    digestAlgorithm: DigestAlgorithms,         ;[SUIT_DRAFTv3]; Digest algorithms
    digest: bstr .size IFX_DigestSize          ;[IFX_PROP]; size depending the IFX_DigestAlgorithm
]
Trust_resource = [                             ;[TRUST_PROP]
    type: Trust_PayloadType,                   ;[SUIT_DRAFTv2]; Custom Types are indicated via negative
Value
    ;indicator: {},                            ;[SUIT_DRAFTv2]; = UriList: where to find the resource;
    size: uint .size 2,                        ;[SUIT_DRAFTv2]; Size of the resource; Length of payload to be updated in the target object
    ;digest: DigestInfo,                       ;[SUIT_DRAFTv2]; Digest of complete payload. Not considered/supported
    ;IFX Extensions
    Trust_PayloadVersion: uint .size 2,        ;[TRUST_PROP] Payload Version - up to (2^15 - 1 = 32767)
    ;Additional Info has a different set of information, based on Trust_PayloadType chosen.
    AdditionalInfo: Trust_AddInfo_Data,        ;(TRUST_PROP); for Trust_PayloadType = Payload_Data
]
Trust_AddInfo_Data = [                         ;[TRUST_PROP]; Additional information for the payload type = Payload_Data
    offset: uint .size 2,                      ;[TRUST_PROP]; Offset from which the data to be updated
    write_type: Trust_AddInfo_WriteType        ;[TRUST_PROP]; Specifies the type of update/write in the target
]
Trust_target = [                               ;[TRUST_PROP] based on [SUIT_DRAFTv2]
    componentIdentifier: bstr .size 0,         ;[SUIT_DRAFTv2]; 0 Bytes for the Broadcast operations
    ;Reserved for future use to enable uni-cast operations.
    storageIdentifier: bstr .size 2,           ;[SUIT_DRAFTv2]; Update Target of the Payload; OPTIGA(TM) Object ID (IFX_PROP)
    ;encoding: bstr .size 1,                   ;[SUIT_DRAFTv2]; the format of the resource;
]
Trust_processors = [                           ;[SUIT_DRAFTv2]; = [ * ProcessingStep ]: Array of processing steps
    ProcessingStep1: ProcessingStep_integrity, ;[SUIT_DRAFTv2]; = first processing step is to check the Fragment Integrity
    ProcessingStep2: nil,                      ;[SUIT_DRAFTv2]; = second processing step is not defined. This is reserved for future use for confidentiality purpose.
]
ProcessingStep_integrity = [                   ;[SUIT_DRAFTv2]; = Custom ProcessingStep
    process: -1,                               ;[TRUST_PROP]; Check Integrity of first fragment
    parameters: bstr .cbor IFX_DigestInfo,     ;[TRUST_PROP]; Digest of first fragment
]

```
