//
// A simple server implementation giving 
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//

#include "sensor.h"


void setup() {
    startWebinterface();
}

void loop() {
    sensors[0].analog();
    sensors[1].digital();
    delay(200);
}