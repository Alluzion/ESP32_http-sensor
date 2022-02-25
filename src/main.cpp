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
Sensor sensors[3] = {Sensor("Brightness", A1), Sensor("Pressure", A2), Sensor("Temperture", A3)}; //sensordata as an Object from sensor.h for sake of scalability






//css, js and html separated for easier editing
const String index_html = {
    "<style>div{width:100%; diplay: flex;float:left;}div.sensor{border-style: inset;width: 10em; padding: 0.5em;}</style>"
    "<script onload='clearInterval(myInterval);'>const sensors = ['"+sensors[0].name+"','"+sensors[1].name+"','"+sensors[2].name+"'];let myInterval=setInterval(changeValues, 1000);async function request (){const response = await fetch('data');return response.json()} function changeValues (){for(let x in sensors){update(sensors[x]);}} function update(valueId){request().then(data=>{document.getElementById(valueId).innerHTML=data[valueId];});};</script>"
    "<h1>"DEVICE_NAME"</h1>"
    "<div><h2>Sensoren</h2></div>"
    "<div>"+sensorhtml+"</div>"
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
        request->send(200, "text/html", index_html);
    });

    server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/json", "{\""+sensors[0].name+"\": \"" + String(100*(4096 - sensors[0].value )/4096) + " %\", \""+sensors[1].name+"\": \"" + String(sensors[1].value) + "\", \""+sensors[2].name+"\": \"" + String(sensors[2].value) + "\"}");
    });
   

    server.onNotFound(notFound);

    server.begin();
}

void loop() {
    sensors[0].analog();
    sensors[1].analog();
    sensors[2].analog();
    delay(1000);
}