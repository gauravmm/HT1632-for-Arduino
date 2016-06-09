#include "HT1632.h"

#if PIXELS_PER_BYTE != 8
	#error "The current drawImage, drawText and getTextWidth implementation requires PIXELS_PER_BYTE == 8"
#endif

/*
 * HIGH LEVEL FUNCTIONS
 * Functions that perform advanced tasks using lower-level
 * functions go here:
 */

void HT1632Class::drawText(const char text [], int x, int y, const byte font [], int font_end [], uint8_t font_height, uint8_t gutter_space) {
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
		int chr_width = getCharWidth(font_end, font_height, currchar);
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
int HT1632Class::getTextWidth(const char text [], int font_end [], uint8_t font_height, uint8_t gutter_space) {
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

		wd += getCharWidth(font_end, font_height, currchar) + gutter_space;
		++i;
	}
}

int HT1632Class::getCharWidth(int font_end [], uint8_t font_height, uint8_t font_index) {
	uint8_t bytesPerColumn = (font_height >> 3) + ((font_height & 0b111)?1:0); // Assumes that PIXELS_PER_BYTE is 8

	if(font_index == 0) {
		return font_end[0];
	}
	// The width is the difference between the ending index of
	//  this and the previous characters:
	return (font_end[font_index] - font_end[font_index - 1])/bytesPerColumn;
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
	
	for (uint8_t i = 0; i < _numActivePins; ++i){
		pinMode(_pinCS[i], OUTPUT);
	}

	pinMode(_pinWR, OUTPUT);
	pinMode(_pinDATA, OUTPUT);
	
	select();
	
	for (uint8_t i = 0; i < NUM_CHANNEL; ++i) {
		// Allocate new memory for each channel
		mem[i] = (byte *)malloc(ADDR_SPACE_SIZE);
	}
	// Clear all memory
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
	
	// Custom initialization from each:
#if defined TYPE_3208_MONO
	writeCommand(HT1632_CMD_COMS00);
#elif defined TYPE_3216_BICOLOR
	writeCommand(HT1632_CMD_COMS00);
	writeCommand(HT1632_CMD_RCCLK);  // Master Mode, external clock
#elif defined TYPE_2416_MONO
	writeCommand(HT1632_CMD_COMS01);
#else
	writeCommand(HT1632_CMD_COMS00);
#endif

	writeCommand(HT1632_CMD_SYSEN); //Turn on system
	writeCommand(HT1632_CMD_LEDON); // Turn on LED duty cycle generator
	writeCommand(HT1632_CMD_PWM(16)); // PWM 16/16 duty
	
	select();
	
	// Clear all screens by default:
	for(uint8_t i = 0; i < _numActivePins; ++i) {
		renderTarget(i);
		render();
	}
	// Set renderTarget to the first board.
	renderTarget(0);
}

void HT1632Class::selectChannel(uint8_t channel) {
	if(channel < NUM_CHANNEL) {
		_tgtChannel = channel;
	}
}

void HT1632Class::renderTarget(uint8_t target) {
	if(target < _numActivePins) {
		_tgtRender = target;
	}
}

void HT1632Class::drawImage(const byte * img, uint8_t width, uint8_t height, int8_t x, int8_t y, int img_offset) {
	// Assuming that we are using 8 PIXELS_PER_BYTE, this does the equivalent of Math.ceil(height/PIXELS_PER_BYTE):
	uint8_t bytesPerColumn = (height >> 3) + ((height & 0b111)?1:0);

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
			uint8_t copyData = pgm_read_byte(&img[img_offset + (bytesPerColumn * src_x) + (src_y >> 3)]) << (src_y & 0b111);
			// Shift data to match the destination place value.
			copyData >>= (dst_y & 0b111);

			// Perform the copy
			mem[_tgtChannel][GET_ADDR_FROM_X_Y(dst_x, dst_y)] =  // Put in destination
				(mem[_tgtChannel][GET_ADDR_FROM_X_Y(dst_x, dst_y)] & ~dst_copyMask) | // All bits not in the mask from destination
				(copyData & dst_copyMask); // All bits in the mask from source

			src_y += copyInNextStep;
			dst_y += copyInNextStep;
		}

		src_x++;
		dst_x++;
	}
}

