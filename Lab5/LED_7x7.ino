/*/
 * Print Patterns on 7x7 LED Display
 * Andrew Player
 * 2022
/*/


typedef enum 
{ 

  // Cols are 0 ->  8
  c0=0,
  c1,
  c2,
  c3,
  c4,
  c5,
  c6,
  c7,

  // Rows are 8 -> 15
  r0=8,
  r1,
  r2,
  r3,
  r4,
  r5,
  r6,
  r7,

} rc_t;


uint8_t outputPin = 3; // RCLK  -- ⚠ Change me if yours is different ⚠
uint8_t dataPin   = 4; // DATA  -- ⚠ Change me if yours is different ⚠
uint8_t shiftPin  = 2; // SRCLK -- ⚠ Change me if yours is different ⚠


const uint8_t nhardwire   = 8;
const uint8_t nshift_bits = 8;
const uint8_t h_pins[nhardwire  ] = {12,11,10, 9, 8, 7, 6, 5}; // -- ⚠ Change me if yours is different ⚠
const rc_t    rch   [nhardwire  ] = {r4,r6,c1,c2,r7,c4,r5,r2};
const rc_t    rcs   [nshift_bits] = {c7,c6,r1,c0,r3,c5,c3,r0};


const int nbits=16;
const uint8_t rc_and_pin_at_bit[nbits][3]=
{
  {0,         4, rcs[3]},  // -- C0
  {1, h_pins[2], rch[2]},  // -- C1
  {1, h_pins[3], rch[3]},  // -- C2
  {0,         1, rcs[6]},  // -- C3
  {1, h_pins[5], rch[5]},  // -- C4
  {0,         2, rcs[5]},  // -- C5
  {0,         6, rcs[1]},  // -- C6
  {0,         7, rcs[0]},  // -- C7
  {0,         0, rcs[7]},  // -- R0
  {0,         5, rcs[2]},  // -- R1
  {1, h_pins[7], rch[7]},  // -- R2
  {0,         3, rcs[4]},  // -- R3
  {1, h_pins[0], rch[0]},  // -- R4
  {1, h_pins[6], rch[6]},  // -- R5
  {1, h_pins[1], rch[1]},  // -- R6
  {1, h_pins[4], rch[4]}   // -- R7
};


// Port bit positions for pins 0 -> 13
const uint8_t port_positions[14] = 
{
  0b00000001,
  0b00000010,
  0b00000100,
  0b00001000,
  0b00010000,
  0b00100000,
  0b01000000,
  0b10000000,
  0b00000001,
  0b00000010,
  0b00000100,
  0b00001000,
  0b00010000,
  0b00100000
};


typedef unsigned short LED_rc_bits_t;


inline bool get_rc_bit(LED_rc_bits_t rc_bits, rc_t bit)
{
  return (rc_bits >> bit) & 1;
}


const int    npatterns=6 + 8 + 8;
LED_rc_bits_t patterns[npatterns] =
{

  /*------------------
    ROW    COL
    ON: 0  ON: 1   
    7654321076543210
  ------------------*/

  0b1111111100000000, // All LEDs OFF
  0b0000000011111111, // All LEDs ON
  0b0111111010000001, // Corner LEDs ON
  0b1011110101000010, // Corner Inset-1
  0b1101101100100100, // Corner Inset-2
  0b1110011100011000, // Center 4

  0b0000000010000000, // Right-to-left Col Scan
  0b0000000001000000,
  0b0000000000100000,
  0b0000000000010000,
  0b0000000000001000,
  0b0000000000000100,
  0b0000000000000010,
  0b0000000000000001,

  0b0111111111111111, // Bottom-to-top Row Scan
  0b1011111111111111,
  0b1101111111111111,
  0b1110111111111111,
  0b1111011111111111,
  0b1111101111111111,
  0b1111110111111111,
  0b1111111011111111,

};


void clockPulse(int pin) {
  digitalWrite(pin, 1);
  digitalWrite(pin, 0);
}


void sendByte(uint8_t data) 
{
  for (uint8_t b = 0; b < 8; b++) 
  {
    bool sendBit = data & (1 << b);

    if(sendBit) 
    {
      digitalWrite(dataPin, 1);
    }
    else 
    {
      digitalWrite(dataPin, 0);
    }
    clockPulse(shiftPin);
  }
  clockPulse(outputPin);
}


void setup() {

  Serial.begin(115200);
  Serial.println("Serial Activated!");

  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);

  pinMode(outputPin,OUTPUT);
  digitalWrite(outputPin,0);

  pinMode(shiftPin,OUTPUT);
  digitalWrite(shiftPin,0);

  pinMode(dataPin,OUTPUT);
  digitalWrite(dataPin,0);

}


// Display a given pattern  -- Ports and such because they are cool.
void showPattern(LED_rc_bits_t pattern) {

  uint8_t shift  = 0b00000000; // This value gets sent to sendByte for the shift register
  uint8_t port_b = 0b00000000; // This value gets sent to PORTB
  uint8_t port_d = 0b00000000; // This value gets sent to PORTD

  for(uint8_t b = 0; b < 8; b++)
  {
    for(int i = 2; i > 0; i--) 
    {
      uint8_t  rc                       = (2 * i * 4) - 1 - b;
      uint8_t *isnotshiftreg_pin_and_rc = rc_and_pin_at_bit[rc];

      uint8_t pin              = isnotshiftreg_pin_and_rc[1];  
      uint8_t rc_val           = isnotshiftreg_pin_and_rc[2];
      bool    not_in_shift_reg = isnotshiftreg_pin_and_rc[0];
      bool    bit              = get_rc_bit(pattern, rc_val);

      /* Set the values using port if this row/column goes directly to the pins. */
      if(not_in_shift_reg)
      {
        uint8_t port_position = port_positions[pin];
 
        if(bit) 
        {
          if(pin < 8) port_d |= port_position;
          else        port_b |= port_position;
        }
        else
        {
          if(pin < 8) port_d &= ~(port_position);
          else        port_b &= ~(port_position);
        }
      }

      /* Set the values using shift if this row/column goes to the shift register. */
      else
      {
        if(bit) shift |=  (1 << pin);  /* Since the pin is constant for the shift register, */
        else    shift &= ~(1 << pin);  /* I have tied the offset to its pin value instead.  */
      }
    }
  }
  PORTB = port_b;
  PORTD = port_d;
  sendByte(shift);
}


void loop() 
{
  for(int i = 0; i < 22; i++) 
  {
    showPattern(patterns[i]);
    delay(500);
  }
}
