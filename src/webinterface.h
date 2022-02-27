#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "secrets.h" 

#define REFRESH_INTERVALL "1000"
#define CLASSNAME "variable"
#define VARIABLE_SPACE 5
#define SWITCH_SPACE 5

#define BUTTONCSS_OFF "background-color: lightcoral; border-style: inset;"
#define BUTTONCSS_ON "background-color: lightgreen; border-style: outset;"


String index_html; //MainPage can be eddited after all variables have been added
const String JS = 
    "<script onload='clearInterval(myInterval);'>"
    "const variables=[];"
    "const switchStates = {};"
    "let myInterval=setInterval(updateVariables, " REFRESH_INTERVALL ");"
    "async function requestVariableState (){const response = await fetch('data');return response.json()};"
    "function updateVariables (){requestVariableState().then(data=>{dt=data;for(let x in variables){document.getElementById(variables[x]).innerHTML=dt[variables[x]];};});};"
    "async function requestSwitchState (path){var x = await fetch(path);};"
    "function updateSwitch (switchName){if(switchStates[switchName]){switchStates[switchName]=0;document.getElementById(switchName+'Switch').style='" BUTTONCSS_OFF ";';}else{switchStates[switchName]=1;document.getElementById(switchName+'Switch').style='" BUTTONCSS_ON "';};requestSwitchState('switch?'+switchName+'='+switchStates[switchName]);};"//
    "</script>";


int switchCount = 0;
String sNames[SWITCH_SPACE];
int* switches[SWITCH_SPACE];

String addSwitch(String name, int &value){
    switches[switchCount] = &value;
    sNames[switchCount] = name;
    switchCount++;
    
    return "<button id='"+name+"Switch' style='" BUTTONCSS_OFF "' onclick=\"updateSwitch ('"+name+"');\">"+name+"</button><script>switchStates[\""+name+"\"]="+String(*switches[switchCount-1])+";</script>";
}


/*
void (*function)();

String addFunction(void (&fcnPtr)()){
    function = (&fcnPtr)();
    return "<button id='function' onclick=\"updateSwitch ('"+name+"');\">"+name+"</button><script>switchStates[\""+name+"\"]="+String(*switches[switchCount-1])+";</script>";
}
*/





int variableCount = 0;
String vNames[VARIABLE_SPACE]; //All the vNames are saved here

//Pointer-arrays for useful Data-Types
int* integers[VARIABLE_SPACE];
float* floats[VARIABLE_SPACE];
String* strings[VARIABLE_SPACE];

//Adding a variable to the update-cycle
String addVariable(String NAME, int &VALUE){ 
    integers[variableCount] = &VALUE; //Save the Reference to the corresponding array
    vNames[variableCount] = NAME; //Save the Name to the vNames array
    variableCount++; //Stepusp variableCount to keep track of array-size
    return "<div class='" CLASSNAME "'>"+NAME+" <a id='"+NAME+"'>-</a></div><script>variables.push('"+NAME+"'); </script>";
}
//See Above but for floats
String addVariable(String NAME, float &VALUE){
    floats[variableCount] = &VALUE;
    vNames[variableCount] = NAME;
    variableCount++;
    return "<div class='" CLASSNAME "'>"+NAME+" <a id='"+NAME+"'>-</a></div><script>variables.push('"+NAME+"'); </script>";
}
//See Above but for Strings
String addVariable(String NAME, String &VALUE){
    strings[variableCount] = &VALUE;
    vNames[variableCount] = NAME;
    variableCount++;
    return "<div class='" CLASSNAME "'>"+NAME+" <a id='"+NAME+"'>-</a></div><script>variables.push('"+NAME+"'); </script>";
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
        request->send(200, "text/html",  JS + index_html);
    });

    //data-api for live-updates
    server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
        String json = "{";
        for(int i=0; i<variableCount; i++){
            if (i != 0) json += ", ";
            if (integers[i]){
                json += "\"" + vNames[i] + "\": " + String(*integers[i]);
            }
            if (floats[i]){
                json += "\"" + vNames[i] + "\": " + String(*floats[i]);
            }
            if (strings[i]){
                json += "\"" + vNames[i] + "\": \"" + String(*strings[i])  + "\"";
            }
        }
        json += "}";
        request->send(200, "text/plain", json);
    });


    
    //address for setting values with buttons and forms
    server.on("/switch", HTTP_GET, [](AsyncWebServerRequest *request) {
        for (int i=0; i<switchCount; i++)
        if (request->hasParam(sNames[i])) {
            if(request->getParam(sNames[i])->value().toInt()){
                *switches[i] = 1;
                
            }
            else{
                *switches[i] = 0;
            }            
        }
        request->send(206, "text/plain", "success");
    });
    

    server.begin();
    
    Serial.begin(9600);
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    return true;
}