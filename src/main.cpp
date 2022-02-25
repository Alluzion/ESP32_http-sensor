//
// A simple server implementation giving 
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "sensor.h"

#include "secrets.h" 


//define namie of components 
#define DEVICE_NAME "Device 1"
Sensor sensors[2] = {
    Sensor(A1, "Brightness", "%", 100, -0.0244), //Photeresistior at A1
    Sensor(D9, "Door")
    }; 





//css, js and html separated for easier editing
const String index_html = 
    "<style>div{width:100%; diplay: flex;float:left;}div.sensor{border-style: inset;width: 8em; padding: 0.8em;}</style>"
    "<script onload='clearInterval(myInterval);'>const sensors=[];let myInterval=setInterval(changeValues, 1000);async function request (){const response = await fetch('data');return response.json()} function changeValues (){request().then(data=>{dt=data;for(let x in sensors){document.getElementById(sensors[x]).innerHTML=dt[sensors[x]];};});};</script>"
    "<h1>"DEVICE_NAME"</h1>"
    "<div><h2>Sensoren</h2></div>"
    "<div>"+sensorhtml+"</div>";
    ;
    
AsyncWebServer server(80);

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void setup() {
    Serial.begin(9600);
    sensors[1].pullup();

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

void loop() {
    sensors[0].analog();
    sensors[1].digital();
    delay(200);
}