#include <Arduino.h>

String sensorhtml = "";

class Sensor{
    public:
    int pin;
    String name;
    String unit;
    float offset;
    float factor;
    String value;
    Sensor(int PIN, String NAME) : pin{PIN}, name{NAME}{
        sensorhtml += "<div class='sensor'>"+name+" <a style='float:right;' id='"+name+"'>-</a></div><script>sensors.push('"+name+"'); </script>";
        pinMode(pin, INPUT);
    }
    Sensor(int PIN, String NAME, String UNIT, float OFFSET, float FACTOR) : pin{PIN}, name{NAME}, unit{UNIT}, offset{OFFSET}, factor{FACTOR}{
        sensorhtml += "<div class='sensor'>"+name+" <a style='float:right;' id='"+name+"'>-</a></div><script>sensors.push('"+name+"'); </script>";
        pinMode(pin, INPUT);
    }

    void pullup(){
        pinMode(pin, INPUT_PULLUP);
    }

    bool digital(){
        if(digitalRead(pin)){
            value = "on";
            return true;
        }
        else{
            value = "of";
            return false;
        }
        
    }

    void analog(){
        value =  String(factor*analogRead(pin) + offset, 0) + unit;
    }
};