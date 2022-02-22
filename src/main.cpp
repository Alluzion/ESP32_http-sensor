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
/*
*/


#define SENSOR_PIN1 A3






AsyncWebServer server(80);


const char* PARAM_MESSAGE = "message";

int sensorValue = 0;

const String htmlPage = "<h3>Sensor 1: </h3><object id='value' data='sensorValue' height='45' width='80'></object><script onload='clearInterval(myVar)'>var obj = document.getElementById('value');let myVar = setInterval(myTimer, 5000);function myTimer(){obj.data=obj.data;};</script>";


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
        request->send(200, "text/html", "<p>" + String(sensorValue) + "</p>");
    });
   

    server.onNotFound(notFound);

    server.begin();
}

void loop() {
sensorValue = analogRead(SENSOR_PIN1);
delay(1000);
}