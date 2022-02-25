#include <Arduino.h>
class Sensor{
    public:
    String name;
    int pin;
    int value = 0;
    Sensor(String NAME, int PIN) :name{NAME}, pin{PIN}{

        pinMode(pin, INPUT);
    }

    void pullup(){
        pinMode(pin, INPUT_PULLUP);
    }

    void digital(){
        value = digitalRead(pin);
    }

    void analog(){
        value = analogRead(pin);
    }
};