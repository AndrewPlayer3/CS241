/*/
 * Library for Displaying Patterns on 74HC595 8x8 LED Display
 * with a 74HC595 shift regesiter.
 * Andrew Player
 * 2022
/*/


#include "LED_Display.hpp"


/*----------------------------------------------------------------------------------------------------------------*/
/* Macros for speed and cleanliness, but feels illegal to me for some reason.                                     */
/*----------------------------------------------------------------------------------------------------------------*/
#define PORTDWRITE1(PIN) PORTD |=  (0b1 <<  PIN)
#define PORTDWRITE0(PIN) PORTD &= ~(0b1 <<  PIN)

#define PORTBWRITE1(PIN) PORTB |=  (0b1 << (PIN % 8))
#define PORTBWRITE0(PIN) PORTB &= ~(0b1 << (PIN % 8))

#define PICKPORTANDWRITE1(PIN) if (PIN < 8){PORTDWRITE1(PIN);} else {PORTBWRITE1(PIN);}
#define PICKPORTANDWRITE0(PIN) if (PIN < 8){PORTDWRITE0(PIN);} else {PORTBWRITE0(PIN);}

#define CLOCKPULSE(PIN) if (PIN < 8) {PORTDWRITE1(PIN); PORTDWRITE0(PIN);} else {PORTBWRITE1(PIN); PORTBWRITE0(PIN);}
/*----------------------------------------------------------------------------------------------------------------*/


LED_Display::LED_Display()
{

}


LED_Display::LED_Display(const uint8_t shift_pin_array    [n_shift_pins    ],
                         const uint8_t hard_wire_pin_array[n_hard_wire_pins],
                         const uint8_t pixel_drawing      [n_pixels        ])
{
    init(shift_pin_array, hard_wire_pin_array, pixel_drawing);
}


void LED_Display::init(const uint8_t shift_pin_array    [n_shift_pins    ],
                       const uint8_t hard_wire_pin_array[n_hard_wire_pins],
                       const uint8_t pixel_drawing      [n_pixels        ])
{
    setPins(shift_pin_array, hard_wire_pin_array);
    setPinModes();
    setPixels(pixel_drawing);
    setIsHardwiredPinAndRcAtBit();
}


LED_Display::~LED_Display()
{
  const uint8_t zeros[n_pixels] = {0, 0, 0, 0, 0, 0, 0, 0};
  setPixels(zeros);
  drawDisplay();
}


void LED_Display::setPins(const uint8_t shift_pin_array    [n_shift_pins    ],
                          const uint8_t hard_wire_pin_array[n_hard_wire_pins])
{
    DATA_pin  = shift_pin_array[0];
    SRCLK_pin = shift_pin_array[1];
    RCLK_pin  = shift_pin_array[2];

    for (uint8_t pin = 0; pin < n_hard_wire_pins; pin++) 
    {
        hard_wire_pins[pin] = hard_wire_pin_array[pin];
    }
}


void LED_Display::setPixels(const uint8_t pixel_drawing[n_pixels])
{
    for (uint8_t pxl = 0; pxl < n_pixels; pxl++)
    {
        pixels[pxl] = pixel_drawing[pxl];
    }

    setPatternArray();
}


void LED_Display::setPatternArray() 
{
    // Blank out between frames for less glitchiness.
    for (uint8_t row = 1; row < n_pixels * 2; row += 2)
    {
        patterns[row] = 0b1111111100000000;
    }

    uint8_t placement = 0;

    // Go through each uint8_t pixel row and generate a LED_rc_bits_t pattern row
    for (int8_t row = n_pixels - 1; row >= 0; row--)
    { 
        LED_rc_bits_t pattern_row = 0b1111111100000000;
 
        const uint8_t drawing_row = pixels[placement];
        const uint8_t row_shift   = row + 8;

        for (int8_t col = 0; col < n_pixels; col++)
        {
            const uint8_t pixel = (drawing_row & (1 << col));

            if (pixel)
            {
                pattern_row &= ~(0b1 << row_shift);  // Row Bit
                pattern_row |=   0b1 << col;         // Col Bit
            }
        }
        patterns[placement * 2] = pattern_row;
        placement++;
    }
}


void LED_Display::setPinModes() const
{
    for (const uint8_t hard_wired_pin : hard_wire_pins) 
    {
        pinMode(hard_wired_pin, OUTPUT);
    }

    pinMode(RCLK_pin, OUTPUT);
    PICKPORTANDWRITE0(RCLK_pin);

    pinMode(SRCLK_pin, OUTPUT);
    PICKPORTANDWRITE0(SRCLK_pin);

    pinMode(DATA_pin, OUTPUT);
    PICKPORTANDWRITE0(DATA_pin);
}


