#include <SPI.h>
#include <Ethernet.h>

//Declaración de la direcciones MAC e IP. También del puerto 80
byte mac[]={ 0x90, 0xA2, 0xDA, 0x0F, 0xF9, 0x41 }; //MAC
byte ip[] = { 192, 168, 2, 146 };//IP

EthernetServer servidor(80);
int PIN_LED=9;
int PIN_BUTTON=8;
String readString=String(30);
int led_status=0;
String cad = "";
int val; //Variable para leer el estado del pin
int val2; //Variable para la segunda lectura
int buttonState; //Variable para el estado del pulsador

void setup(){
  Serial.begin(9600);
  Ethernet.begin(mac,ip);
  servidor.begin();
  pinMode(PIN_LED,OUTPUT);
  digitalWrite(PIN_LED,LOW);
  pinMode(PIN_BUTTON, INPUT);
  digitalWrite(PIN_BUTTON,HIGH); //Activa RPull-up.
  buttonState = digitalRead(PIN_BUTTON); //lee el estado inicial
}

void loop(){
  read_button();
  EthernetClient client= servidor.available();
  if(client){
    boolean lineaenblanco=true;
    while(client.connected()){//cliente conectado
      if(client.available()){
        char c=client.read();
        if(readString.length()<30){//Leemos petición HTTP caracter a caracter
          readString.concat(c); //Almacenar los caracteres en la variable readString
        }
        if(c=='\n' && lineaenblanco){//Si la petición HTTP ha finalizado
          print_header(client);
          client.println(cad);
          client.stop();//Cierro conexión con el cliente
          readString="";
        }
      }
    }
  }
}

void print_header(EthernetClient client){
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println();
}

void read_button(){
  val = digitalRead(PIN_BUTTON); //lee el valor del pulsador y lo almacena
  delay(5);
  val2 = digitalRead(PIN_BUTTON); //lee el valor del pulsador y lo almacena
  if(val==val2){
    if (val!=buttonState){ //el estado del pulsador ha cambiado
      if(val==LOW){ //comprueba si el pulsador esta pulsado
        if(led_status==0){ //esta la luz apagada?
          led_status=1; //enciende la luz
          digitalWrite(PIN_LED,HIGH);
          cad = "led encendido";
        }else{
          led_status=0; //apaga la luz
          digitalWrite(PIN_LED,LOW);
          cad = "led apagado";
        }
      }
    }
    buttonState=val; //guarda el nuevo estado del pulsador
  }
}