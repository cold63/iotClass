#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R2, /* reset=*/ U8X8_PIN_NONE);

int fontWitdh,fontHigh;
void setup() {
  // put your setup code here, to run once:
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_unifont_t_chinese);
  u8g2.setFontPosTop();
  fontWitdh = u8g2.getMaxCharWidth();
  fontHigh = u8g2.getMaxCharHeight();

   u8g2.clear();
   u8g2.clearBuffer();
   u8g2.setCursor(fontWitdh * 1,fontHigh *2);
   u8g2.print("哈嘍!");
   u8g2.setCursor(fontWitdh * 2,fontHigh *3);
   u8g2.print("我是");
   u8g2.sendBuffer();  
}

void loop() {
  // put your main code here, to run repeatedly:

}
