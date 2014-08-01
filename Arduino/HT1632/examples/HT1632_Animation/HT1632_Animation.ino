#include <HT1632.h>
#include <font_5x4.h>
#include <images.h>

int i = 0;

void setup () {
  HT1632.begin(12, 10, 9);
}

void loop () {
  // Clear the previous image contents:
  HT1632.clear();

  // Draw a different image based on the frame number:
  if(i++ % 2 == 0) {
    HT1632.drawImage(IMG_SPEAKER_A, IMG_SPEAKER_WIDTH,  IMG_SPEAKER_HEIGHT, 0, 0);
    HT1632.drawImage(IMG_MUSICNOTE, IMG_MUSICNOTE_WIDTH,  IMG_MUSICNOTE_HEIGHT, 8, 0);
    HT1632.drawImage(IMG_MUSIC, IMG_MUSIC_WIDTH,  IMG_MUSIC_HEIGHT, 13, 1);
    HT1632.drawImage(IMG_MUSICNOTE, IMG_MUSICNOTE_WIDTH,  IMG_MUSICNOTE_HEIGHT, 23, 0);
    HT1632.drawImage(IMG_MUSICNOTE, IMG_MUSICNOTE_WIDTH,  IMG_MUSICNOTE_HEIGHT, 28, 1);
  } else {
    HT1632.drawImage(IMG_SPEAKER_B, IMG_SPEAKER_WIDTH,  IMG_SPEAKER_HEIGHT, 0, 0);
    HT1632.drawImage(IMG_MUSICNOTE, IMG_MUSICNOTE_WIDTH,  IMG_MUSICNOTE_HEIGHT, 8, 1);
    HT1632.drawImage(IMG_MUSIC, IMG_MUSIC_WIDTH,  IMG_MUSIC_HEIGHT, 13, 0);
    HT1632.drawImage(IMG_MUSICNOTE, IMG_MUSICNOTE_WIDTH,  IMG_MUSICNOTE_HEIGHT, 23, 1);
    HT1632.drawImage(IMG_MUSICNOTE, IMG_MUSICNOTE_WIDTH,  IMG_MUSICNOTE_HEIGHT, 28, 0);
  }

  // Perform the drawing:
  HT1632.render();
  
  delay(200);
}
