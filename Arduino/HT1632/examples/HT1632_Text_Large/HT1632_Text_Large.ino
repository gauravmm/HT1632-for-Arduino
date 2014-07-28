#include <HT1632.h>
#include <font_8x4.h>
#include <images.h>

char c [] = "S\0";

void setup () {
  Serial.begin(9600);
  HT1632.begin(12, 10, 9);
}

void loop () {
  while(!Serial.available());
  int ct = Serial.read();
  c[0] = (char) ct;
  HT1632.drawTarget(BUFFER_BOARD(1));
  HT1632.clear();
  HT1632.drawText(c, 0, 0, FONT_8X4, FONT_8X4_END, FONT_8X4_HEIGHT);
  HT1632.render();
  delay(50);
}
