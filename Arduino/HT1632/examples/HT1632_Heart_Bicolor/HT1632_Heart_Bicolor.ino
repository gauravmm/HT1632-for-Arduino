#include <HT1632.h>
#include <font_5x4.h>
#include <images.h>

int i = 0;

void setup () {
  // Working with Bicolor displays.
  // Make sure that a bicolor display is set in HT1632.h  
  HT1632.begin(12, 10, 9);
}

void loop () {
  // Draws a heart that changes color every second:
  // This is a 4-frame animation, with frame number i in range [0..3]
  
  // Zero all data in all channels:
  HT1632.clear();
  
  if (i & 0b01) { // On frames 1 and 3:
    HT1632.selectChannel(0); // Select the first channel
    // Draw a heart:
    HT1632.drawImage(IMG_HEART, IMG_HEART_WIDTH,  IMG_HEART_HEIGHT, (OUT_SIZE - IMG_HEART_WIDTH)/2, 0);
  }
  
  if (i & 0b10) { // On frames 2 and 3:
    HT1632.selectChannel(1); // Select the second channel
    // Draw a heart:
    HT1632.drawImage(IMG_HEART, IMG_HEART_WIDTH,  IMG_HEART_HEIGHT, (OUT_SIZE - IMG_HEART_WIDTH)/2, 0);
  }
  
  HT1632.render(); // This sends the data in both channels to the screen.

  // Get the next number in the sequence, wrapping from 3 back to 0:
  i = (i + 1) & 0b11;
  
  delay(1000);  
}
