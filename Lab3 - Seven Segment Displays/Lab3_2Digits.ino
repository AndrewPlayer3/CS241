const int pinA = 9;
const int pinB = 8;
const int pinC = 4;
const int pinD = 5;
const int pinE = 6;
const int pinF = 10;
const int pinG = 3;
const int pinP = 7;

int pins[] = {pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinP};

const int nseg = 8;
const int nsym = 64;

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


const int DIG1 = 11;
const int DIG2 = 12;

int digits[] = {8, 2};
int digit_pins[] = {DIG1, DIG2};

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


void setDigit(int digit) {
  digitalWrite(digit_pins[digit], LOW);
  for(int i = 0; i < 3; i++) {
    if(i != digit) {
      digitalWrite(digit_pins[i], HIGH);
    }
  }
  setPattern(digits[digit]);
  delay(1);
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

}


void loop()

{
  for(int i = 0; i < 2; i++) {
    setDigit(i);
  }
}
