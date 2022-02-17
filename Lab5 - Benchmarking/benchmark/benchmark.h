/*
 Arduino header for benchmarking (timing) short functions.
 
 Dr. Orion Lawlor, lawlor@alaska.edu, 2021-02-10 (Public Domain)
*/

// This is the interface for benchmarkable functions.
//   You write these functions as:
//    int yourFunction(int value) { ... }
typedef int (*benchmarkable)(int value);

// This is used to benchmark a function call.
//  It prints the result to the serial port.
void benchmark(const char *name,
               int (*fn)(int value),
               int nRepeat)
{
  Serial.print(name);

  long start=micros(); // start stopwatch
  
  volatile int value=0;
  for (int repeat=0;repeat<nRepeat;repeat++)
    value = fn(value);
  
  long end=micros(); // end stopwatch
  long usTotal=end-start;
  float usPerCall=((float)usTotal)/nRepeat;
  
  Serial.print("\t");
  Serial.print(usPerCall);
  Serial.print("\tus per call\n");
}


