#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "token buraya";

char ssid[] = "wifi-adı";
char pass[] = "wifi-sifresi";
BlynkTimer timer;

const int sensor_pin = A0;
float moisture_percentage;

int LowLimit, HighLimit;
unsigned long time_now = 0;
unsigned long period = 10000; //10sn
bool moistureLow = false;

BLYNK_WRITE(V1)
{
  int pinValue = param.asInt();         // assigning incoming value from pin V1 to a variable
  if (pinValue == 1) {                        // If value is 1 run this command
    digitalWrite(D2, HIGH);              //D4 output from Wemos D1 mini
  }
  else {                                           // If value is 0 run this command
    digitalWrite(D2, LOW);
  }
  Serial.print("V1 Button value is: ");
  Serial.println(pinValue);
}

void setup()
{
  delay(10);
  Serial.begin(9600);  
  pinMode(D2, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.hostname("PLANT");
  Blynk.begin(auth, ssid, pass);
  while (Blynk.connect() == false) {}

  /////////////////////////BLINK LED////////////////////////////
  timer.setInterval(3300L, []() {  // Start 1st Timed Lambda Function - Turn ON LED every 2 seconds (repeating)
    timer.setTimeout(3000L, []() {  // Start 2nd Timed Lambda Function - Turn OFF LED 1 second later (once per loop)
      digitalWrite(LED_BUILTIN, LOW); // Turn ESP On-Board LED ON
    });  // END 2nd Timer Function
    digitalWrite(LED_BUILTIN, HIGH); // Turn ESP On-Board LED OFF
  });
  /////////////////////////BLINK LED////////////////////////////

  //ArduinoOTA.onError([](ota_error_t error) { ESP.restart(); });
  ArduinoOTA.setHostname("PLANT");
  ArduinoOTA.begin();
  timer.setInterval(60000L, sensor);      //(10dk)

}

void loop()
{
  Blynk.run();
  ArduinoOTA.handle();
  timer.run();
  //doit();
}

BLYNK_WRITE(V3) {
  LowLimit = param.asInt();
  Serial.print("LowLimit:");
  Serial.println(LowLimit);
}

BLYNK_WRITE(V4) {
  HighLimit = param.asInt();
  Serial.print("HighLimit:");
  Serial.println(HighLimit);
}

void sensor() {


  moisture_percentage = ( 100.00 - ( (analogRead(sensor_pin) / 1023.00) * 100.00 ) );
  Blynk.virtualWrite(V2, moisture_percentage );
  Serial.print("Soil Moisture(in Percentage) = ");
  Serial.print(moisture_percentage);
  Serial.println("%");

  if (moisture_percentage  < LowLimit) {
    Serial.println("moisture is not enough");


    if (moistureLow == false) {
      Blynk.notify("CEM, BANA SU VER ! ! !");
      moistureLow = true;
    }

    waterON();
    timer.setTimeout(10000L, waterOFF);
  }

  if (moisture_percentage  > HighLimit) {
    moistureLow = false;
    waterOFF();
  }
}


void waterON()
{
  digitalWrite(D2, HIGH);  // Set pin Low
  //timer.setTimeout(10000, waterOFF);
}

void waterOFF()
{
  digitalWrite(D2, LOW);  // Set pin Low
}
