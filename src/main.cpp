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

#include "secrets.h" 



#define SENSOR_PIN1 A3






AsyncWebServer server(80);


const char* PARAM_MESSAGE = "message";

int sensorValue = 0;

const String htmlPage =  "<script>async function request (){const response = await fetch('http://192.168.2.160/sensorValue');return response.json()} function something (){request().then(data=>{document.getElementById('sensorValue').innerHTML=data.brightness;setTimeout(something,1000)});};  </script><button onclick='something()'>Der testbutton</button><h3>Sensoren Raum 1: </h3><p>Brightness <a id='sensorValue'></a></p>";
//"<h3>Sensoren Raum 1: </h3><p>Brightness <object id='value' data='sensorValue' height='40' width='80'></object></p><script onload='clearInterval(intervall)'>var obj = document.getElementById('value');let intervall = setInterval(reloadObj, 2500);function reloadObj(){obj.data=obj.data;};</script>";

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void setup() {
    pinMode(SENSOR_PIN1, INPUT);
    


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
        request->send(200, "text/html", String(htmlPage));
    });

    server.on("/sensorValue", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", "{\"brightness\": \"" + String(sensorValue) + " %\"}");
    });
   

    server.onNotFound(notFound);

    server.begin();
}

void loop() {
sensorValue = 100*(4096 - analogRead(SENSOR_PIN1))/4096;
delay(1000);
}