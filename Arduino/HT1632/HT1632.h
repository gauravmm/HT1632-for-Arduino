/*
  HT1632.h - Library for communicating with the popular HT1632/HT1632C
  LED controllers. This library provides higher-level access (including
  text drawing) for these chips. Currently, the library supports writing 
  to a single chip at a time, and has been tested with a single
  Sure Electronics 3208 5mm red board.
  
  Created by Gaurav Manek, April 8, 2011.
  Released into the public domain.
*/
#ifndef HT1632_h
#define HT1632_h

/*
 * USER OPTIONS
 * Change these options
 */

// Size of COM and OUT in bits:
#define COM_SIZE 8
#define OUT_SIZE 32
// COM_SIZE MUST be either 8 or 16.

// Target buffer
// Each board has a "render" buffer, and all boards share one "secondary" buffer. All calls to 
//   render() draw the contents of the render buffer of the currently selected board to the board
//   itself. All calls to any drawing function (including clear()) only affect the selected buffer
//   of the selected board. you can move the contents of the secondary buffer to the render
//   buffer by calling transition(), with an appropriate transition. See transition() for more details.
// board_num = [1..4]
#define BUFFER_BOARD(board_num) ((board_num)-1)
#define BUFFER_SECONDARY        0x04

// Transition Modes
// Transitions copies the contents of the "secondary" buffer to the currently selected board buffer.
//   Pass one of these transition types to the transition() function and the contents of the
//   "secondary" buffer will be moved to that using some animation. transition() is a blocking function.
// In all transitions other than the first one, the contents of the board buffer is lost and render()
//   is automatically called.
#define TRANSITION_BUFFER_SWAP     0x00
  // Swap the current buffer and the transition buffer. This is the only transition that preserves
  //   the contents of the current buffer.
#define TRANSITION_NONE            0x01
  // Simply copy the buffer.
#define TRANSITION_FADE            0x02
  // Uses the PWM feature to fade through black. Does not preserve current brightness level.
#define TRANSITION_WIPE_FROM_RIGHT 0x03
// Wrap settings
// For advanced rendering (currently only text rendering)

// Address space size (number of 4-bit words in HT1632 memory)
// Exactly equal to the number of 4-bit address spaces available.
#define ADDR_SPACE_SIZE (COM_SIZE*OUT_SIZE/4)

// Use N-MOS (if 1) or P-MOS (if 0):
#define USE_NMOS 1
// There are known issues with this. If the default doesn't work,
// try changing the value.

// NOTE: THIS HARDCODES THE DIMENSIONS OF THE 3208! CHANGE!
#define GET_ADDR_FROM_X_Y(_x,_y) ((_x)*2+(_y)/4)

/*
 * END USER OPTIONS
 * Don't edit anything below unless you know what you are doing!
 */
 
// Meta-data masks
#define MASK_NEEDS_REWRITING 0b00010000

// Round up to multiple of 4 function

// NO-OP Definition
#define NOP(); __asm__("nop\n\t"); 
// The HT1632 requires at least 50 ns between the change in data and the rising
// edge of the WR signal. On a 16MHz processor, this provides 62.5ns per NOP. 

// Standard command list.
// This list is modified from original code by Bill Westfield

#define HT1632_ID_CMD 0b100	/* ID = 100 - Commands */
#define HT1632_ID_RD  0b110	/* ID = 110 - Read RAM */
#define HT1632_ID_WR  0b101	/* ID = 101 - Write RAM */
#define HT1632_ID_LEN 3         /* IDs are 3 bits */

// Do note that SYSON has been changed to SYSEN
#define HT1632_CMD_SYSDIS 0x00	/* CMD= 0000-0000-x Turn off oscil */
#define HT1632_CMD_SYSEN  0x01	/* CMD= 0000-0001-x Enable system oscil */
#define HT1632_CMD_LEDOFF 0x02	/* CMD= 0000-0010-x LED duty cycle gen off */
#define HT1632_CMD_LEDON  0x03	/* CMD= 0000-0011-x LEDs ON */
#define HT1632_CMD_BLOFF  0x08	/* CMD= 0000-1000-x Blink ON */
#define HT1632_CMD_BLON   0x09	/* CMD= 0000-1001-x Blink Off */
#define HT1632_CMD_SLVMD  0x10	/* CMD= 0001-00xx-x Slave Mode */
#define HT1632_CMD_MSTMD  0x14	/* CMD= 0001-01xx-x Master Mode */
#define HT1632_CMD_RCCLK  0x18	/* CMD= 0001-10xx-x Use on-chip clock */
#define HT1632_CMD_EXTCLK 0x1C	/* CMD= 0001-11xx-x Use external clock */
#define HT1632_CMD_COMS00 0x20	/* CMD= 0010-ABxx-x commons options */
#define HT1632_CMD_COMS01 0x24	/* CMD= 0010-ABxx-x commons options */
#define HT1632_CMD_COMS10 0x28	/* CMD= 0010-ABxx-x commons options */
#define HT1632_CMD_COMS11 0x2C	/* CMD= 0010-ABxx-x commons options */
#define HT1632_CMD_PWM_T  0xA0	/* CMD= 101x-PPPP-x PWM duty cycle - template*/
#define HT1632_CMD_PWM(lvl) (HT1632_CMD_PWM_T | (lvl-1))
  /* Produces the correct command from the given value of lvl. lvl = [0..15] */
#define HT1632_CMD_LEN    8	/* Commands are 8 bits long, excluding the trailing bit */
#define HT1632_ADDR_LEN   7	/* Addresses are 7 bits long */
#define HT1632_WORD_LEN   4     /* Words are 4 bits long */

class HT1632Class
{
  private:  
    char _pinCS [4];
    char _numActivePins;
    char _pinWR;
    char _pinDATA;
    char _tgtBuffer;
    char _globalNeedsRewriting [4];
    char * mem [5];
    void writeCommand(char);
    void writeData(char, char);
    void writeDataRev(char, char);
    void writeSingleBit();
    void initialize(int, int);
    void select();
    void select(char mask);
    
    // Debugging functions, write to Serial.
    void writeInt(int);
    void recursiveWriteUInt(int);
    
  public:
    void begin(int pinCS1, int pinWR,  int pinDATA);
    void begin(int pinCS1, int pinCS2, int pinWR,   int pinDATA);
    void begin(int pinCS1, int pinCS2, int pinCS3,  int pinWR,   int pinDATA);
    void begin(int pinCS1, int pinCS2, int pinCS3,  int pinCS4,  int pinWR,   int pinDATA);
    void sendCommand(char command);
    void drawTarget(char targetBuffer);
    void render();
    void transition(char mode, int time = 1000); // Time is in miliseconds.
    void clear();
    void drawImage(const char * img, char width, char height, char x, char y, int offset = 0);
    void drawText(const char [], int x, int y, const char font [], const char font_width [], char font_height, int font_glyph_step, char gutter_space = 1);
    int getTextWidth(const char [], const char font_width [], char font_height, char gutter_space = 1);
    void setBrightness(char brightness, char selectionmask = 0b00010000);
};

extern HT1632Class HT1632;

#else
//#error "HT1632.h" already defined!
#endif
