#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "secrets.h" 

#define REFRESH_INTERVALL "1000"
#define CLASSNAME "variable"
#define VARIABLE_SPACE 5
#define SWITCH_SPACE 5

String index_html; //MainPage can be eddited after all variables have been added
const String JS_updateValues = 
    "<script onload='clearInterval(myInterval);'>"
    "const variables=[];"
    "let myInterval=setInterval(changeValues, "REFRESH_INTERVALL");"
    "async function request (){const response = await fetch('data');"
    "return response.json()};"
    "function changeValues (){request().then(data=>{dt=data;for(let x in variables){document.getElementById(variables[x]).innerHTML=dt[variables[x]];};});};"
    "</script>";


int switchCount = 0;
int* switches[SWITCH_SPACE];

String addSwitch(int &value){
    switches[switchCount] = &value;
    String s = String(switchCount);
    switchCount++;
    
    return 
    "<script>let switch"+s+" = 0;function onSwitch"+s+"(){if(switch"+s+"){switch"+s+"=0;};else{switch"+s+"=1;};await fetch('switch?switch"+s+"='+switch"+s+");}</script><label class='switch'><input type='checkbox' onclick='onSwitch"+s+"()'></label>";
}





int variableCount = 0;
String names[VARIABLE_SPACE]; //All the names are saved here

//Pointer-arrays for useful Data-Types
int* integers[VARIABLE_SPACE];
float* floats[VARIABLE_SPACE];
String* strings[VARIABLE_SPACE];

//Adding a variable to the update-cycle
String addVariable(int &VALUE, String NAME){ 
    integers[variableCount] = &VALUE; //Save the Reference to the corresponding array
    names[variableCount] = NAME; //Save the Name to the names array
    variableCount++; //Stepusp variableCount to keep track of array-size
    return "<div class='"CLASSNAME"'>"+NAME+" <a id='"+NAME+"'>-</a></div><script>variables.push('"+NAME+"'); </script>";
}
//See Above but for floats
String addVariable(float &VALUE, String NAME){
    floats[variableCount] = &VALUE;
    names[variableCount] = NAME;
    variableCount++;
    return "<div class='"CLASSNAME"'>"+NAME+" <a id='"+NAME+"'>-</a></div><script>variables.push('"+NAME+"'); </script>";
}
//See Above but for Strings
String addVariable(String &VALUE, String NAME){
    strings[variableCount] = &VALUE;
    names[variableCount] = NAME;
    variableCount++;
    return "<div class='"CLASSNAME"'>"+NAME+" <a id='"+NAME+"'>-</a></div><script>variables.push('"+NAME+"'); </script>";
}




// This is where the webserver is initialized by the standard of the ESP-Async library
AsyncWebServer server(80);

bool startWebinterface(){
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        return false;
    }

    //main page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html",  JS_updateValues + index_html);
    });

    //data-api for live-updates
    server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
        String json = "{";
        for(int i=0; i<variableCount; i++){
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
        request->send(200, "text/plain", json);
    });


    
    //address for setting values with buttons and forms
    server.on("/switch", HTTP_GET, [] (AsyncWebServerRequest *request) {
        for (int i=0; i<switchCount; i++)
        if (request->hasParam("switch"+String(i))) {
            if(request->getParam("switch"+String(i))->value().toInt()){
                *switches[i] = 1;
                
            }
            else{
                *switches[i] = 0;
            }
            Serial.println(*switches[i]);
        }
        
        request->send(200, "text/html", "antwort");
    });
    

    server.begin();
    
    Serial.begin(9600);
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    return true;
}