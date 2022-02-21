/*/
 * Print Patterns on 8x8 LED Display
 * Andrew Player
 * 2022
 * 
 * Simple 8x8 LED Matrix Drawing program.
/*/

#include "LED_Display.hpp"

#define SRCLK_PIN 2  // -- ⚠ Change me if different :D ⚠
#define RCLK_PIN  3  // -- ⚠ Change me if different :D ⚠
#define DATA_PIN  4  // -- ⚠ Change me if different :D ⚠

const uint8_t shift_pins    [N_SHIFT_PINS    ] = {DATA_PIN, SRCLK_PIN, RCLK_PIN}; 
const uint8_t hard_wire_pins[N_HARD_WIRE_PINS] = {12, 11, 10, 9, 8, 7, 6, 5    };  // ⚠ Change me if different :D ⚠
                                    /*  Default:  R4, ...,                R2                                         */
                                    /*  Else:     R0, ...,                C7 and add display.flipSetup() in setup(). */

/*/
 * ⚠ Put here whatever you want to display!!! :) ⚠
 * ⚠ 1 means the respective pixel will be ON     ⚠
 * ⚠ 0 means the respective pixel will be OFF    ⚠
/*/
const uint8_t pixel_drawing[N_PIXELS] = 
{
    0b11000011,
    0b00000000,
    0b01100110,
    0b01100110,
    0b00000000,
    0b01111110,
    0b01000010,
    0b00111100
};

LED_Display display;

void setup()
{
    Serial.begin(115200);
    Serial.println("Serial Activated!");
    display.init(shift_pins, hard_wire_pins, pixel_drawing);
}

int counter    = 0;
int iterations = 1000;

unsigned long draw_time  = 0;

void loop() 
{
    unsigned long start_t = micros();

    ///////////////////////////
    display.drawDisplay();   // You can optionally add a delay in microseconds (uint32_t) as a parameter.
    ///////////////////////////

    unsigned long end_t   = micros();

    draw_time += (end_t - start_t);

    if (counter == iterations)
    {
      double avg_timing = (double)draw_time / (double)iterations;

      double hertz      = (1000000 / avg_timing);

      Serial.print  ("Average time to display: ");
      Serial.print  (avg_timing                 );
      Serial.print  (" microseconds, which is " );
      Serial.print  (hertz                      );
      Serial.println(" hertz."                  );

      draw_time = 0;
      counter   = 0;
    }

    else counter++;
}
