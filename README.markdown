HT1632 for Arduino v1.0
=======================

This is a powerful library that allows an Arduino to interface with the popular __Holtek HT1632C__ LED driver. It allows programmers to directly perform advanced drawing of images and text with minimal code, using a similar "state machine" paradigm to OpenGL.

Do note that the (now depreciated) __Holtek HT1632__ LED driver is *theoretically* compatible with this library, but requires slightly different initialization.

Quick Start
===========

Code
----

Here's some sample code to draw a blinking heart on the middle of a single screen.

```c++
#include <HT1632.h>
#include <images.h>

void setup () {
	HT1632.begin(pinCS1, pinWR, pinDATA);
	// Where pinCS1, pinWR and pinDATA are the numbers of the output pins
	// that are connected to the appropriate pins on the HT1632.
}

void loop () {
	HT1632.drawImage(IMG_HEART, IMG_HEART_WIDTH,  IMG_HEART_HEIGHT, (OUT_SIZE - IMG_HEART_WIDTH)/2, 0);
	// The definitions for IMG_HEART and its width and height are available in images.h.
	// This step only performs the drawing in internal memory. 
	HT1632.render();
	// This updates the display on the screen.
	
	delay(1000);
	
	HT1632.clear();
	// This zeroes out the internal memory.
	HT1632.render();
	// This updates the screen display.
	
	delay(1000);
}
```

Before running the code, go to HT1632.h and change the following definitions, if needed. This needs to be done only __once__.

```c++
// Size of COM and OUT in bits:
#define COM_SIZE 8
#define OUT_SIZE 32
// COM_SIZE MUST be either 8 or 16.

// Use N-MOS (if 1) or P-MOS (if 0):
#define USE_NMOS 1
```

The defaults shown here (and in the code) are suitable for the SureElectronics 3208 series display boards.

Explanation
-----------

HT1632 is always initialized by calling the begin function, like so:

```c++
HT1632.begin(pinCS1 [, pinCS2 [, pinCS3 [, pinCS4]]], pinWR, pinDATA);
```

All pins are set to `OUTPUT` and memory is allocated and cleared automatically. The square brackets denote an optional argument.

The HT1632 class stores an internal copy of the state of each screen used. __All__ drawing and writing functions operate on this internal memory, allowing you to perform complicated compositing. Once the internal memory is ready for display, the `render()` function will efficiently send the updated image to the screen.

Utilities
==========

Image Drawing
-------------

This project includes an image-drawing utility, written in HTML5 (using the canvas tag) and JavaScript. It has been tested on Firefox 3.6.* on OSX.

The editor provides the data in a ready-to-paste format that allows for quick drawing of fonts and/or images. It can load previously drawn images as well.

It's use should be self-evident. You can find it in "Utilities/Image drawing/". 

Advanced Use
============

Brightness Control
------------------

The HT1632C comes with a 15-level PWM control option. You can control the brightness (from 1/16 to 16/16 of the duty cycle) of the current drawing target using the  `setBrightness(level)` function, where level is a number from 1 to 16.
__level must never be zero!__

If you want to simultaneously set multiple boards to the same brightness level, you can pass a bitmask as an optional second argument, like so: `setBrightness(8, 0b0101)`. The rightmost bit is the first screen, while the fourth bit from the right corresponds to the fourth screen. In the above example, the first and third screen are set to half brightness, while the second and third remain unchanged.

Multiple HT1632s
----------------

This library supports up to 4 chips at a time (though technically more can be run concurrently). To take advantage of this, specify multiple CS pins in the initialization.

All drawing occurs on the first display by default. The `drawTarget(BUFFER_BOARD(x))` function allows you to choose to write output to the board selected by `pinCSx`. The example below shows how scrolling text can be implemented using this:

```c++
#include <font_5x4.h>
#include <HT1632.h>

int wd; 
int i = 0;

void setup () {
	HT1632.begin(pinCS1, pinCS2, pinWR, pinDATA);
	wd = HT1632.getTextWidth("Hello, how are you?", FONT_5X4_WIDTH, FONT_5X4_HEIGHT);
}

void loop () {
	// Select board 1 as the target of subsequent drawing/rendering operations.
	HT1632.drawTarget(BUFFER_BOARD(1));
	HT1632.clear();
	
	HT1632.drawText("Hello, how are you?", 2*OUT_SIZE - i, 2,
		FONT_5X4, FONT_5X4_WIDTH, FONT_5X4_HEIGHT, FONT_5X4_STEP_GLYPH);
	
	HT1632.render(); // Board 1's contents is updated.
	
	// Select board 2 as the target of subsequent drawing/rendering operations.
	HT1632.drawTarget(BUFFER_BOARD(2));
	HT1632.clear();
	
	HT1632.drawText("Hello, how are you?", OUT_SIZE - i, 2,
		FONT_5X4, FONT_5X4_WIDTH, FONT_5X4_HEIGHT, FONT_5X4_STEP_GLYPH);
	
	HT1632.render(); // Board 2's contents is updated.
	
	i = (i+1)%(wd + OUT_SIZE * 2); // Make it repeating.
}
```

