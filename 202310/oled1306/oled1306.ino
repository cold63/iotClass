
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void setup() {
  // put your setup code here, to run once:
  u8g2.begin();
  u8g2.setFont(u8g2_font_8x13_mf);
  u8g2.setFontPosTop();

  
 // u8g2.clear();
 // u8g2.setCursor(0,0);
 // u8g2.print("Hello world");
  //u8g2.updateDisplay();
  
  u8g2.clearBuffer();
  u8g2.drawStr(0,20,"Hello world");
  u8g2.drawHLine(0,40,120);
  u8g2.sendBuffer();

}

void loop() {
  // put your main code here, to run repeatedly:

}
