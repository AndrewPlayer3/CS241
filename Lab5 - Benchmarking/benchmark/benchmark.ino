#include "benchmark.h" //<- Dr. Lawlor's code

void setup() {
  Serial.begin(9600);
}

// Define functions to be benchmarked.
// These need to take int and return int
int retZero(int v) {
  return 0;
}

int writeOnePin(int v) {
  digitalWrite(8, v);
  return 0;
}

int writeManyPins(int v){
  digitalWrite(8, v);
  digitalWrite(9, v);
  digitalWrite(10, v);
  digitalWrite(11, v);
  digitalWrite(12, v);
  digitalWrite(13, v);
  return 0;
}

int pins[] = {8, 9, 10, 11, 12, 13};
int digitalWrite6Table(int v) {
  for (int i = 0; i < 6; i++) {
    digitalWrite(pins[i], v);
  }
  return 0;
}

int writePorts(int v){
  PORTB=0b00111111;
  return 0;
}

int digitalReadPin(int v) {
  return digitalRead(8);
}

int readPorts(int v) {
  return PINB;
}

int analogReadPin(int v) {
  return analogRead(A0);
}

int delayMicroseconds1(int v) {
  delayMicroseconds(1);
}

int delayMicroseconds10(int v) {
  delayMicroseconds(10);
}

int delayMilliseconds1(int v) {
  delay(1);
}

int andInt(int v) {
  return v & 3;
}

int addInt(int v) {
  return v + 3;
}

int mulInt(int v) {
  return v * 3;
}

int divInt(int v) {
  return int(v / 3);
}

int addFloat(int v) {
  return int(v + 3.1415926535) ;
}

int serialPrint(int v) {
  Serial.print(char(0));
}

void loop() {
  Serial.println("Starting benchmarks...");
  
  //Call benchmark on our functions
  //benchmark("retZero",retZero,10000);
  //benchmark("writeOnePin",writeOnePin,10000);
  //benchmark("writeManyPins",writeManyPins,10000);
  //benchmark("digitalWrite6Table",digitalWrite6Table,10000);
  //benchmark("writePorts",writePorts,10000);
  //benchmark("digitalReadPin",digitalReadPin,10000);
  //benchmark("readPorts",readPorts,10000);
  //benchmark("analogReadPin",analogReadPin,10000);
  //benchmark("delayMicroseconds1", delayMicroseconds1, 10000);
  //benchmark("delayMicroseconds10", delayMicroseconds10, 10000);
  //benchmark("delayMilliseconds1", delayMilliseconds1, 1000);
  //benchmark("andInt", andInt, 10000);
  //benchmark("addInt", addInt, 10000);
  //benchmark("mulInt", mulInt, 10000);
  //benchmark("divInt", divInt, 10000);
  //benchmark("addFloat", addFloat, 10000);
  //benchmark("serialPrint", serialPrint, 10000);
  
  Serial.println();
  delay(4000);
}
