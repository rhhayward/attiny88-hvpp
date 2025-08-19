### Important

The code has been tested on a breadboard, and the original pcb has been
validated to work.  There were two changes made since ordering: 1. The
attiny88 footprint was not wide enough, so it was widened in REV002.
2. The 12v source had left and right reversed - that is fixed in REV003.

Caveat emptor:  If you order PCBs from this, be prepared for them not
to work.  You have been warned.

### ATTINY88 High Voltage Parallel Programmer

This project came from a bricked attiny88. I was changing the fuses, and after
one particular update, none of the methods to program or update fuses worked.
The programmer was unable even to read the signature or fuses. However, the code
was still working, albeit terribly slowly. An LED which previously blinked once
per second was taking minutes to complete a blink cycle. While ATTINY88
microcontrollers are inexpensive and could easily be replaced, I took it as a
challenge to unbrick this one. Reading the datasheet and online, a high voltage
parallel programmer was what was needed, but I couldn't find code or a
schematic, so I worked to get a hvpp programmer working.

This was validated working with a purple pi (off-brand rp2040), a 12v boost
regulator, and the aforementioned bricked ATTINY88. I was then able to use the
`WRITEFUS` command to unbrick the device.  I implemented a few additional
commands as well.  The entire suite of possible commands is not yet available,
but should be doable leveraging the primitives that are already implemented.

### How to

The wiring is laid out in the .ino file. The only additional circuitry is that
the 5v providing the ATTINY88's VCC and the 12v necessary for the RST pin are
switched using a transistor+mosfet. All other wiring is direct from the purple
pi to the ATTINY88.  You can see the full schematic in
`/kicad/attiny88-hvpp.kicad_sch`
(https://kicanvas.org/?github=https%3A%2F%2Fgithub.com%2Frhhayward%2Fattiny88-hvpp%2Ftree%2Fmain%2Fkicad).

Once wired, you canbuild and push the code to the purple pi. I have tested this
using the Arduino IDE 2 with the Raspberry Pi Pico profile which worked for me
with no trouble. With the code running, connect to the purple pi's USB Serial
port.  The code will always prefix its output with a `*`.  It will emit some
help on start.  Once it announces done, you can start issuing commands.

## Commands

* `READSIG` - read and display signature bytes
* `READCAL` - read and display calibration byte
* `READFUS` - read and display fuse and lock bytes
* `READFLA 0x<START> 0x<END>` - read and display flash from start to end
* `WRITEFUS 0x<low> 0x<high> 0x<extended>` - write low high and extended
  fuses, then read and display fuse and lock bytes

## Example execution

```
* starting up
* setting up pin modes and values
* setting up pin modes and values - DONE
* entering programming mode
* entering programming mode - DONE
* Commands:
* READSIG - read signature and write to stdout
* READCAL - read calibration byte and write to stdout
* READFUS - read fuses and lock bytes
* READFLA 0x<START> 0x<END> - read flash from START to END.  Each must be 4 hex digits.
* WRITEFUS 0x<LOW> 0x<HIGH> 0x<EXTENDED> - write LOW HIGH and EXTENDED fuses.  Each must be 2 hex digits.
* * * ready
```
```
READSIG
```
```
* SIGNATURE: 0x1E,0x93,0x11 IDENT: ATTINY88
```
```
READFUS
```
```
* FUSE l=0x37 h=0xDF x=0xFF LOCK 0xFF
```
```
READCAL
```
```
* calibration byte: 0x9D
```
```
READFLA 0x0000 0x0010
```
```
* start=0x0000 end=0x0010
* 0x0000   13c0 22c0 21c0 20c0 1fc0 1ec0 1dc0 1cc0  1bc0 1ac0 19c0 18c0 17c0 16c0 74c0 14c0
```
