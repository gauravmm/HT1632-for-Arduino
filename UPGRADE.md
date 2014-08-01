Upgrade Instruction from v1.0
=============================

Why bother?
-----------

Because:
 - Images, fonts and buffers are smaller, with a minimum 50% reduction in size.
 - Drawing is faster, the inner loop of the copying engine has been rewritten.
 - Bugfixes, notably the ability to copy images of any arbitrary size.
 - Compiled code size.

For example, the 8x4 font has been compressed from 640 bytes to 237 bytes!


How to upgrade
--------------

__Structure__

A single buffer is used for all compositing operations. The following functions, macros and constants have been removed:

```c
void HT1632.transition(uint8_t mode, int time = 1000); // Removed

void HT1632.drawTarget(uint8_t targetBuffer); // Replaced by:
void HT1632.renderTarget(uint8_t targetBuffer);

BUFFER_BOARD(n) // Just use:
(n - 1)         // Board number of pin to use.

SECONDARY_BUFFER // Removed
```

New mechanisms for working with color channels have been added, and are documented in [the README](README.md)

__Code__

1. You need to include `HT1632.h` before `images.h` or any font file.
2. The newer functions require the images/fonts to be updated.
3. All calls to `HT1632.drawText` need to have the `font_glyph_step` argument removed.
4. All references to `FONT_NAME_WIDTH` need to be changed to `FONT_NAME_END`.

__Images__

Your images need to be updated. The upgrade tool is built into the image drawing utility in `Utilities/image_drawing.html`. To use it:

1. Copy the source array of the image (everything between `{` and `}`).
2. Open `image_drawing.html`.
3. Click `Version 1.0`.
4. Change the width and height values to match your image.
5. Paste the source array.
6. Click `Version 2.0`.
7. Copy the new array and replace the old version in your source code.

__Fonts__

Your fonts need to be updated. The upgrade tool is in `Utilities/font_conv.html`, and should be self-explanatory. Do note that this tool is experimental - if you have trouble converting a particular font file, raise an issue on GitHub.

The new font format is described in the [README](README.md).
