/* 
 * The digital pin numbers for each segment and an 
 * array to hold them in alphabetic order.
 */
const int pinA = 9;
const int pinB = 8;
const int pinC = 4;
const int pinD = 5;
const int pinE = 6;
const int pinF = 10;
const int pinG = 3;
const int pinP = 7;

const int pins[] = {pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinP};


const int nseg = 8;  // Number of Segments on Display
const int nsym = 64; // Number of Patterns we want to support

/*
 * Table of patters represents the numbers 0->9 and the letters A->F
 */
int patterns[nsym][nseg] = {
                      {pinA, pinB, pinC, pinD, pinE, pinF},       // 0 
                      {pinB, pinC},                               // 1
                      {pinA, pinB, pinG, pinE, pinD},             // 2
                      {pinA, pinB, pinG, pinC, pinD},             // 3
                      {pinF, pinG, pinB, pinC},                   // 4
                      {pinA, pinF, pinG, pinC, pinD},             // 5
                      {pinA, pinF, pinG, pinC, pinD, pinE},       // 6
                      {pinA, pinB, pinC},                         // 7
                      {pinA, pinF, pinB, pinG, pinE, pinC, pinD}, // 8
                      {pinA, pinF, pinB, pinG, pinC},             // 9
                      {pinA, pinF, pinB, pinG, pinE, pinC},       // A
                      {pinA, pinF, pinB, pinG, pinE, pinC, pinD}, // B
                      {pinA, pinF, pinE, pinD},                   // C
                      {pinA, pinB, pinC, pinD, pinE, pinF},       // D
                      {pinA, pinF, pinG, pinE, pinD},             // E
                      {pinA, pinF, pinG, pinE},                   // F
                      };


/*
 * Digital Ground Pins and an array to hold them
 */
const int DIG1 = 11;
const int DIG2 = 12;
const int DIG3 = 13;
const int digit_pins[] = {DIG1, DIG2, DIG3};

/* 
 * Current value/pattern for each digit in ascending order 
 */
int digits[] = {0, 0, 0};

/*
 * Display the given pattern
 */
void setPattern(int pattern) {
  int* pins = patterns[pattern];
  int p = 0;
  while(p < nseg) {
    if(*pins >= 3 && *pins <= 10) {
      digitalWrite(*pins, HIGH);
      delay(1);
      digitalWrite(*pins, LOW);
    }
    pins++;
    p++;
  }
}

/*
 * Display the pattern associated with the given digit position
 */
void setDigit(int digit) {
  digitalWrite(digit_pins[digit], LOW);
  for(int i = 0; i < 3; i++) {
    if(i != digit) {
      digitalWrite(digit_pins[i], HIGH);
    }
  }
  setPattern(digits[digit]);
  digitalWrite(digit_pins[digit], HIGH);
}


void setup()

{

  for(int pin : pins) {
    pinMode(pin, OUTPUT);
  }

  Serial.begin(9600);
  
  pinMode(DIG1, OUTPUT); 
  pinMode(DIG2, OUTPUT);
  pinMode(DIG3, OUTPUT);

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);

}

int current_digit = 0;    // Current digit position
int current_value = 0;    // Current value/pattern to set digit to
int cur_ms = 0;           // Initiallization for current ms time in the loop

void loop()

{

  int start_ms = millis();

  /*
   * All digits are on except the current digit for the first 200ms.
   * Then they are all on for the remaining 800ms.
   */
  while(cur_ms - start_ms < 1000) {
    for(int i = 0; i < 3; i++) {
      if(cur_ms - start_ms < 200 && i != current_digit) {
        setDigit(i);     
      } else if(cur_ms - start_ms >=  200){
        setDigit(i);
      }
    }
    cur_ms = millis();
  }

  // Get the position values from the joystick
  int y_direction = analogRead(A1);
  int x_direction = analogRead(A0);
  
  Serial.print("Y: ");
  Serial.println(y_direction); // Default is 524
  Serial.print("X: ");
  Serial.println(x_direction); // Default is 493

  // Some buffer from defaults to avoid it changing accidentally
  if(x_direction < 470 && current_digit >= 0) { 
    if(current_digit > 0) current_digit --;
    else current_digit = 2; // Scrolling off goes to the other side 
    current_value = digits[current_digit];
  } 
  else if(x_direction > 515) {
    if(current_digit < 2) current_digit++;
    else current_digit = 0; // Scrolling off goes to the other side 
    current_value = digits[current_digit];
  }
  if(y_direction < 500 && current_value >= 0) {
    if(current_value > 0) current_value--; 
    else current_value = 15; // Scrolling under zero goes to F (15)
    digits[current_digit] = current_value;
  }
  else if(y_direction > 550) {
    if(current_value < 15) current_value++;
    else current_value = 0; // Scrolling over goes to 0
    digits[current_digit] = current_value;
  }

  Serial.print("current_digit: ");
  Serial.println(current_digit);
  Serial.print("current_value: ");
  Serial.println(current_value);
  
}
