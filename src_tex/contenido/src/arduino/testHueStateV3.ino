#include <Ethernet.h>
#include <SPI.h>
#include "RestClient.h"
//https://github.com/bblanchon/ArduinoJson
#include "ArduinoJson.h"
//http://playground.arduino.cc/Code/AvailableMemory
#include "MemoryFree.h"

/*Última versión, utilizando ArduinoJson y RestClient modificado.
Se usa MemoryFree para comprobar memoria disponible en Arduino.*/

//Declaración de la direcciones MAC e IP. También del puerto 80
byte mac[]={ 0x90, 0xA2, 0xDA, 0x0F, 0xF9, 0x41 }; //MAC
byte ip[] = { 192, 168, 2, 146 };//IP
const char* hueuser = "d3dede62b969c73f21574a17b7a8fb";

RestClient hue = RestClient("192.168.2.121");

StaticJsonBuffer<200> jsonBuffer;

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
  Serial.print("freeMemory()=");
  Serial.println(freeMemory());
  int n = getNumHueLights();
  Serial.println(n);
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

int getNumHueLights(){
  char* json = getHueList();
  Serial.print(json);
  JsonObject& root = jsonBuffer.parseObject(json);
  int num = 0;
  for(JsonObject::iterator it=root.begin(); it!=root.end(); ++it) num++;

  return num;
}