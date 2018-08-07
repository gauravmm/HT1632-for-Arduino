#include <HT1632.h>
#include <font_5x4.h>
#include <images.h>

int j = 0;

void setup () {
  // Working with Bicolor displays.
  // Make sure that a bicolor display is set in HT1632.h  
  HT1632.setCLK(13);
  HT1632.begin(12, 6, 10, 9);
}

void loop () {
  // Zero all data in all channels:
  HT1632.renderTarget(0);
  HT1632.clear();  

  HT1632.selectChannel(0); // Red heart on first screen:
  HT1632.drawImage(IMG_HEART, IMG_HEART_WIDTH,  IMG_HEART_HEIGHT, j - IMG_HEART_WIDTH, 3);

  HT1632.selectChannel(1); // Green heart on first screen:
  HT1632.drawImage(IMG_HEART, IMG_HEART_WIDTH,  IMG_HEART_HEIGHT, OUT_SIZE * 2 - j, 3);
  
  HT1632.render(); // This sends the data in both channels to the screen.

  HT1632.renderTarget(1);
  HT1632.clear();

  HT1632.selectChannel(0); // Red heart on second screen:
  HT1632.drawImage(IMG_HEART, IMG_HEART_WIDTH,  IMG_HEART_HEIGHT, j - IMG_HEART_WIDTH - OUT_SIZE, 3);

  HT1632.selectChannel(1); // Green heart on second screen:
  HT1632.drawImage(IMG_HEART, IMG_HEART_WIDTH,  IMG_HEART_HEIGHT, OUT_SIZE - j, 3);

  HT1632.render();

  // Get the next number in the sequence, wrapping from 3 back to 0:
  j = (j + 1) % (OUT_SIZE * 2);
  
  delay(50);  
}
