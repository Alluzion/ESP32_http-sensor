//
// A simple server implementation giving 
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//

#include "webinterface.h"
int wert = 0;
int status = 0;

void setup() {
    startWebinterface();
    addVariable(wert, "timer");
    addVariable(status, "Status: ");
    index_html = 
        "<style>div{width:100%; diplay: flex;float:left;}div.variable{border-style: inset;width: 8em; padding: 0.8em;}</style>"
        "<h1>Lieblings-ESP</h1>"
        "<div><h2>Variablen: </h2></div>"
        "<div>"+VARIABLES+"</div>";
}

void loop() {
    wert ++;
    delay(800);
}