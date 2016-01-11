#include <SPI.h>
#include <Ethernet.h>
//https://github.com/csquared/arduino-restclient
#include "RestClient.h"
//https://github.com/interactive-matter/aJson
#include "aJSON.h"

/*Primera versión, utilizando aJSON y RestClient original.
Cadenas JSON de prueba pre-establecidas (obtenidas de puente Hue).*/

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
  aJsonObject* lightsarray = aJson.createArray();
  for(int i=0;i<n;i++){
    aJson.addItemToArray(lightsarray,getLightState(i+1));
    Serial.print("Antiguo estado de luz ");
    Serial.print(i+1);
    Serial.println(":");
    Serial.println(aJson.print(aJson.getArrayItem(lightsarray,i)));
    delay(2000);
  }
  int j=1;
  aJsonObject* lightState = getLightState(j);
  Serial.println(aJson.print(lightState));
  if(getLightOnOff(lightState))
    Serial.println("Luz encendida");
  else
    Serial.println("Luz apagada");
  delay(5000);
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