#include <Arduino.h>
//#include <NTPClient.h>    // activate for NTP
//#include <WiFiUdp.h>      // activate for NTP
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>

#define DEVmessages 1 // change 1 to 0 if you don´t want MQTT DEV messages

// Wifi and mqtt network settings
const char *ssid = "the dude-net";
const char *password = "iR3DNw8ZFk-t9e3ixVJjhAE-2d9374H9sw5-Sv99fC645C2-6G4359L463tY";
const char *HostName = "ESP8266-BBQ"; // Edit the hostname which will be shown in your LAN
const char *mqtt_server = "10.0.0.10";
const char *mqtt_user = "mark";
const char *mqtt_pass = "8749";
const char *SensorName = "BBQcontrol";
//const char *NTPserver = "10.0.0.1";     // activate for NTP
const char *version = "BBQcontrol v1.00";

// Other variables
const int servoPin = 2;
//long unsigned cycleTime = 2000;
//long unsigned lastTime;
int incomingByte = 0;    // for incoming serial data
int pos = 90;            // init position
int stepServo;           // actual step lengh the servo does
int hysteresis;          // for reversing general direction
int stepRelease;         // push a little bit further then back again to release preasure from servo in idle
int boostTime;           // time [s] for how long the servo should go to max position for short extra heating
int boostPosition = 1; // postion of servo for maximum
int lastDir = 2;
long unsigned startTime;

// NTP stuff - activate for NTP
//WiFiUDP ntpUDP;
//NTPClient timeClient(ntpUDP, NTPserver, 3600, 600000);

Servo myservo;
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi()
{
  delay(100);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.hostname(HostName);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");

    if (client.connect(SensorName, mqtt_user, mqtt_pass))
    {
      client.subscribe("BBQcontrol/p_step");
      client.subscribe("BBQcontrol/p_hysteresis");
      client.subscribe("BBQcontrol/p_stepRelease");
      client.subscribe("BBQcontrol/p_boostTime");
      client.subscribe("BBQcontrol/control");
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      delay(6000);
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (unsigned int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  String stringTopic = String(topic); // format topic for evaliation of received parameters below

  // Receive stepsize
  if (stringTopic.equals("BBQcontrol/p_step"))
  {
    char helperString[6];
    uint8_t pIdx;
    for (pIdx = 0; pIdx < 5 && pIdx < length; pIdx++)
    {
      helperString[pIdx] = payload[pIdx];
    }
    helperString[pIdx] = '\0';
    if (String(helperString).toInt() > 0)
    {
      stepServo = String(helperString).toInt();
      Serial.print("p_step: ");
      Serial.println(stepServo);
    }
  }

  // Receive hysteresis
  if (stringTopic.equals("BBQcontrol/p_hysteresis"))
  {
    char helperString[6];
    uint8_t pIdx;
    for (pIdx = 0; pIdx < 5 && pIdx < length; pIdx++)
    {
      helperString[pIdx] = payload[pIdx];
    }
    helperString[pIdx] = '\0';
    if (String(helperString).toInt() > 0)
    {
      hysteresis = String(helperString).toInt();
      Serial.print("p_hysteresis: ");
      Serial.println(hysteresis);
    }
  }

  // Receive stepRelease
  if (stringTopic.equals("BBQcontrol/p_stepRelease"))
  {
    char helperString[6];
    uint8_t pIdx;
    for (pIdx = 0; pIdx < 5 && pIdx < length; pIdx++)
    {
      helperString[pIdx] = payload[pIdx];
    }
    helperString[pIdx] = '\0';
    if (String(helperString).toInt() > 0)
    {
      stepRelease = String(helperString).toInt();
      Serial.print("p_stepRelease: ");
      Serial.println(stepRelease);
    }
  }

  // Receive boostTime
  if (stringTopic.equals("BBQcontrol/p_boostTime"))
  {
    char helperString[6];
    uint8_t pIdx;
    for (pIdx = 0; pIdx < 5 && pIdx < length; pIdx++)
    {
      helperString[pIdx] = payload[pIdx];
    }
    helperString[pIdx] = '\0';
    if (String(helperString).toInt() > 0)
    {
      boostTime = String(helperString).toInt();
      Serial.print("p_boostTime: ");
      Serial.println(boostTime);
    }
  }

  // Evaluate first character received for new step and direction
  if ((char)payload[0] == '-')
  {
    if (lastDir == 0)
    {
      pos = pos + hysteresis;
    }
    if (pos + stepServo < 181) // set endstops
    {
      pos = pos + stepServo;
      myservo.write(pos + stepRelease);
      delay(500);
      myservo.write(pos);
      delay(100);
      lastDir = 1;
      Serial.print("Position: ");
      Serial.print(pos);
      Serial.print(" - Step: ");
      Serial.println(stepServo);
    }
  }

  if ((char)payload[0] == '+')
  {
    if (lastDir == 1)
    {
      pos = pos - hysteresis;
    }
    if (pos - stepServo > 0) // set endstops
    {
      pos = pos - stepServo;
      myservo.write(pos - stepRelease);
      delay(500);
      myservo.write(pos);
      delay(100);
      lastDir = 0;
      Serial.print("Position: ");
      Serial.print(pos);
      Serial.print(" - Step: ");
      Serial.println(stepServo);
    }
  }

  // Boosting
  if ((char)payload[0] == 'B')
  {
    myservo.write(boostPosition);
    delay(500);
    client.publish("BBQcontrol/DEVposition", String("Boosting").c_str(), true);
    myservo.write(boostPosition + stepRelease);
    delay(boostTime * 1000);
    myservo.write(pos + stepRelease); // "-" if boost postion is the largest possible value, e.g. 180°
    delay(500);
    myservo.write(pos);
    delay(100);
    lastDir = 0;
    //Serial.print("Position: ");
    //Serial.print(pos);
    //Serial.print(" - Step: ");
    //Serial.println(stepServo);
  }

  client.publish("BBQcontrol/DEVposition", String(pos).c_str(), true);
  client.publish("BBQcontrol/DEV_Wifi_RSSI", String(WiFi.RSSI()).c_str(), true);
}

void booster()
{

startTime = millis();

}

void setup()
{
  Serial.begin(9600);
  Serial.println(" ");
  Serial.println("Starting up...");
  setup_wifi();
  client.setServer(mqtt_server, 1885);
  client.setCallback(callback);
  //timeClient.begin();   // activate for NTP
  Serial.println(version);
  myservo.attach(servoPin);
  myservo.write(pos);
  delay(100);
  client.publish("BBQcontrol/DEVposition", String(pos).c_str(), true);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }

  client.loop();
  //timeClient.update();   // activate for NTP

  /* if (millis() - lastTime >= cycleTime)
  {

  } */
}