void HT1632Class::setPixel(uint8_t x, uint8_t y) {
	if( x < 0 || x > OUT_SIZE || y < 0 || y > COM_SIZE )
		return;
	mem[_tgtChannel][GET_ADDR_FROM_X_Y(x, y)] |= (0b1 << PIXELS_PER_BYTE-1) >> (y % PIXELS_PER_BYTE);
}
void HT1632Class::clearPixel(uint8_t x, uint8_t y) {
	if( x < 0 || x > OUT_SIZE || y < 0 || y > COM_SIZE )
		return;
	mem[_tgtChannel][GET_ADDR_FROM_X_Y(x, y)] &= ~((0b1 << PIXELS_PER_BYTE-1) >> (y % PIXELS_PER_BYTE));
}
uint8_t HT1632Class::getPixel(uint8_t x, uint8_t y) {
	if( x < 0 || x > OUT_SIZE || y < 0 || y > COM_SIZE )
		return 0;
	return mem[_tgtChannel][GET_ADDR_FROM_X_Y(x, y)] & (0b1 << PIXELS_PER_BYTE-1) >> (y % PIXELS_PER_BYTE);
}

void HT1632Class::setPixel(uint8_t x, uint8_t y, uint8_t channel) {
	if( x < 0 || x > OUT_SIZE || y < 0 || y > COM_SIZE )
		return;
	mem[channel][GET_ADDR_FROM_X_Y(x, y)] |= GET_BIT_FROM_Y(y);
}
void HT1632Class::clearPixel(uint8_t x, uint8_t y, uint8_t channel) {
	if( x < 0 || x > OUT_SIZE || y < 0 || y > COM_SIZE )
		return;
	mem[channel][GET_ADDR_FROM_X_Y(x, y)] &= ~(GET_BIT_FROM_Y(y));
}
uint8_t HT1632Class::getPixel(uint8_t x, uint8_t y, uint8_t channel) {
	if( x < 0 || x > OUT_SIZE || y < 0 || y > COM_SIZE )
		return 0;
	return mem[channel][GET_ADDR_FROM_X_Y(x, y)] & GET_BIT_FROM_Y(y);
}

void HT1632Class::fill() {
	for(uint8_t i = 0; i < ADDR_SPACE_SIZE; ++i) {
		mem[_tgtChannel][i] = 0xFF;
	}
}
void HT1632Class::fillAll() {
	for(uint8_t c = 0; c < NUM_CHANNEL; ++c) {
		for(uint8_t i = 0; i < ADDR_SPACE_SIZE; ++i) {
			mem[c][i] = 0xFF; // Needs to be redrawn
		}
	}
}

void HT1632Class::clear(){
	for(uint8_t c = 0; c < NUM_CHANNEL; ++c) {
		for(uint8_t i = 0; i < ADDR_SPACE_SIZE; ++i) {
			mem[c][i] = 0x00; // Needs to be redrawn
		}
	}
}

#if defined TYPE_3216_BICOLOR
// Draw the contents of mem
void HT1632Class::render() {
	if(_tgtRender >= _numActivePins) {
		return;
	}

	// Write chip-by-chip:
	uint8_t _pinForCS = _pinCS[_tgtRender];
	
	for (uint8_t nChip = 0; nChip < NUM_ACTIVE_CHIPS; ++nChip) {
		// Select a single sub-chip:
		digitalWrite(_pinForCS, HIGH);
		for(uint8_t tmp = 0; tmp < NUM_ACTIVE_CHIPS; tmp++){
			if (tmp == nChip) {
				digitalWrite(_pinForCS, LOW);
				pulseCLK();
				digitalWrite(_pinForCS, HIGH);
			} else {
				pulseCLK();
			}
		}
		
		// Output data!
		writeData(HT1632_ID_WR, HT1632_ID_LEN);
		writeData(0, HT1632_ADDR_LEN); // Selecting the memory address

		// Write the channels in order
		for(uint8_t c = 0; c < NUM_CHANNEL; ++c) {
			//for(uint8_t i = (nChip & 0b1)?0:(ADDR_SPACE_SIZE >> 1); i < (nChip & 0b1)?(ADDR_SPACE_SIZE >> 1):ADDR_SPACE_SIZE; ++i) {
			uint8_t i, iMax;

			if(nChip & 0b1) { // If we're writing to the chips on the left
				i = 0; // Start from zero
				iMax = ADDR_SPACE_SIZE/2; // Stop at the halfway point.
			} else { // If we're writing to the chips on the right
				i = ADDR_SPACE_SIZE/2; // Start from the halfway point.
				iMax = ADDR_SPACE_SIZE; // Stop at the end of the buffer.
			}

			// If we are not (top-row chip)
			if(!(nChip & 0b10)) {
				++i; // Write only odd-numbered bytes.
			}

			for(; i < iMax; i+=2) { // Write every other byte.
				// Write the higher bits before the the lower bits.
				writeData(mem[c][i] >> HT1632_WORD_LEN, HT1632_WORD_LEN);
				writeData(mem[c][i], HT1632_WORD_LEN);
			}
		}
	}
}
#elif defined TYPE_3208_MONO
// Draw the contents of mem
void HT1632Class::render() {
	if(_tgtRender >= _numActivePins) {
		return;
	}
	
	select(0b0001 << _tgtRender); // Selecting the chip
	
	writeData(HT1632_ID_WR, HT1632_ID_LEN);
	writeData(0, HT1632_ADDR_LEN); // Selecting the memory address

	// Write the channels in order
	for(uint8_t c = 0; c < NUM_CHANNEL; ++c) {
		for(uint8_t i = 0; i < ADDR_SPACE_SIZE; ++i) {
			// Write the higher bits before the the lower bits.
			writeData(mem[c][i] >> HT1632_WORD_LEN, HT1632_WORD_LEN); // Write the data in reverse.
			writeData(mem[c][i], HT1632_WORD_LEN); // Write the data in reverse.
		}
	}

	select(); // Close the stream at the end
}
#elif defined TYPE_2416_MONO
	// Draw the contents of mem
	void HT1632Class::render() {
		if (_tgtRender >= _numActivePins) {
			return;
		}

		select(0b0001 << _tgtRender); // Selecting the chip

		writeData(HT1632_ID_WR, HT1632_ID_LEN);
		writeData(0, HT1632_ADDR_LEN); // Selecting the memory address

									   // Write the channels in order
		for (uint8_t c = 0; c < NUM_CHANNEL; ++c) {
			for (uint8_t i = 0; i < ADDR_SPACE_SIZE; ++i) {
				// Write the higher bits before the the lower bits.
				writeData(mem[c][i] >> HT1632_WORD_LEN, HT1632_WORD_LEN); // Write the data in reverse.
				writeData(mem[c][i], HT1632_WORD_LEN); // Write the data in reverse.
			}
		}

		select(); // Close the stream at the end
	}
