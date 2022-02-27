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
    "const checkboxStates = {};"
    "let myInterval=setInterval(updateVariables, " REFRESH_INTERVALL ");"
    "async function requestVariableState (){const response = await fetch('data');return response.json()};"
    "function updateVariables (){requestVariableState().then(data=>{dt=data;for(let x in variables){document.getElementById(variables[x]).innerHTML=dt[variables[x]];};});};"
    "async function requestState (path){var x = await fetch(path);};"
    "function updateCheckbox (checkboxId){if(checkboxStates[checkboxId]){checkboxStates[checkboxId]=0;}else{checkboxStates[checkboxId]=1;};requestState('change?'+checkboxId+'='+checkboxStates[checkboxId]);};"//
    "function updateSlider (sliderId){requestState('change?'+sliderId+'='+document.getElementById(sliderId).value);};"
    "</script>";


int checkboxCount = 0;
int* checkboxes[SWITCH_SPACE];

String addCheckbox(int &value){
    checkboxes[checkboxCount] = &value;
    String id = "checkbox"+String(checkboxCount);
    checkboxCount++;
    return "<input id='"+id+"' type='checkbox' onchange=\"updateCheckbox ('"+id+"');\"></input><script>checkboxStates[\""+id+"\"]="+String(*checkboxes[checkboxCount-1])+";</script>";
}


String addSlider(int &value){
    checkboxes[checkboxCount] = &value;
    String id = "slider"+String(checkboxCount);
    checkboxCount++;
    return "<input id='"+id+"' type='range' min='0' max='100' onchange=\"updateSlider ('"+id+"');\"></input>";
}

String addNumber(int &value){
    checkboxes[checkboxCount] = &value;
    String id = "slider"+String(checkboxCount);
    checkboxCount++;
    return "<input id='"+id+"' type='number' onchange=\"updateSlider ('"+id+"');\"></input>";
}






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
    server.on("/change", HTTP_GET, [](AsyncWebServerRequest *request) {
        for (int i=0; i<checkboxCount; i++){
        if (request->hasParam("checkbox"+String(i))) {
            if(request->getParam("checkbox"+String(i))->value().toInt()){
                *checkboxes[i] = 1;
                
            }
            else{
                *checkboxes[i] = 0;
            }            
        }
        else if(request->hasParam("slider"+String(i))) {
            *checkboxes[i]=request->getParam("slider"+String(i))->value().toInt();
        }
        }
        request->send(206, "text/plain", "");
    });
    

    server.begin();
    
    Serial.begin(9600);
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    return true;
}