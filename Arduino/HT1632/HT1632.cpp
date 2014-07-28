#include "HT1632.h"

/*
 * HIGH LEVEL FUNCTIONS
 * Functions that perform advanced tasks using lower-level
 * functions go here:
 */

void HT1632Class::drawText(char text [], int x, int y, byte font [], int font_end [], uint8_t font_height, uint8_t gutter_space) {
	int curr_x = x;
	char i = 0;
	char currchar;
	
	// Check if string is within y-bounds
	if(y + font_height < 0 || y >= COM_SIZE)
	return;
	
	while(true){  
		if(text[i] == '\0')
			return;
		
		currchar = text[i] - 32;
		if(currchar >= 65 && currchar <= 90) // If character is lower-case, automatically make it upper-case
			currchar -= 32; // Make this character uppercase.

		if(currchar < 0 || currchar >= 64) { // If out of bounds, skip
			++i;
			continue; // Skip this character.
		}

		// Check to see if character is not too far right.
		if(curr_x >= OUT_SIZE)
			break; // Stop rendering - all other characters are no longer within the screen 
		
		// Check to see if character is not too far left.
		int chr_width = getCharWidth(font_end, currchar);
		if(curr_x + chr_width + gutter_space >= 0){
			drawImage(font, chr_width, font_height, curr_x, y,  getCharOffset(font_end, currchar));
			
			// Draw the gutter space
			for(char j = 0; j < gutter_space; ++j)
			drawImage(font, 1, font_height, curr_x + chr_width + j, y, 0);
		}
		
		curr_x += chr_width + gutter_space;
		++i;
	}
}

// Gives you the width, in columns, of a particular string.
int HT1632Class::getTextWidth(char text [], int font_end [], uint8_t gutter_space) {
	int wd = 0;
	char i = 0;
	char currchar;
	
	while(true){  
		if (text[i] == '\0') {
			return wd - gutter_space;
		}
			
		currchar = text[i] - 32;
		if (currchar >= 65 && currchar <= 90) { // If character is lower-case, automatically make it upper-case
			currchar -= 32;                     // Make this character uppercase.
		}

		if (currchar < 0 || currchar >= 64) {   // If out of bounds, skip
			++i;
			continue; // Skip this character.
		}

		wd += getCharWidth(font_end, currchar) + gutter_space;
		++i;
	}
}

int HT1632Class::getCharWidth(int font_end [], uint8_t font_index) {
	if(font_index == 0) {
		return font_end[0];
	}
	// The width is the difference between the ending index of
	//  this and the previous characters:
	return font_end[font_index] - font_end[font_index - 1];
}

int HT1632Class::getCharOffset(int font_end [], uint8_t font_index) {
	if(font_index == 0) {
		return 0;
	}
	// The offset is in the ending index of the previous character:
	return font_end[font_index - 1];
}

/*
 * MID LEVEL FUNCTIONS
 * Functions that handle internal memory, initialize the hardware
 * and perform the rendering go here:
 */

void HT1632Class::begin(uint8_t pinCS1, uint8_t pinWR, uint8_t pinDATA) {
	_numActivePins = 1;
	_pinCS[0] = pinCS1;
	initialize(pinWR, pinDATA);
}
void HT1632Class::begin(uint8_t pinCS1, uint8_t pinCS2, uint8_t pinWR,   uint8_t pinDATA) {
	_numActivePins = 2;
	_pinCS[0] = pinCS1;
	_pinCS[1] = pinCS2;
	initialize(pinWR, pinDATA);
}
void HT1632Class::begin(uint8_t pinCS1, uint8_t pinCS2, uint8_t pinCS3,  uint8_t pinWR,   uint8_t pinDATA) {
	_numActivePins = 3;
	_pinCS[0] = pinCS1;
	_pinCS[1] = pinCS2;
	_pinCS[2] = pinCS3;
	initialize(pinWR, pinDATA);
}
void HT1632Class::begin(uint8_t pinCS1, uint8_t pinCS2, uint8_t pinCS3,  uint8_t pinCS4,  uint8_t pinWR,   uint8_t pinDATA) {
	_numActivePins = 4;
	_pinCS[0] = pinCS1;
	_pinCS[1] = pinCS2;
	_pinCS[2] = pinCS3;
	_pinCS[3] = pinCS4;
	initialize(pinWR, pinDATA);
}