#endif

// Set the brightness to an integer level between 1 and 16 (inclusive).
// Uses the PWM feature to set the brightness.
void HT1632Class::setBrightness(char brightness, char selectionmask) {
	if(selectionmask == 0b00010000) {
		if(_tgtRender < _numActivePins) {
			selectionmask = 0b0001 << _tgtRender;
		} else {
			return;
		}
	}
	
	select(selectionmask); 
	writeData(HT1632_ID_CMD, HT1632_ID_LEN);    // Command mode
	writeCommand(HT1632_CMD_PWM(brightness));   // Set brightness
	select();
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

void HT1632Class::setCLK(uint8_t pinCLK) {
	_pinCLK = pinCLK;
	pinMode(_pinCLK, OUTPUT);
	digitalWrite(_pinCLK, LOW);
}

inline void HT1632Class::pulseCLK() {
	digitalWrite(_pinCLK, HIGH);
	NOP();
	digitalWrite(_pinCLK, LOW);
}

#if defined TYPE_3216_BICOLOR
// This is used to send initialization commands, and so selects all chips
// in the selected board.
void HT1632Class::select(uint8_t mask) {
	for(uint8_t i=0, t=1; i<_numActivePins; ++i, t <<= 1){
		digitalWrite(_pinCS[i], (t & mask)?LOW:HIGH);
	}
	for (uint8_t tmp = 0; tmp < NUM_ACTIVE_CHIPS; tmp++) {
		pulseCLK();
	}
}
#elif defined TYPE_3208_MONO
// Choose a chip. This function sets the correct CS line to LOW, and the rest to HIGH
// Call the function with no arguments to deselect all chips.
// Call the function with a bitmask (0b4321) to select specific chips. 0b1111 selects all. 
void HT1632Class::select(uint8_t mask) {
	for(uint8_t i=0, t=1; i<_numActivePins; ++i, t <<= 1){
		digitalWrite(_pinCS[i], (t & mask)?LOW:HIGH);
	}
}
#elif defined TYPE_2416_MONO
// Choose a chip. This function sets the correct CS line to LOW, and the rest to HIGH
// Call the function with no arguments to deselect all chips.
// Call the function with a bitmask (0b4321) to select specific chips. 0b1111 selects all. 
void HT1632Class::select(uint8_t mask) {
	for (uint8_t i = 0, t = 1; i<_numActivePins; ++i, t <<= 1) {
		digitalWrite(_pinCS[i], (t & mask) ? LOW : HIGH);
	}
}
#endif
void HT1632Class::select() {
	select(0);
}

HT1632Class HT1632;
