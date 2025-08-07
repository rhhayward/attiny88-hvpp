/*
 * attiny88-hvpp.ino
 *
 *  Created on: 6 Aug 2025
 *      Author: Ryan Hayward
 * 
 * This program implements  HVPP programming protocol from the ATTINY88 
 *   datasheet. The pins defined were tested on a Purple Pi rp2040 and a 
 *   12v boost converter. Both the VCC_PIN and RST_PIN connect to the base
 *   of an NPN transistor (BC547), the emitter of which goes to ground 
 *   through a 2k2 resistor.  The collector is hooked to the gate of an 
 *   P-Channel mosfet (IRLML6402).  They have 5v and 12v directly to their
 *   source as well as through a 220k resistor to the source to hold it
 *   high.  The drain is hooked to the ATTINY88's VCC and RST pins.
 *   Otherwise, all the pins hook directly to the ATTINY88.  The pin
 *   setup was chosen to try and make PCB design simpler.
 *
 * Usage: Write this to a purple pi, wire it up as discussed above,
 *   run it with an ATTINY88 connected up.  The problem will write
 *   some help to the Serial output, and take commands to execute.  Those
 *   which are currently supported are:
 *   * READSIG - read and display signature bytes
 *   * READCAL - read and display calibration byte
 *   * READFUS - read and display fuse and lock bytes
 *   * READFLA 0x<start> 0x<end> - read and display flash from start to end
 *   * WRITEFUS 0x<low> 0x<high> 0x<extended> - write low high and extended
 *       fuses, then read and display fuse and lock bytes
 *
 * TODO:  These features are not yet implemented:
 *   * chip erase
 *   * write flash
 *   * read and write eeprom
*/

#define SERIAL_SPEED 9600
// Purple PI built-in LED Pin
#define LED_PIN 25

// HVPP to ATTINY88 pin setup

// pi.0 <-> attiny88.32
// pi.1 <-> attiny88.1
// pi.2 <-> attiny88.2
// pi.3 <-> attiny88.3
// pi.4 -> transistor -> mosfet -> attiny88.4
// pi.7 <-> attiny88.7
// pi.9 <-> attiny88.9
// pi.10 <-> attiny88.10
// pi.11 <-> attiny88.11
// pi.12 <-> attiny88.12
// pi.13 <-> attiny88.13
// pi.14 <-> attiny88.14
// pi.15 <-> attiny88.15
// pi.16 <-> attiny88.16
// pi.17 <-> attiny88.17
// pi.20 <-> attiny88.23
// pi.21 <-> attiny88.24
// pi.22 <-> attiny88.25
// pi.23 -> transistor -> mosfet -> attiny88.29
// pi.24 <-> attiny88.31

#define OE_PIN 0
#define WR_PIN 1
#define VCC_PIN 4
#define BS1_PIN 2
#define CLKI_PIN 7
#define XA0_PIN 9
#define XA1_PIN 10
#define PAGEL_PIN 11
#define D0_PIN 12
#define D1_PIN 13
#define D2_PIN 14
#define D3_PIN 15
#define D4_PIN 16
#define D5_PIN 17
#define D6_PIN 20
#define D7_PIN 21
#define BS2_PIN 22
#define RST_PIN 23
#define RDYBSY_PIN 24

// mark pin as output, with initial HIGH or LOW
void setOutput(int pin, int initValue) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, initValue);
}

// setup pins with their initial values
void setStartingPinModesAndValues() {
  // set outputs low initially
  setOutput(LED_PIN, LOW);
  setOutput(VCC_PIN, LOW);
  setOutput(RST_PIN, LOW);
  setOutput(PAGEL_PIN, LOW);
  setOutput(XA1_PIN, LOW);
  setOutput(XA0_PIN, LOW);
  setOutput(BS1_PIN, LOW);
  setOutput(BS2_PIN, LOW);
  setOutput(CLKI_PIN,LOW);
  setOutput(OE_PIN, LOW);
  setOutput(WR_PIN, LOW);

  // RDYBSY is an input
  pinMode(RDYBSY_PIN, INPUT);
}

