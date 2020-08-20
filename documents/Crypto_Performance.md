# Crypto Performance

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
