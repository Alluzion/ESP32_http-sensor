#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "secrets.h" 

int wert = 0;


AsyncWebServer server(80);

String html = "<script onload='clearInterval(myInterval);'>const sensors=[];let myInterval=setInterval(changeValues, 1000);async function request (){const response = await fetch('data');return response.json()};function changeValues (){request().then(data=>{dt=data;for(let x in sensors){document.getElementById(sensors[x]).innerHTML=dt[sensors[x]];};});};</script>";

class liveData{
    public:
    int* value;
    String name;
    liveData(int &VALUE, String NAME){
        value = &VALUE;
        name = NAME;
        html += "<div class='sensor'>"+name+" <a id='"+name+"'>-</a></div><script>sensors.push('"+name+"'); </script>";
    }
};

liveData linked_values[1]={liveData(wert,"name")};

String json(){
        String json = "{";
        for(int i=0; i<1; i++){
            if (i != 0) json += ", ";
            json += "\"" + linked_values[i].name + "\": \"" + String(*linked_values[i].value)  + "\"";
        }
        json += "}";
        return json;
    }


void startWebinterface(){
    Serial.begin(9600);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf("Webinterface Failed!\n");
        return;
    }

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", html);
    });

    server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", json());
    });

    server.begin();
}