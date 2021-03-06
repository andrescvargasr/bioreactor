#include "SST.h"
#include <SPI.h>
#include <avr/io.h>

SST sst = SST('F', 4); // A3 is F4


boolean wr;

boolean verbose=false;



#define ADDRESS_BEG   0x000000
#define ADDRESS_MAX   0x080000  // http://www.sst.com/dotAsset/40498.pdf&usd=2&usg=ALhdy294tEkn4s_aKwurdSetYTt_vmXQhw
#define SECTOR_SIZE       4096 // anyway the size of the sector is also hardcoded in the library !!!!

#define LINE_SIZE 64 // should be a divider of the SECTOR_SIZE


uint8_t testBytes[LINE_SIZE];



// ======================================================================================= //

void setup()
{
  for (int i = 0; i < LINE_SIZE; i++) {
    testBytes[i] = random(0, 255);
  }
  // waiting 10s before starting the FORMATTING
  delay(10000);
  Serial.begin(9600);
  setupMemory(sst);
  Serial.print("FlashID: ");
  sst.printFlashID(&Serial);

  Serial.println("Starting test");

  for (long i = 0; i < ADDRESS_MAX; i++) {
    if (i % SECTOR_SIZE == 0) { // should erase the sector
      Serial.print("Formatting and testing sector (4096 bytes): ");
      Serial.println(i / SECTOR_SIZE);
      sst.flashSectorErase(i);
    }
    if (true) { // do we want to write and read the flash ?
      if (i % LINE_SIZE == 0) printLine(i, false);
      if (i % LINE_SIZE == 0) writeLine(i);
      if (i % LINE_SIZE == 0) printLine(i, true);
    }
  }
}

// ======================================================================================= //



void loop() {}


void printLine(long address, boolean check) {
  if (verbose) {
    Serial.print("Read Address:  ");
    Serial.print(address);
    Serial.print(" : ");
  }
  sst.flashReadInit(address);
  for (byte j = 0; j < LINE_SIZE; j++) {
    byte oneByte = sst.flashReadNextInt8();
    if (check && testBytes[j] != oneByte) {
      Serial.print(testBytes[j]);
      Serial.print(" ");
      Serial.print(oneByte);
      Serial.print(" ");
      Serial.println("VERIFICATION ERROR - DONT USE THIS FLASH !!!!");
    }
    if (verbose) {
      if (oneByte < 16) Serial.print("0");
      Serial.print(oneByte, HEX);
      Serial.print(" ");
      address++;
    }
  }
  sst.flashReadFinish();
  if (verbose) Serial.println("");
}

void writeLine(long address) {
  if (verbose) {
    Serial.print("Write Address: ");
    Serial.print(address);
    Serial.print(" : ");
  }
  sst.flashWriteInit(address);
  for (byte j = 0; j < LINE_SIZE; j++) {
    sst.flashWriteNextInt8(testBytes[j]);
    if (verbose) {
      if (testBytes[j] < 16) Serial.print("0");
      Serial.print(testBytes[j], HEX);
      Serial.print(" ");
    }
    address++;
  }
  sst.flashReadFinish();
  if (verbose) Serial.println("");
}



void setupMemory(SST sst) {
  SPI.begin();

  sst.init();
}
