/* PAW3902 Optical Flow Sensor
 * Copyright (c) 2019 Tlera Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
 
#include "PAW3902.h"
#include <SPI.h>

PAW3902::PAW3902(uint8_t cspin)
  : _cs(cspin)
{ }


boolean PAW3902::begin(void) 
{
  // Setup SPI port
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3)); // 2 MHz max SPI clock frequency

  // Make sure the SPI bus is reset
  digitalWrite(_cs, HIGH);
  delay(1);
  digitalWrite(_cs, LOW);
  delay(1);
  digitalWrite(_cs, HIGH);
  delay(1);

  SPI.endTransaction();
}

void PAW3902::setMode(uint8_t mode) 
{
  // Reading the motion registers one time
  readByte(0x02);
  readByte(0x03);
  readByte(0x04);
  readByte(0x05);
  readByte(0x06);
  delay(1);

  if(mode == 1)
  {
  initBright();
  }

  if(mode == 2)
  {
  initLowLight();
  }

  if(mode == 3)
  {
  initSuperLowLight();
  }

}

boolean PAW3902::checkID()
{
  // check device ID
  uint8_t product_ID = readByte(0x00);
  uint8_t revision_ID = readByte(0x01);
  uint8_t inverse_product_ID = readByte(0x5F);

  Serial.print("Product ID = 0x"); Serial.print(product_ID, HEX); Serial.println(" should be 0x49");
  Serial.print("Revision ID = 0x0"); Serial.println(revision_ID, HEX); 
  Serial.print("Inverse Product ID = 0x"); Serial.print(inverse_product_ID, HEX); Serial.println(" should be 0xB6"); 

  if (product_ID != 0x49 && inverse_product_ID != 0xB6) return false;
  return true;
}


void PAW3902::reset()
{
  // Power on reset
  writeByte(0x3A, 0x5A);
  delay(5); 
}


void PAW3902::shutdown()
{
  // Shutdown
  writeByte(0x3B, 0xB6);
}



uint8_t PAW3902::status()
{
  uint8_t temp = readByte(0x02);
  return temp;
}


void PAW3902::readMotionCount(int16_t *deltaX, int16_t *deltaY, uint8_t *SQUAL, uint16_t *Shutter)
{
  *deltaX =  ((int16_t) readByte(0x04) << 8) | readByte(0x03);
  *deltaY =  ((int16_t) readByte(0x06) << 8) | readByte(0x05);
  *SQUAL =              readByte(0x07);
  *Shutter = ((uint16_t)readByte(0x0C) << 8) | readByte(0x0B);
}


void PAW3902::readBurstMode(uint8_t * dataArray)
{
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3));
  digitalWrite(_cs, LOW);
  delayMicroseconds(150);
  SPI.transfer(0x16); // start burst mode
  delayMicroseconds(5);
  for(uint8_t ii = 0; ii < 12; ii++)
  {
    dataArray[ii] = SPI.transfer(0);
  }
  digitalWrite(_cs, HIGH);
  delayMicroseconds(500);
  SPI.endTransaction();
}


void PAW3902::writeByte(uint8_t reg, uint8_t value) {
  reg |= 0x80u;

  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3));
  digitalWrite(_cs, LOW);
  delayMicroseconds(50);
  SPI.transfer(reg);
  SPI.transfer(value);
  delayMicroseconds(50);
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();
}


uint8_t PAW3902::readByte(uint8_t reg) {
  reg &= ~0x80u;

  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
  digitalWrite(_cs, LOW);
  delayMicroseconds(50);
  SPI.transfer(reg);
  uint8_t temp = SPI.transfer(0);
  delayMicroseconds(50);
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();
  
  return temp;
}


// Performance optimization registers for the three different modes
void PAW3902::initBright()
{
  writeByte(0x7F, 0x00);
  writeByte(0x55, 0x01);
  writeByte(0x50, 0x07);
  writeByte(0x7F, 0x0E);
  writeByte(0x43, 0x10);
  writeByte(0x48, 0x02);
  writeByte(0x7F, 0x00);
  writeByte(0x51, 0x7B);
  writeByte(0x50, 0x00);
  writeByte(0x55, 0x00);
  writeByte(0x7F, 0x00);

  writeByte(0x61, 0xAD);
  writeByte(0x7F, 0x03);
  writeByte(0x40, 0x00);
  writeByte(0x7F, 0x05);
  writeByte(0x41, 0xB3);
  writeByte(0x43, 0xF1);
  writeByte(0x45, 0x14);
  writeByte(0x5F, 0x34);
  writeByte(0x7B, 0x08);
  writeByte(0x5E, 0x34);
  writeByte(0x5B, 0x32);
  writeByte(0x45, 0x17);
  writeByte(0x70, 0xE5);
  writeByte(0x71, 0xE5);
  writeByte(0x7F, 0x06);
  writeByte(0x44, 0x1B);
  writeByte(0x40, 0xBF);
  writeByte(0x4E, 0x3F);
  writeByte(0x7F, 0x08);
  writeByte(0x66, 0x44);
  writeByte(0x65, 0x20);
  writeByte(0x6A, 0x3A);
  writeByte(0x61, 0x05);
  writeByte(0x62, 0x05);
  writeByte(0x7F, 0x09);
  writeByte(0x4F, 0xAF);
  writeByte(0x48, 0x80);
  writeByte(0x49, 0x80);
  writeByte(0x57, 0x77);
  writeByte(0x5F, 0x40);
  writeByte(0x60, 0x78);
  writeByte(0x61, 0x78);
  writeByte(0x62, 0x08);
  writeByte(0x63, 0x50);
  writeByte(0x7F, 0x0A);
  writeByte(0x45, 0x60);
  writeByte(0x7F, 0x00);
  writeByte(0x4D, 0x11);
  writeByte(0x55, 0x80);
  writeByte(0x74, 0x21);
  writeByte(0x75, 0x1F);
  writeByte(0x4A, 0x78);
  writeByte(0x4B, 0x78);
  writeByte(0x44, 0x08);
  writeByte(0x45, 0x50);
  writeByte(0x64, 0xFE);
  writeByte(0x65, 0x1F);
  writeByte(0x72, 0x0A);
  writeByte(0x73, 0x00);
  writeByte(0x7F, 0x14);
  writeByte(0x44, 0x84);
  writeByte(0x65, 0x47);
  writeByte(0x66, 0x18);
  writeByte(0x63, 0x70);
  writeByte(0x6F, 0x2C);
  writeByte(0x7F, 0x15);
  writeByte(0x48, 0x48);
  writeByte(0x7F, 0x07);
  writeByte(0x41, 0x0D);
  writeByte(0x43, 0x14);
  writeByte(0x4B, 0x0E);
  writeByte(0x45, 0x0F);
  writeByte(0x44, 0x42);
  writeByte(0x4C, 0x80);
  writeByte(0x7F, 0x10);
  writeByte(0x5B, 0x03);
  writeByte(0x7F, 0x07);
  writeByte(0x40, 0x41);

  delay(10);
  writeByte(0x7F, 0x00);
  writeByte(0x32, 0x00);
  writeByte(0x7F, 0x07);
  writeByte(0x40, 0x40);
  writeByte(0x7F, 0x06);
  writeByte(0x68, 0x70);
  writeByte(0x69, 0x01);
  writeByte(0x7F, 0x0D);
  writeByte(0x48, 0xC0);
  writeByte(0x6F, 0xD5);
  writeByte(0x7F, 0x00);
  writeByte(0x5B, 0xA0);
  writeByte(0x4E, 0xA8);
  writeByte(0x5A, 0x50);
  writeByte(0x40, 0x80);
  writeByte(0x73, 0x1F);

  delay(10);
  writeByte(0x73, 0x00);
}


 void PAW3902::initLowLight()   // default
{
  writeByte(0x7F, 0x00);
  writeByte(0x55, 0x01);
  writeByte(0x50, 0x07);
  writeByte(0x7F, 0x0E);
  writeByte(0x43, 0x10);
  writeByte(0x48, 0x02);
  writeByte(0x7F, 0x00);
  writeByte(0x51, 0x7B);
  writeByte(0x50, 0x00);
  writeByte(0x55, 0x00);
  writeByte(0x7F, 0x00);

  writeByte(0x61, 0xAD);
  writeByte(0x7F, 0x03);
  writeByte(0x40, 0x00);
  writeByte(0x7F, 0x05);
  writeByte(0x41, 0xB3);
  writeByte(0x43, 0xF1);
  writeByte(0x45, 0x14);
  writeByte(0x5F, 0x34);
  writeByte(0x7B, 0x08);
  writeByte(0x5E, 0x34);
  writeByte(0x5B, 0x65);
  writeByte(0x6D, 0x65);
  writeByte(0x45, 0x17);
  writeByte(0x70, 0xE5);
  writeByte(0x71, 0xE5);
  writeByte(0x7F, 0x06);
  writeByte(0x44, 0x1B);
  writeByte(0x40, 0xBF);
  writeByte(0x4E, 0x3F);
  writeByte(0x7F, 0x08);
  writeByte(0x66, 0x44);
  writeByte(0x65, 0x20);
  writeByte(0x6A, 0x3A);
  writeByte(0x61, 0x05);
  writeByte(0x62, 0x05);
  writeByte(0x7F, 0x09);
  writeByte(0x4F, 0xAF);
  writeByte(0x48, 0x80);
  writeByte(0x49, 0x80);
  writeByte(0x57, 0x77);
  writeByte(0x5F, 0x40);
  writeByte(0x60, 0x78);
  writeByte(0x61, 0x78);
  writeByte(0x62, 0x08);
  writeByte(0x63, 0x50);
  writeByte(0x7F, 0x0A);
  writeByte(0x45, 0x60);
  writeByte(0x7F, 0x00);
  writeByte(0x4D, 0x11);
  writeByte(0x55, 0x80);
  writeByte(0x74, 0x21);
  writeByte(0x75, 0x1F);
  writeByte(0x4A, 0x78);
  writeByte(0x4B, 0x78);
  writeByte(0x44, 0x08);
  writeByte(0x45, 0x50);
  writeByte(0x64, 0xFE);
  writeByte(0x65, 0x1F);
  writeByte(0x72, 0x0A);
  writeByte(0x73, 0x00);
  writeByte(0x7F, 0x14);
  writeByte(0x44, 0x84);
  writeByte(0x65, 0x67);
  writeByte(0x66, 0x18);
  writeByte(0x63, 0x70);
  writeByte(0x6F, 0x2C);
  writeByte(0x7F, 0x15);
  writeByte(0x48, 0x48);
  writeByte(0x7F, 0x07);
  writeByte(0x41, 0x0D);
  writeByte(0x43, 0x14);
  writeByte(0x4B, 0x0E);
  writeByte(0x45, 0x0F);
  writeByte(0x44, 0x42);
  writeByte(0x4C, 0x80);
  writeByte(0x7F, 0x10);
  writeByte(0x5B, 0x03);
  writeByte(0x7F, 0x07);
  writeByte(0x40, 0x41);

  delay(10);
  writeByte(0x7F, 0x00);
  writeByte(0x32, 0x00);
  writeByte(0x7F, 0x07);
  writeByte(0x40, 0x40);
  writeByte(0x7F, 0x06);
  writeByte(0x68, 0x70);
  writeByte(0x69, 0x01);
  writeByte(0x7F, 0x0D);
  writeByte(0x48, 0xC0);
  writeByte(0x6F, 0xD5);
  writeByte(0x7F, 0x00);
  writeByte(0x5B, 0xA0);
  writeByte(0x4E, 0xA8);
  writeByte(0x5A, 0x50);
  writeByte(0x40, 0x80);
  writeByte(0x73, 0x1F);

  delay(10);
  writeByte(0x73, 0x00);
}


 void PAW3902::initSuperLowLight()
{
  writeByte(0x7F, 0x00);
  writeByte(0x55, 0x01);
  writeByte(0x50, 0x07);
  writeByte(0x7F, 0x0E);
  writeByte(0x43, 0x10);
  writeByte(0x48, 0x04);
  writeByte(0x7F, 0x00);
  writeByte(0x51, 0x7B);
  writeByte(0x50, 0x00);
  writeByte(0x55, 0x00);
  writeByte(0x7F, 0x00);

  writeByte(0x61, 0xAD);
  writeByte(0x7F, 0x03);
  writeByte(0x40, 0x00);
  writeByte(0x7F, 0x05);
  writeByte(0x41, 0xB3);
  writeByte(0x43, 0xF1);
  writeByte(0x45, 0x14);
  writeByte(0x5F, 0x34);
  writeByte(0x7B, 0x08);
  writeByte(0x5E, 0x34);
  writeByte(0x5B, 0x32);
  writeByte(0x6D, 0x32);
  writeByte(0x45, 0x17);
  writeByte(0x70, 0xE5);
  writeByte(0x71, 0xE5);
  writeByte(0x7F, 0x06);
  writeByte(0x44, 0x1B);
  writeByte(0x40, 0xBF);
  writeByte(0x4E, 0x3F);
  writeByte(0x7F, 0x08);
  writeByte(0x66, 0x44);
  writeByte(0x65, 0x20);
  writeByte(0x6A, 0x3A);
  writeByte(0x61, 0x05);
  writeByte(0x62, 0x05);
  writeByte(0x7F, 0x09);
  writeByte(0x4F, 0xAF);
  writeByte(0x48, 0x80);
  writeByte(0x49, 0x80);
  writeByte(0x57, 0x77);
  writeByte(0x5F, 0x40);
  writeByte(0x60, 0x78);
  writeByte(0x61, 0x78);
  writeByte(0x62, 0x08);
  writeByte(0x63, 0x50);
  writeByte(0x7F, 0x0A);
  writeByte(0x45, 0x60);
  writeByte(0x7F, 0x00);
  writeByte(0x4D, 0x11);
  writeByte(0x55, 0x80);
  writeByte(0x74, 0x21);
  writeByte(0x75, 0x1F);
  writeByte(0x4A, 0x78);
  writeByte(0x4B, 0x78);
  writeByte(0x44, 0x08);
  writeByte(0x45, 0x50);
  writeByte(0x64, 0xCE);
  writeByte(0x65, 0x0B);
  writeByte(0x72, 0x0A);
  writeByte(0x73, 0x00);
  writeByte(0x7F, 0x14);
  writeByte(0x44, 0x84);
  writeByte(0x65, 0x67);
  writeByte(0x66, 0x18);
  writeByte(0x63, 0x70);
  writeByte(0x6F, 0x2C);
  writeByte(0x7F, 0x15);
  writeByte(0x48, 0x48);
  writeByte(0x7F, 0x07);
  writeByte(0x41, 0x0D);
  writeByte(0x43, 0x14);
  writeByte(0x4B, 0x0E);
  writeByte(0x45, 0x0F);
  writeByte(0x44, 0x42);
  writeByte(0x4C, 0x80);
  writeByte(0x7F, 0x10);
  writeByte(0x5B, 0x02);
  writeByte(0x7F, 0x07);
  writeByte(0x40, 0x41);

  delay(25);
  writeByte(0x7F, 0x00);
  writeByte(0x32, 0x44);
  writeByte(0x7F, 0x07);
  writeByte(0x40, 0x40);
  writeByte(0x7F, 0x06);
  writeByte(0x68, 0x40);
  writeByte(0x69, 0x02);
  writeByte(0x7F, 0x0D);
  writeByte(0x48, 0xC0);
  writeByte(0x6F, 0xD5);
  writeByte(0x7F, 0x00);
  writeByte(0x5B, 0xA0);
  writeByte(0x4E, 0xA8);
  writeByte(0x5A, 0x50);
  writeByte(0x40, 0x80);
  writeByte(0x73, 0x0B);

  delay(25);
  writeByte(0x73, 0x00);
}
