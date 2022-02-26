#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "secrets.h" 

#define REFRESH_INTERVALL "500"
#define CLASSNAME "variable"
#define VARIABLE_SPACE 10

String index_html;
String VARIABLES = "<script onload='clearInterval(myInterval);'>const variables=[];let myInterval=setInterval(changeValues, "REFRESH_INTERVALL");async function request (){const response = await fetch('data');return response.json()};function changeValues (){request().then(data=>{dt=data;for(let x in variables){document.getElementById(variables[x]).innerHTML=dt[variables[x]];};});};</script>";



int counter = 0;
String names[VARIABLE_SPACE];


int* integers[VARIABLE_SPACE];
float* floats[VARIABLE_SPACE];
String* strings[VARIABLE_SPACE];

void addVariable(int &VALUE, String NAME){ 
    integers[counter] = &VALUE;
    names[counter] = NAME;
    VARIABLES += "<div class='"CLASSNAME"'>"+NAME+" <a id='"+NAME+"'>-</a></div><script>variables.push('"+NAME+"'); </script>";
    counter++;
    index_html = VARIABLES;
}
void addVariable(float &VALUE, String NAME){
    floats[counter] = &VALUE;
    names[counter] = NAME;
    VARIABLES += "<div class='"CLASSNAME"'>"+NAME+" <a id='"+NAME+"'>-</a></div><script>variables.push('"+NAME+"'); </script>";
    counter++;
    index_html = VARIABLES;
}
void addVariable(String &VALUE, String NAME){
    strings[counter] = &VALUE;
    names[counter] = NAME;
    VARIABLES += "<div class='"CLASSNAME"'>"+NAME+" <a id='"+NAME+"'>-</a></div><script>variables.push('"+NAME+"'); </script>";
    counter++;
    index_html = VARIABLES;
}


String json(){
        String json = "{";
        for(int i=0; i<counter; i++){
            if (i != 0) json += ", ";
            if (integers[i]){
                json += "\"" + names[i] + "\": " + String(*integers[i]);
            }
            if (floats[i]){
                json += "\"" + names[i] + "\": " + String(*floats[i]);
            }
            if (strings[i]){
                json += "\"" + names[i] + "\": \"" + String(*strings[i])  + "\"";
            }
        }
        json += "}";
        return json;
    }

AsyncWebServer server(80);

bool startWebinterface(){

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        return false;
    }

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", index_html);
    });

    server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", json());
    });

    server.begin();
    
    Serial.begin(9600);
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    return true;
}