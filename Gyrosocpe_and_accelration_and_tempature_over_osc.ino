/*
   Send Gyroscope data via OSC to Touchdesigner.
   Created by Bart van Nes,  December 2022

   Written for ESP-8266 and MPU6050 gyrosocpe.
   vcc of gyroscope --> to vcc of ESP-8266
   gnd of gyroscope --> to gnd of ESP-8266
   slc of gyroscope --> to D1 of ESP-8266
   sda of gyroscope --> to D2 of ESP-8266

   To calibrate gyroscope, hold gyroscope still and press reset button on ESP-8266.

*/

#include "Wire.h"

//gyrosocope setup
#include <MPU6050_light.h>
MPU6050 mpu(Wire);

int X;
int Y;
int Z;

int AX;
int AY;
int AZ;

int temp;

//UDP communicaton setup
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>  // for sending OSC messages
#include <OSCBundle.h>   // for receiving OSC messages

// Wifi name and password
char ssid[] = "DAER-NET-FOCUS";  // your network SSID (name)
char pass[] = "daer22*FOCUS";      // your network password




WiFiUDP Udp;                              //  a UDP instance to let us send and receive packets over UDP
const IPAddress destIp(10, 10, 1, 171);  // IP of the PC
const unsigned int destPort = 51503;      // remote port to receive OSC





void setup() {
  Serial.begin(115200);  // start serial monitor

  // gyrosocope
  Wire.begin();
  byte status = mpu.begin();
  // mpu.upsideDownMounting = true; // uncomment this line if the MPU6050 is mounted upside-down



  // Connect to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {  // while the wifi is not connected print dots
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void loop() {
  mpu.update();         //update the gyroscope data
  X = mpu.getAngleX();  // get gyroscope angle for X axis
  Y = mpu.getAngleY();  // get gyroscope angle for Y axis
  Z = mpu.getAngleZ();  // get gyroscope angle for Z axis

  AX = mpu.getAccX();
  AY = mpu.getAccY();
  AZ = mpu.getAccZ();

  temp= mpu.getTemp();

  sendOSC();  // run the "sendOSC" void
}


void sendOSC() {

  //send OSC
  OSCBundle msgOut;
  msgOut.add("/X").add(X);  //send "/X" + the gyrosocope data for the X axis via osc
  msgOut.add("/Y").add(Y);  //send "/Y" + the gyrosocope data for the Y axis via osc
  msgOut.add("/Z").add(Z);  //send "/Z" + the gyrosocope data for the Z axis via osc

  msgOut.add("/AX").add(AX);  //send "/X" + the gyrosocope data for the X acceleration via osc
  msgOut.add("/AY").add(AY);  //send "/Y" + the gyrosocope data for the Y acceleration via osc
  msgOut.add("/AZ").add(AZ);  //send "/Z" + the gyrosocope data for the Z acceleration via osc

  msgOut.add("/temp").add(temp);  //send "/Z" + the gyrosocope data for the temperature via osc


  Udp.beginPacket(destIp, destPort);
  msgOut.send(Udp);  // send message
  Udp.endPacket();
  msgOut.empty();  // Clear message
  delay(100);
}