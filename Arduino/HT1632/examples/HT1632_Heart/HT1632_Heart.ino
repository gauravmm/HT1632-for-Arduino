#include <HT1632.h>
#include <font_5x4.h>
#include <images.h>

int i = 0;

void setup () {
  HT1632.begin(12, 10, 9);
}

void loop () {
  // The definitions for IMG_HEART and its width and height are available in images.h.
  // This step only performs the drawing in internal memory. 
  HT1632.drawImage(IMG_HEART, IMG_HEART_WIDTH,  IMG_HEART_HEIGHT, (OUT_SIZE - IMG_HEART_WIDTH)/2, 0);
  
  HT1632.render(); // This updates the display on the screen.
  
  delay(1000);
  
  HT1632.clear(); // This zeroes out the internal memory.
  
  HT1632.render(); // This updates the screen display.
  
  delay(1000);
}
