#include <SPI.h>
#include <RH_RF95.h>

//variaveis globais
#define RF95_FREQ 915
const int analogPinA0 = A0;
RH_RF95 rf95;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) ; // Wait for serial port to be available
  if (!rf95.init())
    Serial.println("init failed");

  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }

  rf95.setTxPower(15, true);
}

void loop() {
  Serial.println("--------------------------------");
  Serial.println("------------RUNNING-------------");
  Serial.println("--------------------------------");
  // put your main code here, to run repeatedly:
  float value = analogRead(analogPinA0);

  value = value * (5.0/1024.0);
  //ph Equation
  value = -6.0 * value + 25.0;
  Serial.print("Sensor value = ");
  Serial.println(value);

  //Lora testing...
  Serial.println("Sending to rf95_server");
  // Send a message to rf95_server
  uint8_t valueToSend[10];
  dtostrf(value, 0, 10, valueToSend);
  Serial.println((char*)valueToSend);
  rf95.send(valueToSend, sizeof(valueToSend));

  bool result = rf95.waitPacketSent();
  Serial.print("WaiPacketSent response = ");
  Serial.println(result);
  
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (rf95.waitAvailableTimeout(3000))
  { 
    // Should be a reply message for us now   
    if (rf95.recv(buf, &len))
   {
      Serial.print("got reply: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);    
    }
    else
    {
      Serial.println("recv failed");
    }
  }
  else
  {
    Serial.println("No reply, is rf95_server running?");
  }

  delay(1000);
}
