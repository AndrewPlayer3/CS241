/*/
 * Print Patterns on 7x7 LED Display
 * Andrew Player
 * 2022
/*/


enum : uint8_t
{ 
    // Cols: 0 ->  7
    C0 = 0, C1, C2, C3, C4, C5, C6, C7,

    // Rows: 8 -> 15
    R0 = 8, R1, R2, R3, R4, R5, R6, R7,
};


const uint8_t output_pin = 3;  // RCHK  -- ⚠ Change me if yours is different ⚠
const uint8_t data_pin   = 4;  // DATA  -- ⚠ Change me if yours is different ⚠
const uint8_t shift_pin  = 2;  // SRCLK -- ⚠ Change me if yours is different ⚠


const uint8_t n_hard_wire_bits  = 8;
const uint8_t n_shift_bits      = 8;
const uint8_t h_pins[n_hard_wire_bits ] = {12, 11, 10,  9,  8,  7,  6,  5};  // -- ⚠ Change me if yours is different ⚠
const uint8_t rc_h  [n_hard_wire_bits ] = {R4, R6, C1, C2, R7, C4, R5, R2};
const uint8_t rc_s  [n_shift_bits     ] = {C7, C6, R1, C0, R3, C5, C3, R0};


/*/
 * Table for getting three things:
 * 1. If a bit position value goes to a hardware pin, or not.
 * 2. If hardware, the pin is given; else the left shift amount is given (position from the right in rc_s).
 * 3. The rc value for that bit position.
/*/
const uint8_t n_bits = 16;
const uint8_t is_hardware_pin_and_rc_at_bit[n_bits][3] =
{
    {0,         4, rc_s[3]},  // -- C0
    {1, h_pins[2], rc_h[2]},  // -- C1
    {1, h_pins[3], rc_h[3]},  // -- C2
    {0,         1, rc_s[6]},  // -- C3
    {1, h_pins[5], rc_h[5]},  // -- C4
    {0,         2, rc_s[5]},  // -- C5
    {0,         6, rc_s[1]},  // -- C6
    {0,         7, rc_s[0]},  // -- C7
    {0,         0, rc_s[7]},  // -- R0
    {0,         5, rc_s[2]},  // -- R1
    {1, h_pins[7], rc_h[7]},  // -- R2
    {0,         3, rc_s[4]},  // -- R3
    {1, h_pins[0], rc_h[0]},  // -- R4
    {1, h_pins[6], rc_h[6]},  // -- R5
    {1, h_pins[1], rc_h[1]},  // -- R6
    {1, h_pins[4], rc_h[4]}   // -- R7
};


/* Port bit positions for pins */
const uint8_t port_positions[14] = 
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


/* Patterns are just 16-Bit Unsigned Integers */
typedef uint16_t LED_rc_bits_t;


/* Extract a bit from pattern from rc position bit */
inline bool getRCBit(const LED_rc_bits_t& pattern_bits, const uint8_t& rc_bit)
{
    return ((pattern_bits >> rc_bit) & 1);
}


const uint16_t               n_patterns  = 22;
const LED_rc_bits_t patterns[n_patterns] =
{

    /*-----------------/
      ROW    COL
      ON: 0  ON: 1   
      7654321076543210
    /-----------------*/

    0b1111111100000000,  // All LEDs OFF
    0b0000000011111111,  // All LEDs ON
    0b0111111010000001,  // Corner LEDs ON
    0b1011110101000010,  // Corner LEDs Inset-1
    0b1101101100100100,  // Corner LEDs Inset-2
    0b1110011100011000,  // Center 4 LEDs

    0b0000000010000000,  // Right-to-Left Col Scan
    0b0000000001000000,
    0b0000000000100000,
    0b0000000000010000,
    0b0000000000001000,
    0b0000000000000100,
    0b0000000000000010,
    0b0000000000000001,

    0b0111111111111111,  // Bottom-to-Top Row Scan
    0b1011111111111111,
    0b1101111111111111,
    0b1110111111111111,
    0b1111011111111111,
    0b1111101111111111,
    0b1111110111111111,
    0b1111111011111111,

};


/* Pulse shift register clock (shifts 1 bit or outputs) */
void clockPulse(const uint8_t& pin) 
{
    digitalWrite(pin, 1);
    digitalWrite(pin, 0);
}


/*/
 *  Send an 8-Bit Integer to the Shift Register,
 *  then shift through it and output it.
/*/
void sendByte(const uint8_t& data) 
{
    for (uint8_t bit = 0; bit < 8; bit++) 
    {
        const bool send_bit = data & (1 << bit);

        if (send_bit) digitalWrite(data_pin, 1);
        else          digitalWrite(data_pin, 0);

        clockPulse(shift_pin);
    }
    clockPulse(output_pin);
}


/*/
 *  Display a given pattern 
 *  Uses ports and such because they are nice.
/*/
void showPattern(const LED_rc_bits_t& pattern) 
{

    uint8_t shift  = 0b00000000; // This value gets sent to sendByte for the shift register
    uint8_t port_b = 0b00000000; // This value gets sent to PORTB
    uint8_t port_d = 0b00000000; // This value gets sent to PORTD

    for (uint8_t b = 0; b < 8; b++)
    {
        for (uint8_t i = 2; i > 0; i--) 
        {
            const uint8_t  rc                     = (2 * i * 4) - 1 - b;
            const uint8_t* is_hardware_pin_and_rc = is_hardware_pin_and_rc_at_bit[rc];

            const uint8_t& pin         = is_hardware_pin_and_rc [1];  
            const uint8_t& rc_val      = is_hardware_pin_and_rc [2];
            const bool&    is_hardware = is_hardware_pin_and_rc [0];
            const bool     bit         = getRCBit(pattern,  rc_val);

            /* Set the values using port if this row/column goes directly to the pins. */
            if (is_hardware)
            { 
                const uint8_t& port_position = port_positions[pin];
 
                if (bit) 
                {
                    if (pin < 8) port_d |= port_position;
                    else         port_b |= port_position;
                }
                else
                {
                    if (pin < 8) port_d &= ~(port_position);
                    else         port_b &= ~(port_position);
                }
            }

            /* Set the values using shift if this row/column goes to the shift register. */
            else
            {
                if (bit) shift |=  (1 << pin);  // Since the pin is constant for the shift register,
                else     shift &= ~(1 << pin);  // the offset is its pin value instead.
            }
        }
    }

    PORTB = port_b;
    PORTD = port_d;
    sendByte(shift);
}


void setup()
{
    Serial.begin(115200);

    Serial.println("Serial Activated!");

    for (const uint8_t& hardware_pin : h_pins) 
    {
    pinMode(hardware_pin, OUTPUT);
    }

    pinMode(output_pin, OUTPUT);
    digitalWrite(output_pin, 0);

    pinMode(shift_pin , OUTPUT);
    digitalWrite(shift_pin , 0);

    pinMode(data_pin  , OUTPUT);
    digitalWrite(data_pin  , 0);
}


void loop() 
{
  
    for (const LED_rc_bits_t& pattern : patterns) 
    {
        showPattern(pattern);
        delay(200);
    }

}
