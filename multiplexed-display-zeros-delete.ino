/**
 * This sketch is specifically for programming the EEPROM used in the 8-bit
 * decimal display decoder described in https://youtu.be/dLh1n2dErzE
 * Added code to delete leading zeros
 */
#define SHIFT_DATA 2
#define SHIFT_CLK 3
#define SHIFT_LATCH 4
#define EEPROM_D0 5
#define EEPROM_D7 12
#define WRITE_EN 13

/*
   Output the address bits and outputEnable signal using shift registers.
*/
void setAddress(int address, bool outputEnable) {
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, (address >> 8) | (outputEnable ? 0x00 : 0x80));
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, address);

  digitalWrite(SHIFT_LATCH, LOW);
  digitalWrite(SHIFT_LATCH, HIGH);
  digitalWrite(SHIFT_LATCH, LOW);
}


/*
   Read a byte from the EEPROM at the specified address.
*/
byte readEEPROM(int address) {
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, INPUT);
  }
  setAddress(address, /*outputEnable*/ true);

  byte data = 0;
  for (int pin = EEPROM_D7; pin >= EEPROM_D0; pin -= 1) {
    data = (data << 1) + digitalRead(pin);
  }
  return data;
}


/*
   Write a byte to the EEPROM at the specified address.
*/
void writeEEPROM(int address, byte data) {
  setAddress(address, /*outputEnable*/ false);
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, OUTPUT);
  }

  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    digitalWrite(pin, data & 1);
    data = data >> 1;
  }
  digitalWrite(WRITE_EN, LOW);
  delayMicroseconds(1);
  digitalWrite(WRITE_EN, HIGH);
  delay(10);
}


