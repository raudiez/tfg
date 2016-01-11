#include <Ethernet.h>
#include <SPI.h>
#include "RestClient.h"
//https://github.com/interactive-matter/aJson
#include "aJSON.h"
//http://playground.arduino.cc/Code/AvailableMemory
#include "MemoryFree.h"

/*Otra versión, utilizando aJSON y RestClient modificado.
Se usa MemoryFree para comprobar memoria disponible en Arduino.*/

//Declaración de la direcciones MAC e IP. También del puerto 80
byte mac[]={ 0x90, 0xA2, 0xDA, 0x0F, 0xF9, 0x41 }; //MAC
byte ip[] = { 192, 168, 2, 146 };//IP
const char* hueuser = "d3dede62b969c73f21574a17b7a8fb";

RestClient hue = RestClient("192.168.2.121");

void setup(){
  Serial.print("Inicio - freeMemory()=");
  Serial.println(freeMemory());
  Serial.begin(9600);
  Ethernet.begin(mac,ip);
  Serial.print("Tras conectar por ethernet - freeMemory()=");
  Serial.println(freeMemory());
}

void loop(){
  initHue();
  int n = getNumHueLights();
  aJsonObject* lightsarray = aJson.createArray();
  for(int i=0;i<n;i++){
    aJson.addItemToArray(lightsarray,getLightState(i+1));
    Serial.print("freeMemory()=");
    Serial.println(freeMemory());
    delay(4000);
  }
  int j=1;
  aJsonObject* lightState = getLightState(j);
  delay(5000);
}

void initHue(){
  hue.setContentType("application/json");
}

char* getHueList(){
  char* response;
  char* aux = new char[13+strlen(hueuser)];
  strcpy(aux,"/api/");
  strcat(aux,hueuser);
  strcat(aux,"/lights");
  const char* path = aux;
  int status = hue.get(path, response);
  return response;
}

char* setHueColor(int huelight, long color){
  char* response;
  char* aux = new char[22+strlen(hueuser)];
  strcpy(aux,"/api/");
  strcat(aux,hueuser);
  strcat(aux,"/lights/");
  strcat(aux,(char*)huelight);
  strcat(aux,"/state");
  const char* path = aux;
  char* aux2 = new char[14];
  strcpy(aux2,"{\"hue\":");
  strcat(aux2,(char*)color);
  strcat(aux2,"}");
  const char* body = aux2;
  int status = hue.put(path, body, response);
  return response;
}

char* setHueOn(int huelight){
  char* response;
  char* aux = new char[22+strlen(hueuser)];
  strcpy(aux,"/api/");
  strcat(aux,hueuser);
  strcat(aux,"/lights/");
  strcat(aux,(char*)huelight);
  strcat(aux,"/state");
  const char* path = aux;
  const char* body = "{\"on\":true}";
  int status = hue.put(path, body, response);
  return response;
}

char* setHueOff(int huelight){
  char* response;
  char* aux = new char[22+strlen(hueuser)];
  strcpy(aux,"/api/");
  strcat(aux,hueuser);
  strcat(aux,"/lights/");
  strcat(aux,(char*)huelight);
  strcat(aux,"/state");
  const char* path = aux;
  const char* body = "{\"on\":false}";
  int status = hue.put(path, body, response);
  return response;
}

