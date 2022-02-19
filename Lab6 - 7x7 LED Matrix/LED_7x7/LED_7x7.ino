/*/
 * Print Patterns on 7x7 LED Display
 * Andrew Player
 * 2022
 * 
 * Simple 7x7 LED Matrix Drawing program.
/*/

#include "LED_Display.hpp"

#define SRCLK_PIN 2  // -- ⚠ Change me if different :D ⚠
#define RCHK_PIN  3  // -- ⚠ Change me if different :D ⚠
#define DATA_PIN  4  // -- ⚠ Change me if different :D ⚠

const uint8_t shift_pins    [n_shift_pins    ] = {DATA_PIN, SRCLK_PIN, RCHK_PIN}; 
const uint8_t hard_wire_pins[n_hard_wire_pins] = {12, 11, 10, 9, 8, 7, 6, 5    };  // -- ⚠ Change me if different :D ⚠

/*/
 * ⚠ Put here whatever you want to display!!! :) ⚠
 * ⚠ 1 means the respective pixel will be ON     ⚠
 * ⚠ 0 means the respective pixel will be OFF    ⚠
/*/
const uint8_t pixel_drawing[n_pixels] = 
{
    0b11111111,
    0b00000000,
    0b01100110,
    0b01100110,
    0b00000000,
    0b11000011,
    0b01111110,
    0b00000000
};

LED_Display display(shift_pins, hard_wire_pins, pixel_drawing);

void setup()
{
    Serial.begin(115200);
    Serial.println("Serial Activated!");
}

void loop() 
{
    display.drawDisplay();
}