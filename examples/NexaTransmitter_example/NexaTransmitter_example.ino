#include <NexaTransmitter.h>

NexaTransmitter remote(2,  57585565); 
void setup() {
    Serial.begin(9600);
    Serial.println("start");

    // switch on the unit to high brightness (first unit of the first channel)
    remote.setSwitch(true, 1, 1, 1);
    delay(3000);                      // wait
    remote.setSwitch(true, 1, 1, 8);  // dim to half
    delay(3000);
    remote.setSwitch(true, 1, 1, 15); // dim to lowest
    delay(3000);
    remote.setSwitch(false, 1, 1);    // switch off the unit 

    Serial.println("stop");        // done. 
}

void loop() {
  // do nothing
}


