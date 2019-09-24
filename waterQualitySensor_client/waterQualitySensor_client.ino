#include <SPI.h>
#include <LoRa.h>

//variaveis globais
#define RF95_FREQ 868E6
#define DEBUG true
const int analogPinA0 = A0;
const float a = -6.0;    /* usados na equacao do pH */ 
const float b = 25.0;    /* usados na equacao do pH */ 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) ; // Wait for serial port to be available
  
  if(!LoRa.begin(RF95_FREQ)){
    Serial.println("Starting LoRa failed");
    while(1);  
  }
}

void loop() {
  // put your main code here, to run repeatedly:
#if defined(DEBUG)
  printHeader();
#endif

  float value = getPhValue();

#if defined(DEBUG)
  Serial.println("Sending msg to gateway...");
#endif

  // Send a message to rf95_server
  uint8_t valueToSend[10];
  dtostrf(value, 0, 10, valueToSend);
  //rf95.send(valueToSend, sizeof(valueToSend));
  LoRa.beginPacket();
  LoRa.print((char*)valueToSend);
  LoRa.endPacket();

#if defined(DEBUG)
  //Lora testing...
  Serial.print("Msg enviada = ");
  Serial.println((char*)valueToSend);
#endif
  
  

  delay(2000);
}

void printHeader(){
  Serial.println("--------------------------------------------------");
  Serial.println("---------------------RUNNING----------------------");
  Serial.println("--------------------------------------------------");  
}

float getPhValue(){
  float value = analogRead(analogPinA0);
  float voltagem;

  value = value * (5.0/1024.0);
  voltagem = value;
  //ph Equation
  value = a * value + b;

#if defined(DEBUG)
  Serial.print("**** [Sensor value = ");
  Serial.print(value);
  Serial.print(" || Voltagem = ");
  Serial.print(voltagem);
  Serial.println("] ****");
#endif

  return value;
}
