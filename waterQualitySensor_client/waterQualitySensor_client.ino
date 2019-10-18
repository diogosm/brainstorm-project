#include <SPI.h>
#include <LoRa.h>
#include <DS3232RTC.h>      // https://github.com/JChristensen/DS3232RTC
#include <Streaming.h>      // http://arduiniana.org/libraries/streaming/
#include <RH_RF95.h>

//variaveis globais
RH_RF95 rf95;
#define DEBUG true
const int analogPinA0 = A0;
/*
 * Variaveis a serem utilizadas na equacao do primeiro grau
 * que modela a transformacao da corrente para pH
 */
const float a = -6.0;    /* usados na equacao do pH */ 
const float b = 25.0;    /* usados na equacao do pH */ 

void setup() {
  printSettingHeader();
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) ; // Wait for serial port to be available
  
  if (!rf95.init())
    Serial.println("init failed");

  rf95.setTxPower(20, false);
  rf95.setFrequency(915.0);
  rf95.setSignalBandwidth(500000);
  rf95.setSpreadingFactor(8);
  rf95.setCodingRate4(6);

  /** Seta config inicial para o RTC **/
  setSyncProvider(RTC.get);
  if (timeStatus() != timeSet)
    Serial.println("Starting RTC get time failed");

#if defined(DEBUG)
  Serial.println("Trying to get time set");
  printTempo();
  Serial.println();
#endif
  delay(1000);
  
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
  rf95.send(valueToSend, sizeof(valueToSend));
  rf95.waitPacketSent();

#if defined(DEBUG)
  //Lora testing...
  Serial.print("Msg enviada = ");
  Serial.println((char*)valueToSend);
#endif

  delay(5000);
}

void printSettingHeader(){
  Serial.println("--------------------------------------------------");
  Serial.println("---------------------SETTING----------------------");
  Serial.println("--------------------------------------------------");  
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

void printTempo(){
  time_t t;

  t = now();
  printDateTime(t);
}

/*
 * @TODO
 *  fazer a funcao de pegar o tempo e enviar pelo LoRa
 */
String getTempo(){
  String ans = "";
}

// print date and time to Serial
void printDateTime(time_t t)
{
    printDate(t);
    Serial << ' ';
    printTime(t);
}

// print time to Serial
void printTime(time_t t)
{
    printI00(hour(t), ':');
    printI00(minute(t), ':');
    printI00(second(t), ' ');
}

// print date to Serial
void printDate(time_t t)
{
    printI00(day(t), 0);
    Serial << monthShortStr(month(t)) << _DEC(year(t));
}

// Print an integer in "00" format (with leading zero),
// followed by a delimiter character to Serial.
// Input value assumed to be between 0 and 99.
void printI00(int val, char delim)
{
    if (val < 10) Serial << '0';
    Serial << _DEC(val);
    if (delim > 0) Serial << delim;
    return;
}
