#include <Arduino.h>

String sensorhtml = "";

class Sensor{
    public:
    String name;
    int pin;
    int value = 0;
    Sensor(String NAME, int PIN) :name{NAME}, pin{PIN}{
        sensorhtml += "<div class='sensor'>"+name+" <a style='float:right;' id='"+name+"'>-</a></div>";
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