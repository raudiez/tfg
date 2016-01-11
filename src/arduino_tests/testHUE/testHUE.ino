#include <SPI.h>
#include <Ethernet.h>
//https://github.com/csquared/arduino-restclient
#include "RestClient.h"
//https://github.com/interactive-matter/aJson
#include "aJSON.h"

//Declaración de la direcciones MAC e IP. También del puerto 80
byte mac[]={ 0x90, 0xA2, 0xDA, 0x0F, 0xF9, 0x41 }; //MAC
byte ip[] = { 192, 168, 2, 146 };//IP
String hueuser = "d3dede62b969c73f21574a17b7a8fb";

RestClient hue = RestClient("192.168.2.121");

void setup(){
  Serial.begin(9600);
  Ethernet.begin(mac,ip);
}

void loop(){
  initHue();
  int n = getNumHueLights();
  Serial.println(n);
  for(int i=1;i<=n;i++){
    Serial.println(setHueColor(i,65000));
    delay(1000);
    Serial.println(setHueColor(i,40000));
    delay(1000);
    Serial.println(setHueOff(i));
    delay(1000);
    Serial.println(setHueOn(i));
  }
  delay(3000);
}

void initHue(){
  hue.setContentType("application/json");
}

String getHueList(){
  String response = String();
  String aux = "/api/"+hueuser+"/lights";
  const char* path = aux.c_str();
  int status = hue.get(path, &response);
  return response;
}

String setHueColor(int huelight, long color){
  String response = String();
  String aux = "/api/"+hueuser+"/lights/"+String(huelight)+"/state";
  const char* path = aux.c_str();
  String aux2 = "{\"hue\":"+String(color)+"}";
  const char* body = aux2.c_str();
  int status = hue.put(path, body, &response);
  return response;
}

String setHueOn(int huelight){
  String response = String();
  String aux = "/api/"+hueuser+"/lights/"+String(huelight)+"/state";
  const char* path = aux.c_str();
  String aux2 = "{\"on\":true}";
  const char* body = aux2.c_str();
  int status = hue.put(path, body, &response);
  return response;
}

String setHueOff(int huelight){
  String response = String();
  String aux = "/api/"+hueuser+"/lights/"+String(huelight)+"/state";
  const char* path = aux.c_str();
  String aux2 = "{\"on\":false}";
  const char* body = aux2.c_str();
  int status = hue.put(path, body, &response);
  return response;
}

int getNumHueLights(){
  String response = getHueList();
  char* json = new char [response.length()+1];
  strcpy(json,response.c_str());
  aJsonObject* jsonObject = aJson.parse(json);
  int num = (int)aJson.getArraySize(jsonObject);
  return num;
}