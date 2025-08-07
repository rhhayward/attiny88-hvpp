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

Read the explanation in the .ino file, which gives all the wiring instructions.
The only special wiring is that the 5v to the ATTINY88's VCC and the 12v to the
RST pin are switched using a mosfet - explanation in the .ino file. You can
also examine the `/kicad/attiny88-hvpp.kicad_sch` schematic to see that wiring
in particular. Connect to the purple pi's USB Serial port - it will emit some
help on start, and then commands can be issued.

## Commands

* `READSIG` - read and display signature bytes
* `READCAL` - read and display calibration byte
* `READFUS` - read and display fuse and lock bytes
* `READFLA 0x<START> 0x<END>` - read and display flash from start to end
* `WRITEFUS 0x<low> 0x<high> 0x<extended>` - write low high and extended
  fuses, then read and display fuse and lock bytes

