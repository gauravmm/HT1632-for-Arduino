HT1632 for Arduino v2.0
=======================

__NOTE:__ This new version of the software changes the underlying data format - upgrading is easy and largely automated. See [upgrade instructions](UPGRADE.md).

This is a powerful library that allows an Arduino to interface with the popular __Holtek HT1632C__ LED driver. It allows programmers to directly perform advanced drawing of images and text with minimal code.


Quick Start
===========

Code
----

Here's some sample code to draw a blinking heart on the middle of a single screen:

```c
#include <HT1632.h> // Include this before <images.h> or any font.
#include <images.h>

void setup () {
	HT1632.begin(pinCS1, pinWR, pinDATA);
	// Where pinCS1, pinWR and pinDATA are the numbers of the output pins
	// that are connected to the appropriate pins on the HT1632.
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
```

Before running the code, go to HT1632.h and check the `USER OPTIONS` section and follow the instructions to specify the type of board you are using.


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

The editor provides the data in a ready-to-paste format that allows for quick drawing of fonts and/or images. It can load previously drawn images as well (even those from v1 -- see [upgrading instructions](UPGRADE.md) for more details).

It's use should be self-evident. You can find it in `Utilities/image_drawing.html`.

Font Creation
-------------

Fonts are defined as a series of images of equal height and variable width. (__Note:__ Currently, fonts are only tested to at most one byte per column.) The bytes making up each character, from ASCII char 32 (space) onwards, are appended together without any byte alignment.

An array, `FONT_8X4_END`, encodes information necessary to extract the width and the offset of the character from the font array. The element at position `i` encodes the first index of character `i + 1`.

__Generating FONT_NAME_NUM__

This array can be generated automatically using `Utilities/font_end_generate.html`. Make sure your font has one character per line (with no trailing newline) and paste it into the tool.

If you want to skip character `i`, simply set `FONT_NAME_NUM[i] = FONT_NAME_NUM[i - 1]` or `FONT_NAME_NUM[0] = 0` if `i == 0`. Using the `font_end_generate.html` tool, just leave a blank line.


Advanced Use
============

There are a few examples in `Arduino/HT1632/examples/`.

Bicolor Displays
----------------

This library natively supports Bicolor displays, using the `selectChannel(n)` function to switch to color channel `n` before rendering images. Calls to `clear()` and `render()` automatically operate on all channels.

An example is available in `HT1632/examples/HT1632_Heart_Bicolor/`.

__NOTE:__ You need to call `HT1632.setCLK(PIN_NUMBER_CLK);` before calling `begin()`, where `PIN_NUMBER_CLK` is the Arduino pin connected to the `CLK` of your Bicolor board. Refer to the example.

__NOTE:__ Make sure you have set the board type in `HT1632.h`. If you specify a wrong `NUM_CHANNEL` value, it won't work properly.

Brightness Control
------------------

The HT1632C comes with a 15-level PWM control option. You can control the brightness (from 1/16 to 16/16 of the duty cycle) of the current drawing target using the  `setBrightness(level)` function, where level is a number from 1 to 16. __level must never be zero!__

If you want to simultaneously set multiple boards to the same brightness level, you can pass a bitmask as an optional second argument, like so: `setBrightness(8, 0b0101)`. The rightmost bit is the first screen, while the fourth bit from the right corresponds to the fourth screen. In the above example, the first and third screen are set to half brightness, while the second and third remain unchanged.

Multiple HT1632s
----------------

This library supports up to 4 chips at a time (though technically more can be run concurrently). To take advantage of this, specify multiple CS pins in the initialization.

All rendering occurs on the first display by default. A scrolling text example is available in `HT1632/examples/HT1632_Text_8X4_Multidisplay/`.

Do note that all drawing happens to a single buffer. You need to `clear()` the contents of the buffer if drawing different graphics to different screens. To draw the same image to multiple screens, call `renderTarget()` and `render()` once per target.

Multiple HT1632s, each with multiple color channels, are supported natively.


Bugs & Features
===============

Known Issues
------------

1. Initialization doesn't automatically assign a single HT1632C as the RC Master - some unknown bug prevents this from working. As a result, multiple HT1632Cs only need the power and data pins connected, leaving the OSC and SYNC pins disconnected.

2. A single Arduino cannot support both a Mono-color and a Bi-color display.

Future Plans
------------

1. Support for direct pixel access and primitive drawing.
2. Test support for fonts taller than 8px.
3. Allow `FONT_8X4_END` arrays to be either 2-byte `int`s or 1-byte `uint8_t`s