Secondary Buffer
----------------

In addition to one buffer for each board, one additional buffer (the secondary buffer) is provided. It acts like a drawing target in every way, except that it cannot be rendered. `drawTarget(BUFFER_SECONDARY)` function call allows you to select this buffer.

*This buffer is currently of limited use. Future expansions plan to use this for transitions.*

Here's an example showing this buffer being used to render a two-frame animation much more efficiently than redrawing each frame from scratch:


```c++
#include <images.h>
#include <HT1632.h>

void setup () {
	HT1632.begin(pinCS1, pinWR, pinDATA);
		
	// Draw one image on the board buffer.
	HT1632.drawTarget(BUFFER_BOARD(1)); // This line is unnecessary, this is the default draw target. 
	HT1632.drawImage(IMG_SPEAKER_A, IMG_SPEAKER_WIDTH, IMG_SPEAKER_HEIGHT, 0, 0);
	HT1632.drawImage(IMG_MUSICNOTE, IMG_MUSICNOTE_WIDTH, IMG_MUSICNOTE_HEIGHT, 8, 0);
	HT1632.drawImage(IMG_MUSIC, IMG_MUSIC_WIDTH, IMG_MUSIC_HEIGHT, 13, 1);
	HT1632.drawImage(IMG_MUSICNOTE, IMG_MUSICNOTE_WIDTH, IMG_MUSICNOTE_HEIGHT, 23, 0);
	HT1632.drawImage(IMG_MUSICNOTE, IMG_MUSICNOTE_WIDTH, IMG_MUSICNOTE_HEIGHT, 28, 1);
	
	// Draw another image on the secondary buffer.
	HT1632.drawTarget(BUFFER_SECONDARY);
	HT1632.drawImage(IMG_SPEAKER_B, IMG_SPEAKER_WIDTH, IMG_SPEAKER_HEIGHT, 0, 0);
	HT1632.drawImage(IMG_MUSICNOTE, IMG_MUSICNOTE_WIDTH, IMG_MUSICNOTE_HEIGHT, 8, 1);
	HT1632.drawImage(IMG_MUSIC, IMG_MUSIC_WIDTH, IMG_MUSIC_HEIGHT, 13, 0);
	HT1632.drawImage(IMG_MUSICNOTE, IMG_MUSICNOTE_WIDTH, IMG_MUSICNOTE_HEIGHT, 23, 1);
	HT1632.drawImage(IMG_MUSICNOTE, IMG_MUSICNOTE_WIDTH, IMG_MUSICNOTE_HEIGHT, 28, 0);
	
	HT1632.drawTarget(BUFFER_BOARD(1));
	HT1632.render(); // Render the initial image.
}

void loop () {
	delay(500);
	
	HT1632.transition(TRANSITION_BUFFER_SWAP);
	HT1632.render();
}
```
Notice that all the drawing is done in the setup() function? The loop function just shuffles the data around in memory.

Note: Only three transitions are currently available. 
<table>
  <tr>
    <th>`TRANSITION_BUFFER_SWAP`</th><td>Swap the current buffer and the transition buffer. This is the only transition that preserves the contents of the current buffer.</td>
  </tr>
  <tr>
    <th>`TRANSITION_NONE`</th><td>Simply copy the buffer.</td>
  </tr>
  <tr>
    <th>`TRANSITION_FADE`</th><td>Uses the PWM feature to fade through black. Does not preserve current brightness level.</td>
  </tr>
</table>

Bugs & Features
===============

Known Issues
------------

1. Initialization doesn't automatically assign a single HT1632C as the RC Master - some unknown bug prevents this from working. As a result, multiple HT1632Cs only need the power and data pins connected, leaving the OSC and SYNC pins disconnected.

Future Plans
------------

1. Support for direct pixel access and primitive drawing.
2. Support for advanced transitions (moving entire screen contents around with a single command).
3. "Export" feature that transmits the screen contents over Serial, so that animations can be captured in realtime.

