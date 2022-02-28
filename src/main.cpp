//
// A simple server implementation giving 
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//

#include "webinterface.h"


int variable = 0;



void setup() {
    Webinterface();
    
    index_html = 
        "<!DOCTYPE html>"
        "<html>"
            "<head>"
                "<style>"
                    "body{"
                        "font-family: Arial, Helvetica, sans-serif;"
                        "font-size: large;"
                        "color: azure;"
                        "background-color: #404a53;"
                    "}"
                    "#myVariable{"
                        "font-size: 10em;"
                    "}"
                    "code{"
                        "color: black;"
                        "background-color: white;"
                        "padding: 0.5em;"
                    "}"
                "</style>"
            "</head>"
            "<body>"
                "<h1>This is an example for a simple webinterface</h1>"
                "<h2>Variables</h2>"
                "<p>to link variables to the update queue use <code>addVariable(id, variable)</code></p>"
                "<div>"+addVariable("myVariable", variable)+"</div>"
                "<h2>HTML Forms</h2>"
                "<p>To controll variables, use HTML Forms:<br><br>"
                "<code>addForm(id, Variable)</code> = "+addForm("myForm", variable)+"<br><br>"
                "<code>addSlider(id, variable)</code> = "+addSlider("mySlider", variable)+"<br><br>"
                "<code>addCheckbox(id, variable)</code> = "+addCheckbox("myCheckbox", variable)+"</p>"
            "</body>"
        "</html>";

}

void loop() {
    delay(800);
}