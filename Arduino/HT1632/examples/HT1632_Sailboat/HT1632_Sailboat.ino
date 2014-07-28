#include <HT1632.h>
#include <font_5x4.h>
#include <images.h>

int i = 0;
int wd;

void setup () {
  HT1632.begin(12, 10, 9);
}

void loop () {
  HT1632.clear();
  // This step only performs the drawing in internal memory. 
  HT1632.drawImage(IMG_SAILBOAT, IMG_SAILBOAT_WIDTH,  IMG_SAILBOAT_HEIGHT, i, 0);
  // If the image intersects with the end,
  if (i > OUT_SIZE - IMG_SAILBOAT_WIDTH) {
    // Draw it wrapping around.
    HT1632.drawImage(IMG_SAILBOAT, IMG_SAILBOAT_WIDTH,  IMG_SAILBOAT_HEIGHT, i - OUT_SIZE, 0);
  }
  HT1632.render(); // This updates the display on the screen.
  
  delay(100);
  
  i = (i + 1) % (OUT_SIZE);
}
