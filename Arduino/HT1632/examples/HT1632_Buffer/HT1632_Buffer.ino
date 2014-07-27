#include <HT1632.h>
#include <font_5x4.h>
#include <images.h>

int i = 0;
int wd;

void setup () {
  HT1632.begin(12, 10, 9);
  
  // Buffer swap transition example.
  // Fill board buffer with one image
  HT1632.drawTarget(BUFFER_BOARD(1));
  HT1632.drawImage(IMG_SPEAKER_A, IMG_SPEAKER_WIDTH,  IMG_SPEAKER_HEIGHT, 0, 0);
  HT1632.drawImage(IMG_MUSICNOTE, IMG_MUSICNOTE_WIDTH,  IMG_MUSICNOTE_HEIGHT, 8, 0);
  HT1632.drawImage(IMG_MUSIC, IMG_MUSIC_WIDTH,  IMG_MUSIC_HEIGHT, 13, 1);
  HT1632.drawImage(IMG_MUSICNOTE, IMG_MUSICNOTE_WIDTH,  IMG_MUSICNOTE_HEIGHT, 23, 0);
  HT1632.drawImage(IMG_MUSICNOTE, IMG_MUSICNOTE_WIDTH,  IMG_MUSICNOTE_HEIGHT, 28, 1);
  
  // Fill secondary buffer with another image
  HT1632.drawTarget(BUFFER_SECONDARY);
  HT1632.drawImage(IMG_SPEAKER_B, IMG_SPEAKER_WIDTH,  IMG_SPEAKER_HEIGHT, 0, 0);
  HT1632.drawImage(IMG_MUSICNOTE, IMG_MUSICNOTE_WIDTH,  IMG_MUSICNOTE_HEIGHT, 8, 1);
  HT1632.drawImage(IMG_MUSIC, IMG_MUSIC_WIDTH,  IMG_MUSIC_HEIGHT, 13, 0);
  HT1632.drawImage(IMG_MUSICNOTE, IMG_MUSICNOTE_WIDTH,  IMG_MUSICNOTE_HEIGHT, 23, 1);
  HT1632.drawImage(IMG_MUSICNOTE, IMG_MUSICNOTE_WIDTH,  IMG_MUSICNOTE_HEIGHT, 28, 0);
  
  HT1632.drawTarget(BUFFER_BOARD(1));
  
  HT1632.render();
}

void loop () {
  HT1632.transition(TRANSITION_BUFFER_SWAP);
  HT1632.render();
  
  delay(200);
}
