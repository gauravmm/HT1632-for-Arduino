#include "HT1632.h"

#if (ARDUINO >= 100)
	#include <Arduino.h>
#else
	#include <WProgram.h>
#endif

/*
 * HIGH LEVEL FUNCTIONS
 * Functions that perform advanced tasks using lower-level
 * functions go here:
 */

void HT1632Class::drawText(const char text [], int x, int y, const char font [], const char font_width [], char font_height, int font_glyph_step, char gutter_space) {
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
		if(curr_x + font_width[currchar] + gutter_space >= 0){
			drawImage(font, font_width[currchar], font_height, curr_x, y,  currchar*font_glyph_step);
			
			// Draw the gutter space
			for(char j = 0; j < gutter_space; ++j)
			drawImage(font, 1, font_height, curr_x + font_width[currchar] + j, y, 0);
			
		}
		
		curr_x += font_width[currchar] + gutter_space;
		++i;
	}
}

// Gives you the width, in columns, of a particular string.
int HT1632Class::getTextWidth(const char text [], const char font_width [], char font_height, char gutter_space) {
	int wd = 0;
	char i = 0;
	char currchar;
	
	while(true){  
	if(text[i] == '\0')
		return wd - gutter_space;
		
	currchar = text[i] - 32;
	if(currchar >= 65 && currchar <= 90) // If character is lower-case, automatically make it upper-case
		currchar -= 32; // Make this character uppercase.
	if(currchar < 0 || currchar >= 64) { // If out of bounds, skip
		++i;
		continue; // Skip this character.
	}
	wd += font_width[currchar] + gutter_space;
	++i;
	}
}

/*
 * MID LEVEL FUNCTIONS
 * Functions that handle internal memory, initialize the hardware
 * and perform the rendering go here:
 */

void HT1632Class::begin(int pinCS1, int pinWR, int pinDATA) {
	_numActivePins = 1;
	_pinCS[0] = pinCS1;
	initialize(pinWR, pinDATA);
}
void HT1632Class::begin(int pinCS1, int pinCS2, int pinWR,   int pinDATA) {
	_numActivePins = 2;
	_pinCS[0] = pinCS1;
	_pinCS[1] = pinCS2;
	initialize(pinWR, pinDATA);
}
void HT1632Class::begin(int pinCS1, int pinCS2, int pinCS3,  int pinWR,   int pinDATA) {
	_numActivePins = 3;
	_pinCS[0] = pinCS1;
	_pinCS[1] = pinCS2;
	_pinCS[2] = pinCS3;
	initialize(pinWR, pinDATA);
}
void HT1632Class::begin(int pinCS1, int pinCS2, int pinCS3,  int pinCS4,  int pinWR,   int pinDATA) {
	_numActivePins = 4;
	_pinCS[0] = pinCS1;
	_pinCS[1] = pinCS2;
	_pinCS[2] = pinCS3;
	_pinCS[3] = pinCS4;
	initialize(pinWR, pinDATA);
}

void HT1632Class::initialize(int pinWR, int pinDATA) {
	_pinWR = pinWR;
	_pinDATA = pinDATA;
	
	for(int i=0; i<_numActivePins; ++i){
	pinMode(_pinCS[i], OUTPUT);
	// Allocate new memory for mem
	mem[i] = (char *)malloc(ADDR_SPACE_SIZE);
	drawTarget(i);
	clear(); // Clean out mem
	}
	pinMode(_pinWR, OUTPUT);
	pinMode(_pinDATA, OUTPUT);
	
	select();
	
	mem[4] = (char *)malloc(ADDR_SPACE_SIZE);
	// Each 8-bit mem array element stores data in the 4 least significant bits,
	//   and meta-data in the 4 most significant bits. Use bitmasking to read/write
	//   the meta-data.
	drawTarget(4);
	clear();
	// Clean out memory
	int i=0;
	
	
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
	
	if(false && _numActivePins > 1){
	select(0b0001); // 
	writeData(HT1632_ID_CMD, HT1632_ID_LEN);    // Command mode   
	writeCommand(HT1632_CMD_RCCLK); // Switch system to MASTER mode    
	select(0b1110); // All other boards are slaves
	writeData(HT1632_ID_CMD, HT1632_ID_LEN);    // Command mode
	
	writeCommand(HT1632_CMD_MSTMD); // Switch system to SLAVE mode.
	// The use of the MSTMD command to switch to slave is explained here:
	// http://forums.parallax.com/showthread.php?117423-Electronics-I-O-%28outputs-to-common-anode-RGB-LED-matrix%29-question/page4
	
	
	select(0b1111);
	writeData(HT1632_ID_CMD, HT1632_ID_LEN);    // Command mode
	}

	writeCommand(HT1632_CMD_SYSEN); //Turn on system
	writeCommand(HT1632_CMD_LEDON); // Turn on LED duty cycle generator
	writeCommand(HT1632_CMD_PWM(16)); // PWM 16/16 duty
	
	select();
	 
	
	for(int i=0; i<_numActivePins; ++i) {
	drawTarget(i);
	_globalNeedsRewriting[i] = true;
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

#IF PIXELS_PER_BYTE != 8
	#ERROR "The current drawImage implementation requires PIXELS_PER_BYTE == 8"
#ENDIF

void HT1632Class::drawImage(const uint8_t * img, uint8_t width, uint8_t height, int8_t x, int8_t y, int img_offset) {
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
			offset += bytesPerColumn;
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
			uint8_t copyData = img[img_offset + ] << (src_y & 0b111);
			// Shift data to match the destination place value.
			copyData = copyData >> (dst_y & 0b111);

			// Perform the copy
			mem[_tgtBuffer][GET_ADDR_FROM_X_Y(dst_x, dst_y)] =  // Put in destination
				(mem[_tgtBuffer][GET_ADDR_FROM_X_Y(dst_x, dst_y)] & ~dst_copyMask) | // All bits not in the mask from destination
				(copyData & dst_copyMask); // All bits in the mask from source

			src_y += copyInNextStep;
			dst_y += copyInNextStep;
			/* Copy bytes without alignment:
			 *          0       8       16
			 * To:      |-------|-------|
			 * From: |-------|-------|
			 *      -3       5       13
			 *       A  B    C  D    E
			 * 
			 */

		}

		src_x++;
		dst_x++;
	}
}

