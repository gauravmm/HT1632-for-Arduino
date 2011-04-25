#include <font_5x4.h>
#include <HT1632.h>
#include <images.h>

int i = 0;
int wd;

void setup () {
  Serial.begin(9600);
  HT1632.begin(12, 13, 10, 9);
  
  
  /* 
  // Multiple screen control
  HT1632.drawTarget(BUFFER_BOARD(1));
  HT1632.drawText("Hello!", 0, 1, FONT_5X4, FONT_5X4_WIDTH, FONT_5X4_HEIGHT, FONT_5X4_STEP_GLYPH);
  HT1632.render();
  
  HT1632.drawTarget(BUFFER_SECONDARY);
  HT1632.drawText("Foo", 0, 1, FONT_5X4, FONT_5X4_WIDTH, FONT_5X4_HEIGHT, FONT_5X4_STEP_GLYPH);
  HT1632.render();
  
  HT1632.drawTarget(BUFFER_BOARD(2));
  HT1632.drawText("Bar", 0, 1, FONT_5X4, FONT_5X4_WIDTH, FONT_5X4_HEIGHT, FONT_5X4_STEP_GLYPH);
  HT1632.render();
  //*/
    
  /*
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
  
  //HT1632.transition(TRANSITION_FADE, 4000);
  //*/
  wd = HT1632.getTextWidth("Hello, how are you?", FONT_5X4_WIDTH, FONT_5X4_HEIGHT);
}

void loop () {
  
  // Font rendering example
  
  //  HT1632.transition(TRANSITION_BUFFER_SWAP);
  //  HT1632.render();
  
  //*
  HT1632.drawTarget(BUFFER_BOARD(1));
  HT1632.clear();
  HT1632.drawText("Hello, how are you?", 2*OUT_SIZE - i, 2, FONT_5X4, FONT_5X4_WIDTH, FONT_5X4_HEIGHT, FONT_5X4_STEP_GLYPH);
  HT1632.render();
  
  HT1632.drawTarget(BUFFER_BOARD(2));
  HT1632.clear();
  HT1632.drawText("Hello, how are you?", OUT_SIZE - i, 2, FONT_5X4, FONT_5X4_WIDTH, FONT_5X4_HEIGHT, FONT_5X4_STEP_GLYPH);
  HT1632.render();
  
  
  i = (i+1)%(wd + OUT_SIZE * 2);
  //*/
  
  /*
  // Simple rendering example
  HT1632.clear();
  HT1632.drawImage(i%2 ? IMG_SPEAKER_A:IMG_SPEAKER_B, IMG_SPEAKER_WIDTH,  IMG_SPEAKER_HEIGHT, 0, 0);
  HT1632.drawImage(IMG_MUSICNOTE, IMG_MUSICNOTE_WIDTH,  IMG_MUSICNOTE_HEIGHT, 8, (i)%2);
  HT1632.drawImage(IMG_MUSIC, IMG_MUSIC_WIDTH,  IMG_MUSIC_HEIGHT, 13, (i+1)%2);
  HT1632.drawImage(IMG_MUSICNOTE, IMG_MUSICNOTE_WIDTH,  IMG_MUSICNOTE_HEIGHT, 23, (i)%2);
  HT1632.drawImage(IMG_MUSICNOTE, IMG_MUSICNOTE_WIDTH,  IMG_MUSICNOTE_HEIGHT, 28, (i+1)%2);
  //HT1632.transition(TRANSITION_BUFFER_SWAP);
  Serial.write('\n');
  HT1632.render();
  ++i;
  //*/
  
  /*
  // Example of automatic clipping and data preservation.
  HT1632.drawImage(IMG_MAIL, IMG_MAIL_WIDTH,  IMG_MAIL_HEIGHT, 12, 0);
  HT1632.drawImage(IMG_MUSICNOTE, IMG_MUSICNOTE_WIDTH,  IMG_MUSICNOTE_HEIGHT, 12, i);
  HT1632.drawImage(IMG_MUSICNOTE, IMG_MUSICNOTE_WIDTH,  IMG_MUSICNOTE_HEIGHT, 18, -i);
  //*/
  
  delay(200);
}