/*
   Read the contents of the EEPROM and print them to the serial monitor.
*/
void printContents() {
  for (int base = 0; base <= 2047; base += 16) {   // changed 255 to 2024
    byte data[16];
    for (int offset = 0; offset <= 15; offset += 1) {
      data[offset] = readEEPROM(base + offset);
    }

    char buf[800];   // original 80
    sprintf(buf, "%03x:  %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
            base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
            data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

    Serial.println(buf);
  }
}


void setup() {
  // put your setup code here, to run once:
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  digitalWrite(WRITE_EN, HIGH);
  pinMode(WRITE_EN, OUTPUT);
  Serial.begin(57600);


  // Bit patterns for the digits 0..9
  byte digits[] = { 0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5b, 0x5f, 0x70, 0x7f, 0x7b };

  Serial.println("Programming ones place");
  for (int value = 0; value <= 255; value += 1) {
    writeEEPROM(value, digits[value % 10]);
  }
  Serial.println("Programming tens place");
  for (int value = 0; value <= 255; value += 1) {
    writeEEPROM(value + 256, digits[(value / 10) % 10]);
  }
  Serial.println("Programming hundreds place");
  for (int value = 0; value <= 255; value += 1) {
    writeEEPROM(value + 512, digits[(value / 100) % 10]);
  }
  Serial.println("Programming sign");
  for (int value = 0; value <= 255; value += 1) {
    writeEEPROM(value + 768, 0);
  }

  Serial.println("Programming ones place (twos complement)");
  for (int value = -128; value <= 127; value += 1) {
    writeEEPROM((byte)value + 1024, digits[abs(value) % 10]);
  }
  Serial.println("Programming tens place (twos complement)");
  for (int value = -128; value <= 127; value += 1) {
    writeEEPROM((byte)value + 1280, digits[abs(value / 10) % 10]);
  }
  Serial.println("Programming hundreds place (twos complement)");
  for (int value = -128; value <= 127; value += 1) {
    writeEEPROM((byte)value + 1536, digits[abs(value / 100) % 10]);
  }
  Serial.println("Programming sign (twos complement)");
  for (int value = -128; value <= 127; value += 1) {
    if (value < 0) {
      writeEEPROM((byte)value + 1792, 0x01);
    } else {
      writeEEPROM((byte)value + 1792, 0);
    }
  }
//***********************************************************************************************************************************
//Modifying delete leading zeros
//Added Code not to display leading zeros

Serial.println("Modifying tens place");  // Write 0's to 10 addresses starting at address 256 (100h)  9 digits
  for (int value = 256; value <= 265; value += 1) {
    writeEEPROM(value, 0);
  }

Serial.println("Modifying hundreds place");  // Write 0's to 100 addresses starting at address 512 (200h)  99 digits
  for (int value = 512; value <= 611; value += 1) {
    writeEEPROM(value, 0);    
  }


Serial.println("Modifying tens place (twos complement)");  // Write 0's to 10 addresses starting at address 1280 (100h)  9 digits
  for (int value = 1280; value <= 1289; value += 1) {
    writeEEPROM(value, 0);
  }

Serial.println("Modifying hundreds place (twos complement)");  // Write 0's to 100 addresses starting at address 1536 (200h)  99 digits
  for (int value = 1536; value <= 1635; value += 1) {
    writeEEPROM(value, 0);    
   }  


Serial.println("Modifying tens place (twos complement)");  // Write 0's to 10 addresses starting at address 1527 (100h)  9 digits
  for (int value = 1527; value <= 1535; value += 1) {
    writeEEPROM(value, 0);
  }

Serial.println("Modifying hundreds place (twos complement)");  // Write 0's to 100 addresses starting at address 1693 (200h)  99 digits
  for (int value = 1693; value <= 1791; value += 1) {
    writeEEPROM(value, 0);    
   }  
//***********************************************************************************************************************************

  // Read and print out the contents of the EERPROM
  Serial.println("Reading EEPROM");
  printContents();
}


void loop() {
  // put your main code here, to run repeatedly:

}


/*  Output Final Mod
 
Programming ones place
Programming tens place
Programming hundreds place
Programming sign
Programming ones place (twos complement)
Programming tens place (twos complement)
Programming hundreds place (twos complement)
Programming sign (twos complement)
Modifying tens place
Modifying hundreds place
Modifying tens place (twos complement)
Modifying hundreds place (twos complement)
Modifying tens place (twos complement)
Modifying hundreds place (twos complement)
Reading EEPROM
000:  7e 30 6d 79 33 5b 5f 70   7f 7b 7e 30 6d 79 33 5b
010:  5f 70 7f 7b 7e 30 6d 79   33 5b 5f 70 7f 7b 7e 30
020:  6d 79 33 5b 5f 70 7f 7b   7e 30 6d 79 33 5b 5f 70
030:  7f 7b 7e 30 6d 79 33 5b   5f 70 7f 7b 7e 30 6d 79
040:  33 5b 5f 70 7f 7b 7e 30   6d 79 33 5b 5f 70 7f 7b
050:  7e 30 6d 79 33 5b 5f 70   7f 7b 7e 30 6d 79 33 5b
060:  5f 70 7f 7b 7e 30 6d 79   33 5b 5f 70 7f 7b 7e 30
070:  6d 79 33 5b 5f 70 7f 7b   7e 30 6d 79 33 5b 5f 70
080:  7f 7b 7e 30 6d 79 33 5b   5f 70 7f 7b 7e 30 6d 79
090:  33 5b 5f 70 7f 7b 7e 30   6d 79 33 5b 5f 70 7f 7b
0a0:  7e 30 6d 79 33 5b 5f 70   7f 7b 7e 30 6d 79 33 5b
0b0:  5f 70 7f 7b 7e 30 6d 79   33 5b 5f 70 7f 7b 7e 30
0c0:  6d 79 33 5b 5f 70 7f 7b   7e 30 6d 79 33 5b 5f 70
0d0:  7f 7b 7e 30 6d 79 33 5b   5f 70 7f 7b 7e 30 6d 79
0e0:  33 5b 5f 70 7f 7b 7e 30   6d 79 33 5b 5f 70 7f 7b
0f0:  7e 30 6d 79 33 5b 5f 70   7f 7b 7e 30 6d 79 33 5b
100:  00 00 00 00 00 00 00 00   00 00 30 30 30 30 30 30
110:  30 30 30 30 6d 6d 6d 6d   6d 6d 6d 6d 6d 6d 79 79
120:  79 79 79 79 79 79 79 79   33 33 33 33 33 33 33 33
130:  33 33 5b 5b 5b 5b 5b 5b   5b 5b 5b 5b 5f 5f 5f 5f
140:  5f 5f 5f 5f 5f 5f 70 70   70 70 70 70 70 70 70 70
150:  7f 7f 7f 7f 7f 7f 7f 7f   7f 7f 7b 7b 7b 7b 7b 7b
160:  7b 7b 7b 7b 7e 7e 7e 7e   7e 7e 7e 7e 7e 7e 30 30
170:  30 30 30 30 30 30 30 30   6d 6d 6d 6d 6d 6d 6d 6d
180:  6d 6d 79 79 79 79 79 79   79 79 79 79 33 33 33 33
190:  33 33 33 33 33 33 5b 5b   5b 5b 5b 5b 5b 5b 5b 5b
1a0:  5f 5f 5f 5f 5f 5f 5f 5f   5f 5f 70 70 70 70 70 70
1b0:  70 70 70 70 7f 7f 7f 7f   7f 7f 7f 7f 7f 7f 7b 7b
1c0:  7b 7b 7b 7b 7b 7b 7b 7b   7e 7e 7e 7e 7e 7e 7e 7e
1d0:  7e 7e 30 30 30 30 30 30   30 30 30 30 6d 6d 6d 6d
1e0:  6d 6d 6d 6d 6d 6d 79 79   79 79 79 79 79 79 79 79
1f0:  33 33 33 33 33 33 33 33   33 33 5b 5b 5b 5b 5b 5b
200:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
210:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
220:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
230:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
240:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
250:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
260:  00 00 00 00 30 30 30 30   30 30 30 30 30 30 30 30
270:  30 30 30 30 30 30 30 30   30 30 30 30 30 30 30 30
280:  30 30 30 30 30 30 30 30   30 30 30 30 30 30 30 30
290:  30 30 30 30 30 30 30 30   30 30 30 30 30 30 30 30
2a0:  30 30 30 30 30 30 30 30   30 30 30 30 30 30 30 30
2b0:  30 30 30 30 30 30 30 30   30 30 30 30 30 30 30 30
2c0:  30 30 30 30 30 30 30 30   6d 6d 6d 6d 6d 6d 6d 6d
2d0:  6d 6d 6d 6d 6d 6d 6d 6d   6d 6d 6d 6d 6d 6d 6d 6d
2e0:  6d 6d 6d 6d 6d 6d 6d 6d   6d 6d 6d 6d 6d 6d 6d 6d
2f0:  6d 6d 6d 6d 6d 6d 6d 6d   6d 6d 6d 6d 6d 6d 6d 6d
300:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
310:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
320:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
330:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
340:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
350:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
360:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
370:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
380:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
390:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
3a0:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
3b0:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
3c0:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
3d0:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
3e0:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
3f0:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
400:  7e 30 6d 79 33 5b 5f 70   7f 7b 7e 30 6d 79 33 5b
410:  5f 70 7f 7b 7e 30 6d 79   33 5b 5f 70 7f 7b 7e 30
420:  6d 79 33 5b 5f 70 7f 7b   7e 30 6d 79 33 5b 5f 70
430:  7f 7b 7e 30 6d 79 33 5b   5f 70 7f 7b 7e 30 6d 79
440:  33 5b 5f 70 7f 7b 7e 30   6d 79 33 5b 5f 70 7f 7b
450:  7e 30 6d 79 33 5b 5f 70   7f 7b 7e 30 6d 79 33 5b
460:  5f 70 7f 7b 7e 30 6d 79   33 5b 5f 70 7f 7b 7e 30
470:  6d 79 33 5b 5f 70 7f 7b   7e 30 6d 79 33 5b 5f 70
480:  7f 70 5f 5b 33 79 6d 30   7e 7b 7f 70 5f 5b 33 79
490:  6d 30 7e 7b 7f 70 5f 5b   33 79 6d 30 7e 7b 7f 70
4a0:  5f 5b 33 79 6d 30 7e 7b   7f 70 5f 5b 33 79 6d 30
4b0:  7e 7b 7f 70 5f 5b 33 79   6d 30 7e 7b 7f 70 5f 5b
4c0:  33 79 6d 30 7e 7b 7f 70   5f 5b 33 79 6d 30 7e 7b
4d0:  7f 70 5f 5b 33 79 6d 30   7e 7b 7f 70 5f 5b 33 79
4e0:  6d 30 7e 7b 7f 70 5f 5b   33 79 6d 30 7e 7b 7f 70
4f0:  5f 5b 33 79 6d 30 7e 7b   7f 70 5f 5b 33 79 6d 30
500:  00 00 00 00 00 00 00 00   00 00 30 30 30 30 30 30
510:  30 30 30 30 6d 6d 6d 6d   6d 6d 6d 6d 6d 6d 79 79
520:  79 79 79 79 79 79 79 79   33 33 33 33 33 33 33 33
530:  33 33 5b 5b 5b 5b 5b 5b   5b 5b 5b 5b 5f 5f 5f 5f
540:  5f 5f 5f 5f 5f 5f 70 70   70 70 70 70 70 70 70 70
550:  7f 7f 7f 7f 7f 7f 7f 7f   7f 7f 7b 7b 7b 7b 7b 7b
560:  7b 7b 7b 7b 7e 7e 7e 7e   7e 7e 7e 7e 7e 7e 30 30
570:  30 30 30 30 30 30 30 30   6d 6d 6d 6d 6d 6d 6d 6d
580:  6d 6d 6d 6d 6d 6d 6d 6d   6d 30 30 30 30 30 30 30
590:  30 30 30 7e 7e 7e 7e 7e   7e 7e 7e 7e 7e 7b 7b 7b
5a0:  7b 7b 7b 7b 7b 7b 7b 7f   7f 7f 7f 7f 7f 7f 7f 7f
5b0:  7f 70 70 70 70 70 70 70   70 70 70 5f 5f 5f 5f 5f
5c0:  5f 5f 5f 5f 5f 5b 5b 5b   5b 5b 5b 5b 5b 5b 5b 33
5d0:  33 33 33 33 33 33 33 33   33 79 79 79 79 79 79 79
5e0:  79 79 79 6d 6d 6d 6d 6d   6d 6d 6d 6d 6d 30 30 30
5f0:  30 30 30 30 30 30 30 00   00 00 00 00 00 00 00 00
600:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
610:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
620:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
630:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
640:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
650:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
660:  00 00 00 00 30 30 30 30   30 30 30 30 30 30 30 30
670:  30 30 30 30 30 30 30 30   30 30 30 30 30 30 30 30
680:  30 30 30 30 30 30 30 30   30 30 30 30 30 30 30 30
690:  30 30 30 30 30 30 30 30   30 30 30 30 30 00 00 00
6a0:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
6b0:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
6c0:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
6d0:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
6e0:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
6f0:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
700:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
710:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
720:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
730:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
740:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
750:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
760:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
770:  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00
780:  01 01 01 01 01 01 01 01   01 01 01 01 01 01 01 01
790:  01 01 01 01 01 01 01 01   01 01 01 01 01 01 01 01
7a0:  01 01 01 01 01 01 01 01   01 01 01 01 01 01 01 01
7b0:  01 01 01 01 01 01 01 01   01 01 01 01 01 01 01 01
7c0:  01 01 01 01 01 01 01 01   01 01 01 01 01 01 01 01
7d0:  01 01 01 01 01 01 01 01   01 01 01 01 01 01 01 01
7e0:  01 01 01 01 01 01 01 01   01 01 01 01 01 01 01 01
7f0:  01 01 01 01 01 01 01 01   01 01 01 01 01 01 01 01

 */




