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


// define naming of components 
#define DEVICE_NAME "Raum 1"
Sensor sensors[3] = {Sensor("brightness", A3), Sensor("pressure", A2), Sensor("temperature", A1)}; //sensordata as an Object from sensor.h for sake of scalability








//css, js and html separated for easier editing
const String html = {
    "<style>.sensor{border-style: inset;width: fit-content;padding: 0.8em;}</style>"
    "<script onload='clearInterval(myInterval);'>let myInterval=setInterval(changeValues, 1000);async function request (){const response = await fetch('data');return response.json()} function changeValues (){request().then(data=>{document.getElementById('brightness').innerHTML=data.brightness;});};</script>"
    "<body><h1>"DEVICE_NAME"</h1>"
    "<div style='diplay: flex;'>"
    "<h2>Sensoren</h2>"
    "<div class='sensor'>"+sensors[0].name+" <a id='"+sensors[0].name+"'>-</a></div>"
    "<div class='sensor'>"+sensors[1].name+" <a id='"+sensors[1].name+"'>-</a></div>"
    "<div class='sensor'>"+sensors[2].name+" <a id='"+sensors[2].name+"'>-</a></div>"
    "</div>"
    "<div>"
    "<h2>Steuerelemente</h2>"
    "</div><body>"
    };
    
AsyncWebServer server(80);

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void setup() {
    


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
        request->send(200, "text/html", html);
    });

    server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", "{\"brightness\": \"" + String(100*(4096 - sensors[0].value )/4096) + " %\"}");
    });
   

    server.onNotFound(notFound);

    server.begin();
}

void loop() {
    sensors[0].analog();
    delay(1000);
}