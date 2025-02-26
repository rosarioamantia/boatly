#include <LoRa.h>
#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h> // libreria per I2C
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <SPI.h>
#include <MQTTClient.h>


#define OLED_SDA 21 // data
#define OLED_SCL 22 // clock
#define OLED_RESET -1

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

//define the pins used by the transceiver module
#define cs 18 // -> Chip Select (SPI)
#define rst 23
#define dio0 26 // non si usa perchè bisogna settarlo solo per uso interno

const char* ssid = "Vodafone-A37173362";
const char* password = "LF9477sPhctAYJJn";

int counter = 0;
bool withDisplay = false;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //&Wire = riferimento ad I2C
WiFiClientSecure net;
MQTTClient client;

typedef struct  {
  uint8_t IDDEV[3]; //float temp;
} NotifyPacket;

typedef struct  {
  uint8_t IDDEV[3];

} SwitchModePacket;

void initDisplay(){
  Serial.println("INIT DISPLAY");

  //inizializzazione display OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) // SSD1306_SWITCHCAPVCC = modalità di alimentazione del display OLED
  {                                               // 0x3C = indirizzo I2C del display SSD1306, specifico per dispositivo (es. display OLED 0x3C)
    Serial.print(F("SSD130 allocation failed"));  
  }
  else
  {
    Serial.println("display ok");
    withDisplay = true;
    display.clearDisplay();
    display.display();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(0, 0);
    // display.drawBitmap(SCREEN_WIDTH, SCREEN_HEIGHT, logo16_glcd_bmp, 16, 16, 1);
    display.print("TEST OLED OK");
    display.display();
  }
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
}

void setup() {
  Serial.begin(115200);
  Serial.println("LoRa Receiver");
  LoRa.setPins(cs, rst, dio0);

  //replace the LoRa.begin(---E-) argument with your location's frequency
  //433E6 for Asia
  //866E6 for Europe
  //915E6 for North America
  while (!LoRa.begin(866E6)) {
    Serial.println(".");
    delay(500);
  }

  
  // Change sync word (0xF3) to match the receiver (optional)
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  //LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
  initDisplay();

  // mqtt/wifi init
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf("WiFi Failed!\n");
        return;
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  net.setInsecure();

  client.begin("broker.hivemq.com", 8883, net);
  client.onMessage(messageReceived);
  //while (!client.connect("broker.hivemq.com")) { //Scommenta quando riprendi ip
   // Serial.print(".");
   // delay(1000);
  //}
  Serial.println("MQTT Connected");
}

NotifyPacket presencePacket;
char buff[sizeof(NotifyPacket)];

enum Mode {
  TEST1,
  TEST2,
  TEST3
};
Mode mode = TEST2;

//receiver
void loop() {
  client.loop();

  if(mode == TEST1){

    // try to parse packet
    int packetSize = LoRa.parsePacket();
    
    if (packetSize) {
      // received a packet
      if (LoRa.available()) {
        LoRa.readBytes((uint8_t *)&presencePacket, sizeof(presencePacket));

        Serial.println("Messaggio ricevuto");
        Serial.printf("ID Sensore: %02x %02x %02x\n", presencePacket.IDDEV[0], presencePacket.IDDEV[1], presencePacket.IDDEV[2]);
        //Serial.println(presencePacket.temp);
        
        sprintf(buff, "%02x%02x%02x", presencePacket.IDDEV[0], presencePacket.IDDEV[1], presencePacket.IDDEV[2]);
        Serial.println(buff);
        client.publish("boatly/presence",buff, false, 2);
        
        //String LoRaData = LoRa.readString();
        //Serial.print(LoRaData);      
      }
      // print RSSI of packet
      //Serial.print("' with RSSI ");
      //Serial.println(LoRa.packetRssi());
    }

    counter++;

  }
  if(mode == TEST2){
    String message = "8b288a:2_3b466a-2b466c$";  // Stringa di esempio -> numero_rubate:id_barca:id
    Serial.print("Invio: ");
    Serial.println(message);

    LoRa.beginPacket();        // Inizia il pacchetto
    LoRa.print(message);       // Invia la stringa
    LoRa.endPacket();          // Chiude il pacchetto e invia

    delay(2000);  // Aspetta prima di inviare di nuovo
    mode = TEST3;


  }
  if(mode == TEST3){
    String message = "8b288:2_3b466a-2b466c$";  // Stringa di esempio -> numero_rubate:id_barca:id
    Serial.print("Invio: ");
    Serial.println(message);

    LoRa.beginPacket();        // Inizia il pacchetto
    LoRa.print(message);       // Invia la stringa
    LoRa.endPacket();          // Chiude il pacchetto e invia

    delay(2000);  // Aspetta prima di inviare di nuovo
    mode = TEST2;    
  }
}
