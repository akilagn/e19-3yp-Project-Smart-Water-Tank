#include <Arduino_BuiltIn.h>

#include "certs.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"

#define INLETVALVE 21
#define OUTLETVALVE 19
#define PUMP 2

#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

void messageHandler(char* topic, byte* payload, unsigned int length) {
  Serial.print("incoming: ");
  Serial.println(topic);

  char inputValvetrue[] = "inputValvetrue";
  char inputValvefalse[] = "inputValvefalse";
  char outputValvetrue[] = "outputValvetrue";
  char outputValvefalse[] = "outputValvefalse";
  char motorPumptrue[] = "motorPumptrue";
  char motorPumpfalse[] = "motorPumpfalse";

  char message[length+1];
 
  for(int i=0; i<length; i++){
    message[i]=(char) payload[i];
  }
  message[length] = '\0';
  Serial.println(message);

  if(strcmp(message,inputValvetrue)==0){
    digitalWrite(INLETVALVE,HIGH);
    digitalWrite(14,HIGH);
  }
  if(strcmp(message,inputValvefalse)==0){
    digitalWrite(INLETVALVE,LOW);
    digitalWrite(14,LOW);
  }
  if(strcmp(message,outputValvetrue)==0){
    digitalWrite(OUTLETVALVE,HIGH);
    digitalWrite(12,HIGH);
  }
  if(strcmp(message,outputValvefalse)==0){
    digitalWrite(OUTLETVALVE,LOW);
    digitalWrite(12,LOW);
  }
  if(strcmp(message,motorPumptrue)==0){
    digitalWrite(PUMP,HIGH);
  }
  if(strcmp(message,motorPumpfalse)==0){
    digitalWrite(PUMP,LOW);
  }
}


void connectAWS() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 
  Serial.println("Connecting to Wi-Fi");
 
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
 
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);
 
  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);
 
  // Create a message handler
  client.setCallback(messageHandler);
 
  Serial.println("Connecting to AWS IOT");
 
  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }
 
  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }
 
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
 
  Serial.println("AWS IoT Connected!");
}

