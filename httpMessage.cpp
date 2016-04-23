#include <sys/socket.h>
#include <string>
#include "httpMessage.h"
#include <sstream>
#include <iostream>


// Construct HttpMessage object
HttpMessage::HttpMessage(const char* input) {
    std::stringstream ssInput(input);
    std::string parts[3];

    // Put each part of request into array
    for( int i = 0; i < 3; i++) {
        ssInput >> parts[i];
    }

    // Assign class properties based on HTTP method and filename
    method = parts[0];
    file = parts[1];

    // If the file is long enough to have an extension, get the
    // extension and set the content type property based on that.
    if(file.length() >= 4) {
        std::string extension = file.substr(file.length() - 4);

        if( extension == "html")
            type = "text/html";
        else if(extension == ".txt")
            type = "text/txt";
        else if(extension == ".css")
            type = "text/css";
        else if(extension == ".jpg")
            type = "image/jpeg";
        else if(extension == ".gif")
            type = "image/gif";
    }

    // Default to html
    else type = "text/html";
}
