#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "secrets.h" 


//define namie of components 
#define DEVICE_NAME "Device 1"
AsyncWebServer server(80);
String sensorhtml = "<script onload='clearInterval(myInterval);'>const sensors=[];let myInterval=setInterval(changeValues, 1000);async function request (){const response = await fetch('data');return response.json()} function changeValues (){request().then(data=>{dt=data;for(let x in sensors){document.getElementById(sensors[x]).innerHTML=dt[sensors[x]];};});};</script>";

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


Sensor sensors[2] = {
    Sensor(A1, "Brightness", "%", 100, -0.0244), //Photeresistior at A1
    Sensor(D9, "Door")
    }; 


//css, js and html separated for easier editing
const String index_html = 
    "<style>div{width:100%; diplay: flex;float:left;}div.sensor{border-style: inset;width: 8em; padding: 0.8em;}</style>"
    "<h1>"DEVICE_NAME"</h1>"
    "<div><h2>Sensoren</h2></div>"
    "<div>"+sensorhtml+"</div>";
    ;



void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}
void startWebinterface(){
    Serial.begin(9600);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf("WiFi Failed!\n");
        return;
    }

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", index_html);
    });
    server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
        String json = "{";
        for(int i=0; i<sizeof(sensors)/sizeof(*sensors); i++){
            if (i != 0) json += ", ";
            json += "\"" + sensors[i].name + "\": \"" + sensors[i].value  + "\"";
        }
        json += "}";
        request->send(200, "text/plain", json);
    });
    

    server.onNotFound(notFound);

    server.begin();
}