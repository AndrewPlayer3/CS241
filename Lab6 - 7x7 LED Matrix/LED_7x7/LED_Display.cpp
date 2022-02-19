/*/
 * Print Patterns on 7x7 LED Display
 * Andrew Player
 * 2022
/*/


#include "LED_Display.hpp"


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
    setIsHardwiredPinAndRcAtBit();
    setPinModes();
    setPixels(pixel_drawing);
}


void LED_Display::setPins(const uint8_t shift_pin_array    [n_shift_pins    ],
                          const uint8_t hard_wire_pin_array[n_hard_wire_pins])
{
    DATA_pin = shift_pin_array[0];
    SRCLK_pin = shift_pin_array[1];
    RCHK_pin = shift_pin_array[2];

    for (uint8_t pin = 0; pin < n_hard_wire_pins; pin++) {
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

    // Go through each uint8_t pixel row and generate a LED_rc_bits_t pattern row
    for (uint8_t row = 0; row < n_pixels; row++)
    {   
        LED_rc_bits_t pattern_row = 0b1111111100000000;
        const uint8_t drawing_row = pixels[row];
        const uint8_t row_shift   = row + 8;

        for (uint8_t col = 0; col < n_pixels; col++)
        {
            const uint8_t pixel = (drawing_row & (1 << col));

            if (pixel)
            {
                pattern_row &= ~(0b1 << row_shift);  // Row Bit
                pattern_row |=   0b1 << col;         // Col Bit
            }
        }
        patterns[row * 2] = pattern_row;
    }
}


void LED_Display::setPinModes() const
{
    for (const uint8_t hard_wired_pin : hard_wire_pins) 
    {
        pinMode(hard_wired_pin, OUTPUT);
    }

    pinMode(RCHK_pin, OUTPUT);
    digitalWrite(RCHK_pin, 0);

    pinMode(SRCLK_pin, OUTPUT);
    digitalWrite(SRCLK_pin, 0);

    pinMode(DATA_pin, OUTPUT);
    digitalWrite(DATA_pin, 0);
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
        uint8_t h_temp = hard_wire_rc[index];
        uint8_t s_temp = shift_rc[index];
        hard_wire_rc[index] = s_temp;
        shift_rc[index] = h_temp;

        if(index < back)
        {
            uint8_t o_temp = shift_offsets[index];
            shift_offsets[index] = shift_offsets[back];
            shift_offsets[back] = o_temp;
        }
        back--;
    }

    setIsHardwiredPinAndRcAtBit();
}


bool LED_Display::getRCBit(const LED_rc_bits_t& pattern_bits, const uint8_t& rc_bit) const 
{ 
    return ((pattern_bits >> rc_bit) & 1); 
}


/* Pulse shift register clock (shifts 1 bit or outputs) */
void LED_Display::clockPulse(const uint8_t& pin) const 
{
    digitalWrite(pin, 1);
    digitalWrite(pin, 0);
}


void LED_Display::sendByte(const uint8_t& data) const 
{
    for (uint8_t bit = 0; bit < 8; bit++) 
    {
        const bool send_bit = data & (1 << bit);

        if (send_bit) digitalWrite(DATA_pin, 1);
        else          digitalWrite(DATA_pin, 0);

        clockPulse(SRCLK_pin);
    }
    clockPulse(RCHK_pin);
}


void LED_Display::showPattern(const LED_rc_bits_t& pattern) const 
{

    uint8_t shift  = 0b00000000; // This value gets sent to sendByte for the shift register
    uint8_t port_b = 0b00000000; // This value gets sent to PORTB
    uint8_t port_d = 0b00000000; // This value gets sent to PORTD

    for (uint8_t b = 0; b < 8; b++)
    {
        for (uint8_t i = 2; i > 0; i--) 
        {
            const uint8_t  rc = (2 * i * 4) - 1 - b;

            const uint8_t pin         = is_hardwired_pin_and_rc_at_bit[rc][1];  
            const uint8_t rc_val      = is_hardwired_pin_and_rc_at_bit[rc][2];
            const bool    is_hardware = is_hardwired_pin_and_rc_at_bit[rc][0];
            const bool    bit         = getRCBit(pattern,  rc_val);

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


void LED_Display::drawDisplay() const
{
    for (uint8_t row = 0; row < n_pixels * 2; row++) 
    {
        showPattern(patterns[row]);
        delayMicroseconds(500);
    }
}


void LED_Display::printPixels() const
{
    Serial.print("\n---------------------\n");
    for (uint8_t row = 0; row < n_pixels; row++)
    {
        Serial.print(" PXR");
        Serial.print(row);
        Serial.print(": ");
        Serial.println(pixels[row]);
    }
}


void LED_Display::printPatterns() const
{
    Serial.print("\n---------------------\n");
    for (uint8_t row = 0; row < n_pixels * 2; row++)
    {
        Serial.print(" PAT");
        Serial.print(row);
        Serial.print(": ");
        Serial.println(patterns[row]);
    }
}