void HT1632Class::clear(){
	for(char i=0; i < ADDR_SPACE_SIZE; ++i)
	mem[_tgtBuffer][i] = 0x00 | MASK_NEEDS_REWRITING; // Needs to be redrawn 
}

// Draw the contents of map to screen, for memory addresses that have the needsRedrawing flag
void HT1632Class::render() {
	if(_tgtBuffer >= _numActivePins || _tgtBuffer < 0)
	return;
	
	char selectionmask = 0b0001 << _tgtBuffer;
	
	bool isOpen = false;                   // Automatically compact sequential writes.
	for(int i=0; i<ADDR_SPACE_SIZE; ++i)
	if(_globalNeedsRewriting[_tgtBuffer] || (mem[_tgtBuffer][i] & MASK_NEEDS_REWRITING)) {  // Does this memory chunk need to be written to?
		if(!isOpen) {                      // If necessary, open the writing session by:
			select(selectionmask);           //   Selecting the chip
			writeData(HT1632_ID_WR, HT1632_ID_LEN);
			writeData(i, HT1632_ADDR_LEN);   //   Selecting the memory address
			isOpen = true;
		}
		writeDataRev(mem[_tgtBuffer][i], HT1632_WORD_LEN); // Write the data in reverse.
	} else                               // If a previous sequential write session is open, close it.
		if(isOpen) {
		select();
		isOpen = false;
		}

	if(isOpen) {                           // Close the stream at the end
	select();
	isOpen = false;
	}
	_globalNeedsRewriting[_tgtBuffer] = false;
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

void HT1632Class::transition(char mode, int time){
	if(_tgtBuffer >= _numActivePins || _tgtBuffer < 0)
	return;
	
	switch(mode) {
	case TRANSITION_BUFFER_SWAP:
		{
		char * tmp = mem[_tgtBuffer];
		mem[_tgtBuffer] = mem[BUFFER_SECONDARY];
		mem[BUFFER_SECONDARY] = tmp;
		_globalNeedsRewriting[_tgtBuffer] = true;
		}
		break;
	case TRANSITION_NONE:
		for(char i=0; i < ADDR_SPACE_SIZE; ++i)
		mem[_tgtBuffer][i] = mem[BUFFER_SECONDARY][i]; // Needs to be redrawn 
		_globalNeedsRewriting[_tgtBuffer] = true;
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
void HT1632Class::writeData(char data, char len) {
	for(int j=len-1, t = 1 << (len - 1); j>=0; --j, t >>= 1){
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
// REVERSED Integer write to display. Used to write cell values.
// PRECONDITION: WR is LOW
void HT1632Class::writeDataRev(char data, char len) {
	for(int j=0; j<len; ++j){
	// Set the DATA pin to the correct state
	digitalWrite(_pinDATA, data & 1);
	NOP(); // Delay
	// Raise the WR momentarily to allow the device to capture the data
	digitalWrite(_pinWR, HIGH);
	NOP(); // Delay
	// Lower it again, in preparation for the next cycle.
	digitalWrite(_pinWR, LOW);
	data >>= 1;
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
	/*Serial.write(48+_pinCS[i]);
	Serial.write((t & mask)?"LOW":"HIGH");
	Serial.write('\n');
	*/}
	//Serial.write('\n');
}
void HT1632Class::select() {
	for(int i=0; i<_numActivePins; ++i)
	digitalWrite(_pinCS[i], HIGH);
}

/*
 * HELPER FUNCTIONS
 * "Would you like some fries with that?"
 */

void HT1632Class::recursiveWriteUInt (int inp) {
	if(inp <= 0) return;
	int rd = inp % 10;
	recursiveWriteUInt(inp/10);
	Serial.write(48+rd);
}

void HT1632Class::writeInt (int inp) {
	if(inp == 0)
	Serial.write('0');
	else
	if (inp < 0){
		Serial.write('-');
		recursiveWriteUInt(-inp);
	} else 
		recursiveWriteUInt(inp);
}

HT1632Class HT1632;