int getNumHueLights(){
  char* json = "{\"1\":{\"state\": {\"on\":false,\"bri\":0,\"hue\":0,\"sat\":0,\"effect\":\"none\",\"xy\":[0.0000,0.0000],\"ct\":0,\"alert\":\"none\",\"colormode\":\"hs\",\"reachable\":false}, \"type\": \"Extended color light\", \"name\": \"Hue Lamp 1\", \"modelid\": \"LCT001\", \"manufacturername\": \"Philips\",\"uniqueid\":\"00:17:88:01:00:fc:0e:23-0b\", \"swversion\": \"66009663\", \"pointsymbol\": { \"1\":\"none\", \"2\":\"none\", \"3\":\"none\", \"4\":\"none\", \"5\":\"none\", \"6\":\"none\", \"7\":\"none\", \"8\":\"none\" }},\"2\":{\"state\": {\"on\":true,\"bri\":0,\"hue\":0,\"sat\":0,\"effect\":\"none\",\"xy\":[0.0000,0.0000],\"ct\":0,\"alert\":\"none\",\"colormode\":\"hs\",\"reachable\":false}, \"type\": \"Extended color light\", \"name\": \"Hue Lamp 1\", \"modelid\": \"LCT001\", \"manufacturername\": \"Philips\",\"uniqueid\":\"00:17:88:01:00:fc:0e:23-0b\", \"swversion\": \"66009663\", \"pointsymbol\": { \"1\":\"none\", \"2\":\"none\", \"3\":\"none\", \"4\":\"none\", \"5\":\"none\", \"6\":\"none\", \"7\":\"none\", \"8\":\"none\" }}}";
  aJsonObject* jsonObject = aJson.parse(json);
  int num = (int)aJson.getArraySize(jsonObject);
  aJson.deleteItem(jsonObject);
  return num;
}

aJsonObject* getLightState(int light){
  char* json = "{\"1\":{\"state\": {\"on\":true,\"bri\":0,\"hue\":0,\"sat\":0,\"effect\":\"none\",\"xy\":[0.0000,0.0000],\"ct\":0,\"alert\":\"none\",\"colormode\":\"hs\",\"reachable\":false}, \"type\": \"Extended color light\", \"name\": \"Hue Lamp 1\", \"modelid\": \"LCT001\", \"manufacturername\": \"Philips\",\"uniqueid\":\"00:17:88:01:00:fc:0e:23-0b\", \"swversion\": \"66009663\", \"pointsymbol\": { \"1\":\"none\", \"2\":\"none\", \"3\":\"none\", \"4\":\"none\", \"5\":\"none\", \"6\":\"none\", \"7\":\"none\", \"8\":\"none\" }},\"2\":{\"state\": {\"on\":false,\"bri\":0,\"hue\":40000,\"sat\":0,\"effect\":\"none\",\"xy\":[0.0000,0.0000],\"ct\":0,\"alert\":\"none\",\"colormode\":\"hs\",\"reachable\":false}, \"type\": \"Extended color light\", \"name\": \"Hue Lamp 1\", \"modelid\": \"LCT001\", \"manufacturername\": \"Philips\",\"uniqueid\":\"00:17:88:01:00:fc:0e:23-0b\", \"swversion\": \"66009663\", \"pointsymbol\": { \"1\":\"none\", \"2\":\"none\", \"3\":\"none\", \"4\":\"none\", \"5\":\"none\", \"6\":\"none\", \"7\":\"none\", \"8\":\"none\" }}}";
  aJsonObject* jsonObject = aJson.parse(json);
  aJsonObject* jsonLight = aJson.getObjectItem(jsonObject,String(light).c_str());
  aJsonObject* lightState = aJson.getObjectItem(jsonLight,"state");
  return lightState;
}

boolean getLightOnOff(aJsonObject* lightstate){
  aJsonObject* lightonoff = aJson.getObjectItem(lightstate,"on");
  if(strcmp("true",aJson.print(lightonoff)) == 0){
    aJson.deleteItem(lightonoff);
    return true;
  }else{
    aJson.deleteItem(lightonoff);
    return false;
  }
}

boolean getLightReachable(aJsonObject* lightstate){
  aJsonObject* lightreachable = aJson.getObjectItem(lightstate,"reachable");
  if(strcmp("true",aJson.print(lightreachable)) == 0){
    aJson.deleteItem(lightreachable);
    return true;
  }else{
    aJson.deleteItem(lightreachable);
    return false;
  }
}