void LED_Display::setIsHardwiredPinAndRcAtBit()
{
    for (uint8_t row = 0; row < n_bits; row++)
    {
        for (uint8_t col = 0; col < n_hard_wire_bits; col++)
        {
            if (row == hard_wire_rc[col])
            { 
                is_hardwired_pin_and_rc_at_bit[row][0] = true;
                is_hardwired_pin_and_rc_at_bit[row][1] = hard_wire_pins[col];
                is_hardwired_pin_and_rc_at_bit[row][2] = row;
                break;
            }
            if (row == shift_rc[col])
            {
                is_hardwired_pin_and_rc_at_bit[row][0] = false;
                is_hardwired_pin_and_rc_at_bit[row][1] = shift_offsets[col];
                is_hardwired_pin_and_rc_at_bit[row][2] = row;
                break;
            }
        }
    }
}


uint8_t LED_Display::getPatternRow(const uint8_t& pattern_row) const
{
    if (pattern_row > n_pixels * 2 - 1) return 0;
    return pixels[pattern_row];
}


uint8_t LED_Display::getPixelRow(const uint8_t& pixel_row) const
{
    if (pixel_row > n_pixels - 1) return 0;
    return pixels[pixel_row];
}


void LED_Display::flipSetup()
{
    if (n_hard_wire_bits != n_shift_bits)
    {
        Serial.println("ARRAY SIZES DONT MATCH FOR SWAPRC!");
        return;
    }

    uint8_t back = n_hard_wire_bits - 1;

    for (uint8_t index = 0; index < n_hard_wire_bits; index++)
    {
        const uint8_t h_temp = hard_wire_rc[index];
        const uint8_t s_temp = shift_rc    [index];

        hard_wire_rc[index]  = s_temp;
        shift_rc    [index]  = h_temp;

        if(index < back)
        {
            const uint8_t o_temp = shift_offsets[index];

            shift_offsets[index] = shift_offsets[back ];
            shift_offsets[back ] = o_temp;
        }
 
        back--;
    }

    setIsHardwiredPinAndRcAtBit();
}


void LED_Display::sendByte(const uint8_t& data) const 
{
    for (uint8_t bit = 0; bit < 8; bit++) 
    {
        if (data & (1 << bit)) {PICKPORTANDWRITE1(DATA_pin);}
        else                   {PICKPORTANDWRITE0(DATA_pin);}

       /* Fun fact: If I assume that this goes to Port D, and replace this with PORTDWRITE1 and PORTDWRITE0,
        *           along with the similar calls above and below, it is actually consistantly significantly slower. 
        */
        CLOCKPULSE(SRCLK_pin);
    }

    CLOCKPULSE(RCLK_pin);
}


void LED_Display::showPattern(const LED_rc_bits_t& pattern) const 
{
    uint8_t shift  = 0b00000000; // This value gets sent to sendByte for the shift register
    uint8_t port_b = 0b00000000; // This value gets sent to PORTB
    uint8_t port_d = 0b00000000; // This value gets sent to PORTD

    for (uint8_t rc = 0; rc < 16; rc++)
    {
        const bool&    is_shift = !is_hardwired_pin_and_rc_at_bit[rc][0];
        const uint8_t& pin      =  is_hardwired_pin_and_rc_at_bit[rc][1];  
        const uint8_t& rc_val   =  is_hardwired_pin_and_rc_at_bit[rc][2];

        const bool bit  = 1 & (pattern >> rc_val);  // Get the bit at the row/column offset in the pattern.

        /* Place the bit in the correct position */
        if (is_shift){shift  ^=  (bit << (pin)); continue;}

        if (pin < 8)  port_d += ~(port_d) & (bit << (pin    ));  // This is equivalent to _ ^= (bit << pin),
        else          port_b += ~(port_d) & (bit << (pin % 8));  // but this is faster here.
    }

    PORTB = port_b ;
    PORTD = port_d ;
    sendByte(shift);
}


void LED_Display::drawDisplay() const
{
    for (uint8_t row = 0; row < n_pixels * 2; row++) 
    {
        showPattern(patterns[row]);
    }
}


void LED_Display::drawDisplay(const uint32_t& microseconds) const
{
    for (uint8_t row = 0; row < n_pixels * 2; row++) 
    {
        showPattern(patterns[row]);
    }
    delayMicroseconds(microseconds);
}


void LED_Display::printPixels() const
{
    for (uint8_t row = 0; row < n_pixels; row++)
    {
        Serial.print("PIXEL ");
        Serial.print(row);
        Serial.print(": ");
        Serial.print(pixels[row]);
        Serial.println(" ");
    }
}


void LED_Display::printPatterns() const
{
    for (uint8_t row = 0; row < n_pixels * 2; row++)
    {
        Serial.print("PATTERN ");
        Serial.print(row);
        Serial.print(": ");
        Serial.println(patterns[row]);
        Serial.println(" ");
    }
}
