### ATTINY88 High Voltage Parallel Programmer

This project came from a bricked attiny88. I had changed the fuses, and after
one particular update, none of the methods to program or update fuses worked.
The programmer was unable to read the signature or fuses. However, the code
was still working, albeit terribly slowly - an LED which normally blinked once
per second was taking minutes to blink. While ATTINY88 microcontrollers are
inexpensive and could easily be replaced, I took it as a challenge to unbrick
this one. Reading the datasheet and online, a high voltage parallel programmer
was what was needed, but I couldn't find code or a schematic, so I created my
own.

This was tested with a purple pi (off-brand), a boost converter to provide 12,
and an attiny88. I was able to use the write flash command to unbrick the
device.

### How to use

The wiring is explained in the .ino file. The only special wiring is that the
5v to the ATTINY88's VCC and the 12v to the RST pin are switched using a
mosfet. Aside from those, wiring is direct from the purple pi to the ATTINY88.
You can see the wiring in the `/kicad/attiny88-hvpp.kicad_sch` schematic
(https://kicanvas.org/?github=https%3A%2F%2Fgithub.com%2Frhhayward%2Fattiny88-hvpp%2Ftree%2Fmain%2Fkicad).

Once wired, build and push the code to the purple pi. I have tested this using
the Arduino IDE 2 with the Raspberry Pi Pico profile which worked for me with no
trouble. With the code running, connect to the purple pi's USB Serial port.
The code will always prefix its output with a `*`.  It will emit some help on
start.  Once it announces done, you can start issuing commands.

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
