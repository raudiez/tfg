#include "RestClient.h"

RestClient::RestClient(const char* _host){
  host = _host;
  port = 80;
  num_headers = 0;
  contentType = "x-www-form-urlencoded";	// default
}

RestClient::RestClient(const char* _host, int _port){
  host = _host;
  port = _port;
  num_headers = 0;
  contentType = "x-www-form-urlencoded";	// default
}

void RestClient::dhcp(){
  byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
  if (begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
  }
  //give it time to initialize
  delay(1000);
}

int RestClient::begin(byte mac[]){
  return Ethernet.begin(mac);
  //give it time to initialize
  delay(1000);
}

// GET path
int RestClient::get(const char* path){
  return request("GET", path, NULL, NULL);
}

//GET path with response
int RestClient::get(const char* path, char* response){
  return request("GET", path, NULL, response);
}

// POST path and body
int RestClient::post(const char* path, const char* body){
  return request("POST", path, body, NULL);
}

// POST path and body with response
int RestClient::post(const char* path, const char* body, char* response){
  return request("POST", path, body, response);
}

// PUT path and body
int RestClient::put(const char* path, const char* body){
  return request("PUT", path, body, NULL);
}

// PUT path and body with response
int RestClient::put(const char* path, const char* body, char* response){
  return request("PUT", path, body, response);
}

// DELETE path
int RestClient::del(const char* path){
  return request("DELETE", path, NULL, NULL);
}

// DELETE path and response
int RestClient::del(const char* path, char* response){
  return request("DELETE", path, NULL, response);
}

// DELETE path and body
int RestClient::del(const char* path, const char* body ){
  return request("DELETE", path, body, NULL);
}

// DELETE path and body with response
int RestClient::del(const char* path, const char* body, char* response){
  return request("DELETE", path, body, response);
}

void RestClient::write(const char* string){
  client.print(string);
}

void RestClient::setHeader(const char* header){
  headers[num_headers] = header;
  num_headers++;
}

void RestClient::setContentType(const char* contentTypeValue){
  contentType = contentTypeValue;
}

// The mother- generic request method.
int RestClient::request(const char* method, const char* path, const char* body, char* response){
  if(client.connect(host, port)){
    // Make a HTTP request line:
    write(method);
    write(" ");
    write(path);
    write(" HTTP/1.1\r\n");
    for(int i=0; i<num_headers; i++){
      write(headers[i]);
      write("\r\n");
    }
    write("Host: ");
    write(host);
    write("\r\n");
    write("Connection: close\r\n");
    if(body != NULL){
      char contentLength[30];
      sprintf(contentLength, "Content-Length: %d\r\n", strlen(body));
      write(contentLength);
  	  write("Content-Type: ");
  	  write(contentType);
  	  write("\r\n");
    }
    write("\r\n");
    if(body != NULL){
      write(body);
      write("\r\n");
      write("\r\n");
    }
    //make sure you write all those bytes.
    delay(100);
    int statusCode = readResponse(response);
    //cleanup
    num_headers = 0;
    client.stop();
    delay(50);
    return statusCode;
  }else{
    return 0;
  }
}

int RestClient::readResponse(char* response) {
  // an http request ends with a blank line
  boolean currentLineIsBlank = true;
  boolean httpBody = false;
  boolean inStatus = false;
  char statusCode[4];
  int i = 0;
  int code = 0;
  while (client.connected()){
    if (client.available()){
      char c = client.read();
      char* aux_str = new char[strlen(response)+2];
      if(c == ' ' && !inStatus){
        inStatus = true;
      }
      if(inStatus && i < 3 && c != ' '){
        statusCode[i] = c;
        i++;
      }
      if(i == 3){
        statusCode[i] = '\0';
        code = atoi(statusCode);
      }
      if(httpBody){
        //only write response if its not null
        if(response != NULL){
          strcpy(aux_str,response);
          aux_str[strlen(response)] = c;
          aux_str[strlen(response)] = '\0';
          strcpy(response,aux_str);
        }
      }
      else{
        if (c == '\n' && currentLineIsBlank) {
          httpBody = true;
        }

        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
  }
  return code;
}
