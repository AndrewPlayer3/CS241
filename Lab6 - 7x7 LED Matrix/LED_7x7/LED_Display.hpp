/*/
 * Print Patterns on 7x7 LED Display
 * Andrew Player
 * 2022
 * 
 * ⚠ Note: To make an array to draw, simply fill in draw_in_this_array above loop. 
 * ⚠       Each digit represents a pixel on the LED, make a digit 1 for the pixel 
 * ⚠       to be on, and 0 for it to be off. :)
/*/


#ifndef _LED_DISPLAY_HPP_
#define _LED_DISPLAY_HPP_

#ifndef Arduino_h 
#include <Arduino.h>
#endif 

#include <stdint-gcc.h>

#define n_shift_pins      3
#define n_shift_bits      8
#define n_hard_wire_pins  8
#define n_hard_wire_bits  8
#define n_pixels          8
#define n_bits           16

/* Patterns are just 16-Bit Unsigned  */
typedef uint16_t LED_rc_bits_t;

enum : uint8_t
{ 
    // Cols: 0 ->  7
    C0 = 0, C1, C2, C3, C4, C5, C6, C7,

    // Rows: 8 -> 15
    R0 = 8, R1, R2, R3, R4, R5, R6, R7,
};

/* Port bit positions for pins */
const static uint8_t port_positions[14] = 
{
    0b00000001,  // -- Pin  0
    0b00000010,  // -- Pin  1
    0b00000100,  // -- Pin  2
    0b00001000,  // -- Pin  3
    0b00010000,  // -- Pin  4
    0b00100000,  // -- Pin  5
    0b01000000,  // -- Pin  6
    0b10000000,  // -- Pin  7
    0b00000001,  // -- Pin  8
    0b00000010,  // -- Pin  9
    0b00000100,  // -- Pin 10
    0b00001000,  // -- Pin 11
    0b00010000,  // -- Pin 12
    0b00100000   // -- Pin 13
};


/*
 * LED_Display
 *
 * @Description:
 * -------------
 * This struct contains everything for drawing on a 7x7 LED Dislpay with a shift register.
 * 
 * @Note:
 * ------
 * By default it is assumed that the hard-wired side is the side with Row 4,
 * if not, just you need to use the flipSetup() method.
 * 
 * @Params:
 * --------
 * shift_pins: uint8_t[#shift_register_pins (3)]
 *  This array should contain the pin numbers in this order: DATA, SRCLK, RCHK.
 * hard_wire_pins: uint8_t[#hard_wired_pins (8)]
 *  This array should contain the pin numbers as they correspond to this order: {R4, R6, C1, C2, R7, C4, R5, R2} if default.
 *                                                                           or {R0, C3, C5, R3, C0, R1, C6, C7} if using flipSetup().
 * pixel_drawing: uint8_t[#pixels (8)]
 *  This array contains an array of binary numbers, 1 8-bit number for each row.
 *  Each digit in the binary number represents the respective pixel. 0's are off, 1's are on.
 *      Diagonal Line Example: {
 *                              0b10000000,  ON OFF OFF OFF OFF OFF OFF OFF
 *                              0b01000000,  OFF ON ...                 OFF
 *                              0b00100000,  ...
 *                              0b00010000,  ...
 *                              0b00001000,  ...
 *                              0b00000100,  ...
 *                              0b00000010,  ...
 *                              0b00000001   OFF OFF OFF OFF OFF OFF OFF ON
 *                              }
 */
struct LED_Display 
{
    private:
        /* --- Private Data Members --- */

        uint8_t DATA_pin ;  // Data Pin
        uint8_t SRCLK_pin;  // Shift Pin 
        uint8_t RCHK_pin ;  // Output Pin

        uint8_t hard_wire_pins[n_hard_wire_pins];
        
        LED_rc_bits_t patterns[n_pixels * 2];
        uint8_t       pixels  [n_pixels    ];
        
        uint8_t hard_wire_rc  [n_hard_wire_bits] = {R4, R6, C1, C2, R7, C4, R5, R2};
        uint8_t shift_rc      [n_shift_bits    ] = {C7, C6, R1, C0, R3, C5, C3, R0};
        uint8_t shift_offsets [n_shift_bits    ] = { 7,  6,  5,  4,  3,  2,  1,  0};

        /*/
        * Table for getting three things:
        * 1. If a bit position value goes to a hardware pin, or not.
        * 2. If hardware, the pin is given; else the left shift amount is given (position from the right in rc_s).
        * 3. The rc value for that bit position.
        /*/
        uint8_t is_hardwired_pin_and_rc_at_bit[n_bits][3];


        /* --- Getters --- */

        // Get the rc value for the bit in pattern_bits offset by rc_bit
        bool getRCBit(const LED_rc_bits_t& pattern_bits, const uint8_t& rc_bit) const; 


        /* --- Setters --- */

        // Set the values of the pins
        void setPins(const uint8_t shift_pins[n_shift_pins], const uint8_t hard_wire_pins[n_hard_wire_pins]);
 
        // Setup the is_hardwired_pin_and_rc_at_bit table.
        void setIsHardwiredPinAndRcAtBit();

        // Constructs the pattern array from the pixel array.
        void setPatternArray();

        // Sets the pin mode for all pins to OUTPUT
        void setPinModes() const;

        // Pulse the clock pin UP and DOWN a single time
        void clockPulse(const uint8_t& pin) const;
        
        // Send one byte of data to the shift register and output it.
        void sendByte(const uint8_t& data) const;


        /* --- Misc. --- */

        // Shows a single byte pattern (used by drawPixels to show rows, but it can show anything).
        void showPattern(const LED_rc_bits_t& pattern) const;


    public:
 
        /* --- Constructors --- */

        // Default Constructor
        LED_Display();

        // Value Constructor 
        LED_Display(const uint8_t shift_pins   [n_shift_pins    ],
                    const uint8_t hardware_pins[n_hard_wire_pins],
                    const uint8_t pixel_drawing[n_pixels        ]);

        // Init method for use with default constructor
        void init(const uint8_t shift_pins   [n_shift_pins    ],
                  const uint8_t hardware_pins[n_hard_wire_pins],
                  const uint8_t pixel_drawing[n_pixels        ]);

        
        /* --- Main Setter --- */
        
        // Set the values in the pixel array
        void setPixels(const uint8_t pixel_drawing[n_pixels]);


        /* --- Mutators --- */

        // This flips the configuration so that it will work with the shift_reg on the Row 4 side.
        void flipSetup();
        
        
        /* --- Output Member Functions --- */

        // Serial Print the Pattern Values that are being read.
        void printPatterns() const;
        
        // Serial Print the Pixel Values that are being read.
        void printPixels  () const;

        // Draw the pixel array onto the LED Matrix.
        void drawDisplay  () const;
};

#endif
