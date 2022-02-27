//
// A simple server implementation giving 
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//

#include "webinterface.h"


int wert = 0;
String status = "eios";

void setup() {
    startWebinterface();
    index_html = 
        "<style></style>"
        "<h1>Lieblings-ESP</h1>"
        "<input onchange='logSlider()' type='checkbox'' id='myRange'><script>function logSlider(){console.log(document.getElementById('myRange').value);}</script>"
        "<div><h2>Variablen: </h2></div>"
        "<div>"+addVariable("timer", wert)+addCheckbox(wert)+addSlider(wert)+"</div>";
}

void loop() {
    delay(800);
}