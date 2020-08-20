# Error Codes

Possile errors you can get from the host library include errros from the host library itself; e.g. communication errors, or an error from the security element.

## Errors from the host library

All generic statuses are defined in the [`optiga_lib_return_codes.h`](https://github.com/Infineon/optiga-trust-charge/blob/master/optiga/include/optiga/common/optiga_lib_return_codes.h) file
Below are some of them.

| Code  | Description |
|:-----:| ----------- |
| 0x0102  | Communication error |
| 0x0103  | OPTIGA comms API called with invalid inputs |
| 0x0104  | OPTIGA comms API called with insufficient memory buffer |
| 0x0105  | Protocol stack memory insufficient |
| 0x0106  | Protocol fatal error|
| 0x0107  | Presentation layer handshake error|
| 0x0108  | Presentation layer session error|
| 0x0202  | OPTIGA command API failed|
| 0x0203  |OPTIGA command API called with invalid inputs|
| 0x0204  | OPTIGA command API called with insufficient memory buffer|
| 0x0302  | OPTIGA util API failed|
| 0x0303  | OPTIGA util API called with invalid inputs|
| 0x0304  | OPTIGA util API called with insufficient memory buffer|
| 0x0305  | OPTIGA util API called when, a request of same instance is already in service|
| 0x0402  | OPTIGA crypt API failed|
| 0x0403  | OPTIGA crypt API called with invalid inputs|
| 0x0404  | OPTIGA crypt API called with insufficient memory buffer|
| 0x0405  | OPTIGA crypt API called when, a request of same instance is already in service|


## Errors from the secure element

Possible error codes returned from the security chip are listed in Table below (for more details please refer to the [Solution Reference Manual](OPTIGA_Trust_Charge_V1_Solution_Reference_Manual_v1.00.pdf)). If multiple commands happen to produce
subsequent errors then only the highest-numbered error code is stored.


| Field         | Code  | Description |
| ------------- |:-----:| ----------- |
| No error            | 0x8000  | No Error |
| Invalid OID         | 0x8001  |  Invalid OID |
| Invalid Password    | 0x8002  |  Invalid Password |
| Invalid Param field | 0x8003  | Invalid Param field in command |
| Invalid length field| 0x8004  | Invalid Length field in command|
| Invalid parameter in data field|  0x8005|  Invalid parameter in command data field|
| Internal process error|  0x8006| Internal process error|
| Access conditions not satisfied|  0x8007|  Access conditions are not satisfied|
| Data object boundary exceeded|  0x8008|  The sum of offset and data provided (offset + data length) exceeds the max length of the data object|
| Metadata truncation error| 0x8009 |  Metadata truncation error|
| Invalid command field|  0x800A |  Invalid command field |
| Command not available|  0x800C|<ul><li>due to termination state of the application</li><li>due to Application closed</li></ul> |
|Insufficient buffer/ memory| 0x800D| Insufficient memory to process the command APDU|
|Counter threshold limit exceeded| 0x800E| Counter value crossed the threshold limit and further counting is denied.|
|Invalid Manifest| 0x800F | <ul><li> The Manifest version provided is not supported or the Payload Version in Manifest has MSB set (Invalid Flag=1).</li><li> Invalid or un-supported manifest values or formats including CBOR parsing errors. </li>|
|Invalid/Wrong Payload Version | 0x8010| The Payload Version provided in the Manifest is not greater than the version of the target object, or the last update was interrupted and the restarted/retried update has not the same version|
|Version mismatch| 0x8022| Protocol or data structure version mismatch (e.g. X.509 Version, ...).|
|Insufficient/Unsupported Cipher Suite| 0x8023 |Cipher suite mismatch between client and server.|
|Unsupported extension/identifier |0x8024| <ul><li>An unsupported extension found in the message</li><li>Unsupported keyusage/Algorithm extension/identifier for the usage of Private key</li></ul>
|Invalid Trust Anchor| 0x8026| The Trust Anchor is either not loaded or the loaded Trust Anchor is invalid e.g. not well formed X.509 certificate, public key missing, ...).|
|Trust Anchor Expired| 0x8027 |The Trust Anchor loaded at OPTIGA™ Trust is expired.|
|Unsupported Trust Anchor| 0x8028| The cryptographic algorithms specified in Trust Anchor loaded are not supported by OPTIGA™ Trust.|
|Invalid certificate format| 0x8029 |Invalid certificate(s) in certificate message with the following reasons.| <ul><li> Invalid format</li> <li>Invalid chain of certificates</li> <li>Signature verification failure</li></ul>
|Unsupported certificate algorithm |0x802A| At least one cryptographic algorithm specified in the certificate is not supported (e.g. hash or sign algorithms).|
|Certificate expired| 0x802B| The certificate or at least one certificate in a certificate chain received is expired.|
|Signature verification failure| 0x802C| Signature verification failure.|
