#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <LoRa.h>

#define cs 18 // -> Chip Select (SPI)
#define rst 23
#define dio0 26

const char* ssid = "Pixel_9854";
const char* password = "ciaociao1";

const char* mqtt_server = "broker.emqx.io";
uint16_t mqtt_port = 8883;
WiFiClientSecure client;
PubSubClient mqttClient(client);
String outbound_mqtt_message = "";
boolean outbound_mqtt = false;

boolean runEvery(unsigned long interval){
    static unsigned long previousMillis = 0;
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval)
    {
        previousMillis = currentMillis;
        return true;
    }
    return false;
}


void mqttCallback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    String received = "";
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
        received+=(char) payload[i]; 
    }
    Serial.println();
    LoRa.beginPacket();        // Inizia il pacchetto
    LoRa.print(received);       // Invia la stringa
    LoRa.endPacket();
    Serial.println("-----------------------");
    LoRa.receive();

}

void onReceive(int packetSize){
  if (packetSize == 0) return;
  

  while(LoRa.available()){
    char actual_char = (char) LoRa.read();

    //Serial.print("\nmessaggio lora ricevuto, da scrivere nel topic mqtt\n");

    outbound_mqtt_message += actual_char;
    
  }
  
  Serial.print("ricevuto da barca: ");
  Serial.println(outbound_mqtt_message);
  outbound_mqtt = true;
  LoRa.receive();
}

void setup(){
  Serial.begin(115200);
  LoRa.setPins(LORA_CS, LORA_RST, dio0);
  while (!LoRa.begin(866E6)) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("setup()");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED){
    Serial.println("Internet Connecting");
      delay(500);
  }

  client.setInsecure();
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(mqttCallback);
  Serial.println("Internet Connected");

  while (!mqttClient.connected()) {
      String client_id = "iotBoard-client-";
      client_id += String(WiFi.macAddress());
      
      if (mqttClient.connect(client_id.c_str())) {
        Serial.println("MQTT Connected");
      } else {
        Serial.println("MQTT error");
          delay(2000);
      }
  }

  mqttClient.subscribe("test/rosario");
  LoRa.onReceive(onReceive);
  LoRa.receive();
}

void loop(){

  if(outbound_mqtt == true){
    Serial.print("\nOKOK\n");
    mqttClient.publish("test/receiver", outbound_mqtt_message.c_str());
    outbound_mqtt_message.clear();
    outbound_mqtt = false;
  }

  if(runEvery(2000)){
      
  }
  mqttClient.loop();
}