#include <HT1632.h>
#include <font_8x4.h>
#include <images.h>

int i = 0;
int wd;
char disp [] = "Hello, how are you?";

void setup () {
  HT1632.begin(12, 10, 9);
  
  wd = HT1632.getTextWidth(disp, FONT_8X4_END, FONT_8X4_HEIGHT);
}

void loop () {
  
  HT1632.drawTarget(BUFFER_BOARD(1));
  HT1632.clear();
  HT1632.drawText(disp, OUT_SIZE - i, 0, FONT_8X4, FONT_8X4_END, FONT_8X4_HEIGHT);
  HT1632.render();
  
  i = (i+1)%(wd + OUT_SIZE);
  
  delay(100);
}
