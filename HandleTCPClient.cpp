#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for recv() and send() */
#include <unistd.h>     /* for close() */
#include <string.h>     /* for strlen() */
#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <sstream>
#include "httpMessage.h"

#define RCVBUFSIZE 1024   /* Size of receive buffer */

using namespace std;

void DieWithError(const char *errorMessage);  /* Error handling function */
string getHeader(int conLen, string type);
string getDocumentText(string documentRoot, string file);
void sendResponse(const char* firstLine, int clntSocket, string documentRoot);


void HandleTCPClient(int clntSocket, string documentRoot)
{
    char echoBuffer[RCVBUFSIZE];        /* Buffer for echo string */
    int recvMsgSize;                    /* Size of received message */
    string firstLine = "";              /* First line of the HTTP request header */
    bool sent = false;                  /* Keep track of whether response has been sent */

    /* Receive message from client */
    if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
        DieWithError("recv() failed");

    /* Send received string and receive again until end of transmission */
    do
    {
        // If the first line has not been read yet, search for the first new line
        if (firstLine == "") {
            for(unsigned int i = 0; i < strlen(echoBuffer); i++) {
                if( echoBuffer[i] == '\r' ) {

                    char* substr;
                    substr = new char [i+1];

                    // Copy everything up to the first new line into temperary c string
                    // and then assign it to variable to keep track of HTTP request
                    strncpy(substr, echoBuffer, i);
                    substr[i] = '\0';

                    firstLine = substr;
                    delete substr;

                    cout << firstLine << endl;
                    break;
                }
            }   
        }
        
        // Send response if request is read and response has not yet been sent
        if(firstLine != "" && !sent) {
                sendResponse(firstLine.c_str(), clntSocket, documentRoot);
                sent = true;
            
        }

        /* See if there is more data to receive */
        if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
            DieWithError("recv() failed");
    } while(recvMsgSize == RCVBUFSIZE);

    cout << "Close" << endl;
    close(clntSocket);    /* Close client socket */
}

// Send the server response back to the client.
void sendResponse(const char* firstLine, int clntSocket, string documentRoot) {
    // Create object for parsing and holding the request data
    HttpMessage request(firstLine);

    // Only send response for a GET request. Do nothing for all other methods
    if(request.method == "GET") {
        string content = getDocumentText(documentRoot, request.file);
        string header = getHeader(content.length(), request.type);
        content = header + content;

        int len = content.length();

        // Send the header plus the content
        int val = send(clntSocket, content.c_str(), len, 0);

        // Make sure everything was sent or quit with error
        if( val != len)
            DieWithError("send() failed");
            //printf("Send returned %d", val);
    }
}

// Get the HTTP header based on content length and content type.
string getHeader(int conLen, string type) {
    stringstream header;

    // If the content length is 0 than a file could not be opened
    // so send 404 information.
    if( conLen == 0 ) {
        header << "HTTP/1.0 404 Not Found\r\n";
        header << "Content-Type:text/html\r\n";
        header << "Content-Length:40\r\n";
        header << "\r\n";
        header << "<html><h1>404 File Not Found</h1></html>";
    }
    
    // Otherwise assume the connection is OK and set content type and length
    else {
        header << "HTTP/1.0 200 OK\r\n";
        header << "Content-Type:" << type << "\r\n";
        header << "Content-Length:" << conLen << "\r\n";
        header << "Connection:close\r\n";
        header << "\r\n";
    }

    return header.str();
}

// Get string with the contents of the requested file given document root
// directory and the relative file path.
string getDocumentText(string documentRoot, string file) {
    // Handle root directory input by redirecting it to index.html
    if( file == "/" )
        file = "/index.html";

    string filename = documentRoot + file;

    // Open an input filestream as binary data so it handles images correctly
    ifstream document(filename.c_str(), ifstream::binary);
    stringstream buffer;

    // Read the file into stringstream
    buffer << document.rdbuf();

    // Return the string
    return buffer.str();
}