void HT1632Class::initialize(uint8_t pinWR, uint8_t pinDATA) {
	_pinWR = pinWR;
	_pinDATA = pinDATA;
	
	for(uint8_t i=0; i<_numActivePins; ++i){
		pinMode(_pinCS[i], OUTPUT);
		// Allocate new memory for mem
		mem[i] = (byte *)malloc(ADDR_SPACE_SIZE);
		drawTarget(i);
		clear(); // Clean out mem
	}

	pinMode(_pinWR, OUTPUT);
	pinMode(_pinDATA, OUTPUT);
	
	select();
	
	mem[4] = (byte *)malloc(ADDR_SPACE_SIZE);
	drawTarget(4);
	clear();

	// Send configuration to chip:
	// This configuration is from the HT1632 datasheet, with one modification:
	//   The RC_MASTER_MODE command is not sent to the master. Since acting as
	//   the RC Master is the default behaviour, this is not needed. Sending
	//   this command causes problems in HT1632C (note the C at the end) chips. 
	
	// Send Master commands
	
	select(0b1111); // Assume that board 1 is the master.
	writeData(HT1632_ID_CMD, HT1632_ID_LEN);    // Command mode
	
	writeCommand(HT1632_CMD_SYSDIS); // Turn off system oscillator
	 // N-MOS or P-MOS open drain output and 8 or 16 common option
#if   USE_NMOS == 1 && COM_SIZE == 8
	writeCommand(HT1632_CMD_COMS00);
#elif USE_NMOS == 1 && COM_SIZE == 16
	writeCommand(HT1632_CMD_COMS01);
#elif USE_NMOS == 0 && COM_SIZE == 8
	writeCommand(HT1632_CMD_COMS10);
#elif USE_NMOS == 0 && COM_SIZE == 16
	writeCommand(HT1632_CMD_COMS11);
#else
#error Invalid USE_NMOS or COM_SIZE values! Change the values in HT1632.h.
#endif

	writeCommand(HT1632_CMD_SYSEN); //Turn on system
	writeCommand(HT1632_CMD_LEDON); // Turn on LED duty cycle generator
	writeCommand(HT1632_CMD_PWM(16)); // PWM 16/16 duty
	
	select();
	
	for(uint8_t i = 0; i < _numActivePins; ++i) {
		drawTarget(i);
		clear();
		// Perform the initial rendering
		render();
	}
	// Set drawTarget to default board.
	drawTarget(0);
}

void HT1632Class::drawTarget(uint8_t targetBuffer) {
	if(targetBuffer == 0x04 || (targetBuffer >= 0 && targetBuffer < _numActivePins))  
	_tgtBuffer = targetBuffer;
}

#if PIXELS_PER_BYTE != 8
	#error "The current drawImage implementation requires PIXELS_PER_BYTE == 8"
#endif

void HT1632Class::drawImage(byte * img, uint8_t width, uint8_t height, int8_t x, int8_t y, int img_offset) {
	uint8_t bytesPerColumn = height/PIXELS_PER_BYTE;
	// Equivalent to taking Math.ceil(), without working with floats
	if (bytesPerColumn * PIXELS_PER_BYTE < height)
		bytesPerColumn++;

	// Sanity checks
	if(y + height < 0 || x + width < 0 || y > COM_SIZE || x > OUT_SIZE)
		return;
	// After looking at the rest of this function, you may need one.
	
	// Copying Engine.

	// Current off
	int8_t dst_x = x;
	int8_t src_x = 0;
	// Repeat until each column has been copied.
	while (src_x < width) {
		if(dst_x < 0) {
			// Skip this column if it is too far to the left.
			src_x++;
			dst_x++;
			continue;
		} else if (dst_x >= OUT_SIZE) {
			// End the copy if it is too far to the right.
			break;
		}

		int8_t src_y = 0;
		int8_t dst_y = y;
		while (src_y < height) {
			if (dst_y < 0) {
				// Skip pixels if the starting point to too far up.
				src_y -= dst_y;
				dst_y = 0;
				continue;
			} else if (dst_y >= COM_SIZE) {
				// End copying this column if it is too far down
				break;
			}

			// The use of bitmasking here assumes that PIXELS_PER_BYTE == 8

			// Find out how many we can copy in the next step:
			//  as a minimum of the number of bits in the current byte of source
			//  and destination.
			uint8_t copyInNextStep = 8 - max((src_y & 0b111), (dst_y & 0b111));

			// Limit this by the height of the image:
			copyInNextStep = min(copyInNextStep, (height - src_y));

			// Prepare the bitmask with the number of bits that need to be copied.
			uint8_t dst_copyMask = (0b1 << copyInNextStep) - 1;
			
			// Shift the bitmasks to the correct position.
			dst_copyMask <<= (8 - (dst_y & 0b111) - copyInNextStep);

			// Shift the data to the bits of highest significance
			uint8_t copyData = img[img_offset + (bytesPerColumn * src_x) + (src_y >> 3)] << (src_y & 0b111);
			// Shift data to match the destination place value.
			copyData >>= (dst_y & 0b111);

			// Perform the copy
			mem[_tgtBuffer][GET_ADDR_FROM_X_Y(dst_x, dst_y)] =  // Put in destination
				(mem[_tgtBuffer][GET_ADDR_FROM_X_Y(dst_x, dst_y)] & ~dst_copyMask) | // All bits not in the mask from destination
				(copyData & dst_copyMask); // All bits in the mask from source

			src_y += copyInNextStep;
			dst_y += copyInNextStep;
		}

		src_x++;
		dst_x++;
	}
}