// follow procedure to initiate programming mode
void enterProgrammingMode() {

  // setting low is redundant at first, but
  //  still, let's do it for completeness

  // PAGEL=0 ; XA1=0 ; XA0=0 ; BS1=0
  digitalWrite(PAGEL_PIN, LOW);
  digitalWrite(XA1_PIN, LOW);
  digitalWrite(XA0_PIN, LOW);
  digitalWrite(BS1_PIN, LOW);

  // make sure everything is off
  digitalWrite(VCC_PIN, LOW);
  digitalWrite(RST_PIN, LOW);

  // Enable VCC to ATT
  digitalWrite(VCC_PIN, HIGH);

  // wait 20 to 60 us
  delayMicroseconds(50);

  // Put 12v on RST
  digitalWrite(RST_PIN, HIGH);
  delayMicroseconds(300);

  // set active low pins high
  digitalWrite(OE_PIN, HIGH);
  digitalWrite(WR_PIN, HIGH);
  blink(1);
}

// blink the LED a number of times
void blink(int times) {
  for(int i=0;i<times;i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
  delay(200);
}

// put data out onto data pins
void writeDataPins(uint8_t data) {
  // set data pins as output
  pinMode(D0_PIN, OUTPUT);
  pinMode(D1_PIN, OUTPUT);
  pinMode(D2_PIN, OUTPUT);
  pinMode(D3_PIN, OUTPUT);
  pinMode(D4_PIN, OUTPUT);
  pinMode(D5_PIN, OUTPUT);
  pinMode(D6_PIN, OUTPUT);
  pinMode(D7_PIN, OUTPUT);

  // split data out into pins
  digitalWrite(D0_PIN, (data >> 0) & 0x01);
  digitalWrite(D1_PIN, (data >> 1) & 0x01);
  digitalWrite(D2_PIN, (data >> 2) & 0x01);
  digitalWrite(D3_PIN, (data >> 3) & 0x01);
  digitalWrite(D4_PIN, (data >> 4) & 0x01);
  digitalWrite(D5_PIN, (data >> 5) & 0x01);
  digitalWrite(D6_PIN, (data >> 6) & 0x01);
  digitalWrite(D7_PIN, (data >> 7) & 0x01);
}

// read data from data pins
uint8_t readDataPins() {
  uint8_t result = 0;
  
  // Set data pins as inputs
  pinMode(D0_PIN, INPUT);
  pinMode(D1_PIN, INPUT);
  pinMode(D2_PIN, INPUT);
  pinMode(D3_PIN, INPUT);
  pinMode(D4_PIN, INPUT);
  pinMode(D5_PIN, INPUT);
  pinMode(D6_PIN, INPUT);
  pinMode(D7_PIN, INPUT);
  
  // Read each pin and shift it to the correct bit position
  result |= (digitalRead(D0_PIN) & 0x01) << 0;
  result |= (digitalRead(D1_PIN) & 0x01) << 1;
  result |= (digitalRead(D2_PIN) & 0x01) << 2;
  result |= (digitalRead(D3_PIN) & 0x01) << 3;
  result |= (digitalRead(D4_PIN) & 0x01) << 4;
  result |= (digitalRead(D5_PIN) & 0x01) << 5;
  result |= (digitalRead(D6_PIN) & 0x01) << 6;
  result |= (digitalRead(D7_PIN) & 0x01) << 7;
  
  return result;
}

// CLKI low for 1us, high for 1us
void pulseClockPin() {
  digitalWrite(CLKI_PIN, HIGH);
  delayMicroseconds(1);
  digitalWrite(CLKI_PIN, LOW);
  delayMicroseconds(1);
}

// follow procedure to load a command
void loadCommand(uint8_t data) {
  // XA1=1 ; XA0=0
  digitalWrite(XA1_PIN, HIGH);
  digitalWrite(XA0_PIN, LOW);

  // BS1=0
  digitalWrite(BS1_PIN, LOW);

  // load data
  writeDataPins(data);

  // pulse the clock
  pulseClockPin();
}

// follow procedure to load the address low byte
void loadAddressLowByte(uint8_t data) {
  // XA1=0 ; XA0=0
  digitalWrite(XA1_PIN, LOW);
  digitalWrite(XA0_PIN, LOW);

  // BS1=0
  digitalWrite(BS1_PIN, LOW);

  // DATA=data
  writeDataPins(data);

  pulseClockPin();
}

// follow procedure to load the address high byte
void loadAddressHighByte(uint8_t data) {
  // XA1=0 ; XA0=0
  digitalWrite(XA1_PIN, LOW);
  digitalWrite(XA0_PIN, LOW);

  // BS1=1
  digitalWrite(BS1_PIN, HIGH);

  // DATA=data
  writeDataPins(data);

  pulseClockPin();
}

// follow procedure to load the data low byte
void loadDataLowByte(uint8_t data) {
  // XA1=0 ; XA0=1
  digitalWrite(XA1_PIN, LOW);
  digitalWrite(XA0_PIN, HIGH);

  // DATA=data
  writeDataPins(data);

  pulseClockPin();
}


// variables for signature data
uint8_t sig[3] = {0};
String ident = "UNKNOWN";
// follow procedure to read signature
void readSignature() {
  // set "read signature" command
  loadCommand(0b00001000);

  // read three signature bytes
  for(uint8_t loc=0x00;loc<0x03;loc++) {
    // 0x00, 0x01, 0x02
    loadAddressLowByte(loc);

    // 0E=0 ; BS1=-0
    digitalWrite(OE_PIN, LOW);
    digitalWrite(BS1_PIN, LOW);
    // read signature
    sig[loc] = readDataPins();
    // OE=1
    digitalWrite(OE_PIN, HIGH);
  }

  // identify if possible
  if(sig[0] == 0x1e && sig[1] == 0x93 && sig[2] == 0x11) {
    ident = "ATTINY88";
  } else {
    ident = "UNKNOWN";
  }
}

// variables to hold fuses
uint8_t lfuse = 0;
uint8_t hfuse = 0;
uint8_t efuse = 0;
uint8_t lock = 0;
// follow procedure to read fuse and lock bits
void readFuseAndLockBits() {
  // set "read fuse and lock" command
  loadCommand(0b00000100);

  // OE=0 ; BS1=0 ; BS2=0
  digitalWrite(OE_PIN, LOW);
  digitalWrite(BS1_PIN, LOW);
  digitalWrite(BS2_PIN, LOW);
  // read the low fuse
  lfuse = readDataPins();

  // OE=0 ; BS1=1 ; BS2=1
  digitalWrite(OE_PIN, LOW);
  digitalWrite(BS1_PIN, HIGH);
  digitalWrite(BS2_PIN, HIGH);
  // read the high fuse
  hfuse = readDataPins();

  // OE=0 ; BS1=0 ; BS2=1
  digitalWrite(OE_PIN, LOW);
  digitalWrite(BS1_PIN, LOW);
  digitalWrite(BS2_PIN, HIGH);
  // read the extended fuse
  efuse = readDataPins();

  // OE=0 ; BS1=1 ; BS2=0
  digitalWrite(OE_PIN, LOW);
  digitalWrite(BS1_PIN, HIGH);
  digitalWrite(BS2_PIN, LOW);
  // read the lock bits
  lock = readDataPins();

  // OE=1
  digitalWrite(OE_PIN, HIGH);
}

// variable to hold calibration byte
uint8_t calibration_byte = 0;
// follow procedure to read calibration byte
void readCalibrationByte() {
  // load the "read calibration byte" command
  loadCommand(0b00001000);
  // set address low byte to 0x00
  loadAddressLowByte(0x00);
  // OE=0 ; BS1=1
  digitalWrite(OE_PIN, LOW);
  digitalWrite(BS1_PIN, HIGH);
  // read the calibration byte
  calibration_byte = readDataPins();
  // OE=1
  digitalWrite(OE_PIN, HIGH);
}

// read location from flash
uint16_t readFlash(uint16_t loc) {
  // break 16 bit loc into high and low bytes
  uint8_t low = loc & 0x00ff;
  uint8_t high = (loc & 0xff00) >> 8;
  // load the command to read flash
  loadCommand(0b00000010);

  // load high and low bytes of address
  loadAddressHighByte(high);
  loadAddressLowByte(low);

  // BS1=0 triggers low byte read
  digitalWrite(BS1_PIN, LOW);
  // OE=0 to start read
  digitalWrite(OE_PIN, LOW);
  // read the low data
  uint16_t value = readDataPins() << 8;
  // OE=1 to clear read
  digitalWrite(OE_PIN, HIGH);

  // BS1=1 triggers high byte read
  digitalWrite(BS1_PIN, HIGH);
  // OE=0 to start read
  digitalWrite(OE_PIN, LOW);
  // or in the high data
  value |= readDataPins();
  // OE=1 to clear read
  digitalWrite(OE_PIN, HIGH);

  return value;
}

// read the RDYBSY pin - single value
uint8_t readReadyBusy() {
  uint8_t readyBusy = digitalRead(RDYBSY_PIN);
  return readyBusy;
}

// write low fuse value
void writeLowFuse(uint8_t fuse_val) {
  // load the "write fuse" command
  loadCommand(0b01000000);

  // load the value into the data low byte
  loadDataLowByte(fuse_val);

  // pulse WR low
  digitalWrite(WR_PIN, LOW);
  delayMicroseconds(20);
  digitalWrite(WR_PIN, HIGH);

  // wait for RDYBSY to go HIGH
  while(readReadyBusy() == 0) delay(100);
}

// write high fuse value
void writeHighFuse(uint8_t fuse_val) {
  // load the write fuse command
  loadCommand(0b01000000);

  // load the value into the data low byte
  loadDataLowByte(fuse_val);

  // set BS1=1, BS2=0
  digitalWrite(BS1_PIN, HIGH);
  digitalWrite(BS2_PIN, LOW);

  // pulse WR low
  digitalWrite(WR_PIN, LOW);
  delayMicroseconds(20);
  digitalWrite(WR_PIN, HIGH);

  // wait for RDYBSY to go HIGH
  do { delay(100); } while(readReadyBusy() == 0);

  // reset BS1
  digitalWrite(BS1_PIN, LOW);
}

// write extended fuse value
void writeExtendedFuse(uint8_t fuse_val) {
  // load the write fuse command
  loadCommand(0b01000000);

  // load the value into the data low byte
  loadDataLowByte(fuse_val);

  // set BS1=0, BS2=1u
  digitalWrite(BS1_PIN, LOW);
  digitalWrite(BS2_PIN, HIGH);

  // pulse WR low
  digitalWrite(WR_PIN, LOW);
  delayMicroseconds(20);
  digitalWrite(WR_PIN, HIGH);

  // wait for RDYBSY to go HIGH
  while(readReadyBusy() == 0) delay(100);

  // reset BS2
  digitalWrite(BS2_PIN, LOW);
}

// extract from String s a hex value from start for len characters - 
//   returns a long, can be cast to smaller lengths
long getHexVal(String s, uint8_t start, uint8_t len) {
  // String to substr into
  String hex;
  // char array so that we can use strtoul
  char hexChars[len+1];

  
  hex = s.substring(start, start+len); 
  hex.toCharArray(hexChars, len+1);
  return strtoul(hexChars, NULL, 16); // 16 defines hex
}

// in setup we do the following:
//   * Start Serial
//   * Setup pins
//   * Enter Programming Mode
//   * Write some help text
void setup() {

  Serial.begin(SERIAL_SPEED);

  delay(3000);
  Serial.println("* starting up");
  Serial.println("* setting up pin modes and values");
  setStartingPinModesAndValues();
  Serial.println("* setting up pin modes and values - DONE");
  blink(1);

  Serial.println("* entering programming mode");
  enterProgrammingMode();
  Serial.println("* entering programming mode - DONE");
  blink(2);

  // blink(3);
  Serial.println("* Commands:");
  Serial.println("* READSIG - read signature and write to stdout");
  Serial.println("* READCAL - read calibration byte and write to stdout");
  Serial.println("* READFUS - read fuses and lock bytes");

  Serial.println("* READFLA 0x<START> 0x<END> - read flash from START to END.  Each must be 4 hex digits.");
  Serial.println("* WRITEFUS 0x<LOW> 0x<HIGH> 0x<EXTENDED> - write LOW HIGH and EXTENDED fuses.  Each must be 2 hex digits.");


  Serial.println("* * * ready");
}

// variable to hold cmd user enters
String cmd;
// loop forever expecting user input
void loop() {

  // get user input
  cmd = Serial.readStringUntil('\n');
  cmd.trim();

  // find if we got a command
  if(cmd.equalsIgnoreCase("READSIG")) {
    // read and display signature bytes
    readSignature();
    Serial.printf("* SIGNATURE: 0x%02X,0x%02X,0x%02X IDENT: %s \n", sig[0], sig[1], sig[2], ident.c_str());
  } else if(cmd.equalsIgnoreCase("READCAL")) {
    // read and display calibration byte
    readCalibrationByte();
    Serial.printf("* calibration byte: 0x%02X\n", calibration_byte);
  } else if(cmd.equalsIgnoreCase("READFUS")) {
    // read and display fuse and lock bits
    readFuseAndLockBits();
    Serial.printf("* FUSE l=0x%02X h=0x%02X x=0x%02X LOCK 0x%02X\n", lfuse, hfuse, efuse, lock);
  } else if(cmd.startsWith("READFLA 0x") && cmd.length() == 21) {
    // read and display flash bytes - notice special handling of customer provided
    //   arguments - read from start to end
    uint16_t start = (uint16_t)getHexVal(cmd, 10, 4);
    uint16_t end = (uint16_t)getHexVal(cmd, 17, 4);

    Serial.printf("* start=0x%04x end=0x%04x\n", start, end);
    for(uint16_t i=0;i<(end-start);i++) {
      uint16_t byte = readFlash(start+i);
      // print address every 16 bytes
      if((i%16)==0) Serial.printf("* 0x%04x  ", (start+i));
      // print each byte
      Serial.printf(" %04x", byte);
      // print an extra space half way through
      if((i%16)==7) Serial.printf(" ");
      // print a newline every last of 16 bytes
      if((i%16)==15) Serial.printf("\n");
    }
    Serial.println(); // may write an extra line - that's okay by me
  } else if(cmd.startsWith("WRITEFUS 0x") && cmd.length() == 23) {
    // write fuse bytes, then read and display fuse and lock bits to show what happenedu
    uint8_t l = (uint8_t)getHexVal(cmd, 11, 2);
    uint8_t h = (uint8_t)getHexVal(cmd, 16, 2);
    uint8_t x = (uint8_t)getHexVal(cmd, 21, 2);

    Serial.printf("* WRITING FUSES l=0x%02X h=0x%02X x=0x%02X\n", l, h, x);
    writeLowFuse(l);
    writeHighFuse(h);
    writeExtendedFuse(x);
    Serial.printf("* DONE WRITING FUSES.  READING FUSES...\n");
    readFuseAndLockBits();
    Serial.printf("* FUSE l=0x%02X h=0x%02X x=0x%02X LOCK 0x%02X\n", lfuse, hfuse, efuse, lock);
  }
}