void HT1632Class::clear(){
	for(char i=0; i < ADDR_SPACE_SIZE; ++i)
	mem[_tgtBuffer][i] = 0x00; // Needs to be redrawn
}

// Draw the contents of map to screen, for memory addresses that have the needsRedrawing flag
void HT1632Class::render() {
	if(_tgtBuffer >= _numActivePins || _tgtBuffer < 0) {
		return;
	}
	
	select(0b0001 << _tgtBuffer); // Selecting the chip
	
	writeData(HT1632_ID_WR, HT1632_ID_LEN);
	writeData(0, HT1632_ADDR_LEN); // Selecting the memory address

	for(uint8_t i = 0; i < ADDR_SPACE_SIZE; ++i) {
		// Write the higher bits before the the lower bits.
		writeData(mem[_tgtBuffer][i] >> HT1632_WORD_LEN, HT1632_WORD_LEN); // Write the data in reverse.
		writeData(mem[_tgtBuffer][i], HT1632_WORD_LEN); // Write the data in reverse.
	}

	select(); // Close the stream at the end
}

// Set the brightness to an integer level between 1 and 16 (inclusive).
// Uses the PWM feature to set the brightness.
void HT1632Class::setBrightness(char brightness, char selectionmask) {
	if(selectionmask == 0b00010000) {
	if(_tgtBuffer < _numActivePins)
		selectionmask = 0b0001 << _tgtBuffer;
	else
		return;
	}
	
	select(selectionmask); 
	writeData(HT1632_ID_CMD, HT1632_ID_LEN);    // Command mode
	writeCommand(HT1632_CMD_PWM(brightness));   // Set brightness
	select();
}

void HT1632Class::transition(uint8_t mode, int time){
	if(_tgtBuffer >= _numActivePins || _tgtBuffer < 0)
	return;
	
	switch(mode) {
	case TRANSITION_BUFFER_SWAP:
		{
			byte * tmp = mem[_tgtBuffer];
			mem[_tgtBuffer] = mem[BUFFER_SECONDARY];
			mem[BUFFER_SECONDARY] = tmp;
		}
		break;
	case TRANSITION_NONE:
		for(char i=0; i < ADDR_SPACE_SIZE; ++i)
			mem[_tgtBuffer][i] = mem[BUFFER_SECONDARY][i]; // Needs to be redrawn 
		break;
	case TRANSITION_FADE:
		time /= 32;
		for(int i = 15; i > 0; --i) {
			setBrightness(i);
			delay(time);
		}
		clear();
		render();
		delay(time);
		transition(TRANSITION_BUFFER_SWAP);
		render();
		delay(time);
		for(int i = 2; i <= 16; ++i) {
			setBrightness(i);
			delay(time);
		}
		break;
	}
	
}


/*
 * LOWER LEVEL FUNCTIONS
 * Functions that directly talk to hardware go here:
 */
 
void HT1632Class::writeCommand(char data) {
	writeData(data, HT1632_CMD_LEN);
	writeSingleBit();
} 
// Integer write to display. Used to write commands/addresses.
// PRECONDITION: WR is LOW
void HT1632Class::writeData(byte data, uint8_t len) {
	for(int j = len - 1, t = 1 << (len - 1); j >= 0; --j, t >>= 1){
		// Set the DATA pin to the correct state
		digitalWrite(_pinDATA, ((data & t) == 0)?LOW:HIGH);
		NOP(); // Delay 
		// Raise the WR momentarily to allow the device to capture the data
		digitalWrite(_pinWR, HIGH);
		NOP(); // Delay
		// Lower it again, in preparation for the next cycle.
		digitalWrite(_pinWR, LOW);
	}
}

// Write single bit to display, used as padding between commands.
// PRECONDITION: WR is LOW
void HT1632Class::writeSingleBit() {
	// Set the DATA pin to the correct state
	digitalWrite(_pinDATA, LOW);
	NOP(); // Delay
	// Raise the WR momentarily to allow the device to capture the data
	digitalWrite(_pinWR, HIGH);
	NOP(); // Delay
	// Lower it again, in preparation for the next cycle.
	digitalWrite(_pinWR, LOW);
}
// Choose a chip. This function sets the correct CS line to LOW, and the rest to HIGH
// Call the function with no arguments to deselect all chips.
// Call the function with a bitmask (0b4321) to select specific chips. 0b1111 selects all. 
void HT1632Class::select(char mask) {
	for(int i=0, t=1; i<_numActivePins; ++i, t <<= 1){
		digitalWrite(_pinCS[i], (t & mask)?LOW:HIGH);
	}
}
void HT1632Class::select() {
	for(int i=0; i<_numActivePins; ++i) {
		digitalWrite(_pinCS[i], HIGH);
	}
}

HT1632Class HT1632;
