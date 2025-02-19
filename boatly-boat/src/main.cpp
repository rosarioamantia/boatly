#include <LoRa.h>
#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h> // libreria per I2C

#include "esp_bt_main.h"
#include "esp_bt_device.h"

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  5        /* Time ESP32 will go to sleep (in seconds) */

#define OLED_SDA 21 // data
#define OLED_SCL 22 // clock
#define OLED_RESET -1

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

//define the pins used by the transceiver module
#define cs 18 // -> Chip Select (SPI)
#define rst 23
#define dio0 26 // non si usa perchè bisogna settarlo solo per uso interno

typedef struct  {
  uint8_t IDDEV[3]; //float temp;
} NotifyPacket;

enum Mode {
  SEA,
  HARBOR
};

int counter = 0;
bool withDisplay = false;
//uint8_t test = 125;
NotifyPacket presencePacket;
enum Mode mode = HARBOR;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //&Wire = riferimento ad I2C

bool initBluetooth(){
  if (!btStart()) {
    Serial.println("Failed to initialize controller");
    return false;
  }
 
  if (esp_bluedroid_init() != ESP_OK) {
    Serial.println("Failed to initialize bluedroid");
    return false;
  }
 
  if (esp_bluedroid_enable() != ESP_OK) {
    Serial.println("Failed to enable bluedroid");
    return false;
  }
 
}

void initDisplay(){
  Serial.println("INIT DISPLAY");

  //inizializzazione display OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) // SSD1306_SWITCHCAPVCC = modalità di alimentazione del display OLED
  {                                               // 0x3C = indirizzo I2C del display SSD1306, specifico per dispositivo (es. display OLED 0x3C)
    Serial.print(F("SSD130 allocation failed"));  
  }
  else
  {
    Serial.print("display ok");
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

void setup() {
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.begin(115200);
  Serial.println("LoRa Sender");
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
  initBluetooth();

  //ottenere l'indirizzo MAC univoco del modulo Bluetooth integrato nel dispositivo
  const uint8_t* device_mac_address = esp_bt_dev_get_address();
  for (int i = 3; i < 6; i++) {
    presencePacket.IDDEV[i - 3] = (char)device_mac_address[i];
  }
  esp_bluedroid_disable();
  esp_bluedroid_deinit();
}

char testok[sizeof(NotifyPacket)];
//sender
void loop() {

  if(mode == HARBOR){
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("PORTO invio: " + String(counter));
    display.display();

    Serial.print("Sending packet: ");
    Serial.println(counter);
    
    //Send LoRa packet to receiver
    LoRa.beginPacket();
    //LoRa.print("hello");
    //LoRa.print(counter);
    uint8_t * buff = (uint8_t *)(&presencePacket);
    for(int i = 0; i < sizeof(NotifyPacket); i++){
      LoRa.write(buff[i]);
      Serial.println("INIZIO");
      Serial.println(buff[i]);
      Serial.println("FINE");
    }
    Serial.printf("ID Sensore: %02x%02x%02x\n", buff[0], buff[1], buff[2]);
    LoRa.endPacket();
    counter++;    
  }
  else if(mode == SEA){
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("SEA invio: " + String(counter));
    display.display();

    Serial.print("Sending packet: ");
    Serial.println(counter);
    
    //Send LoRa packet to receiver
    LoRa.beginPacket();
    LoRa.print("hello");
    //LoRa.write(test);
    LoRa.print(counter);
    LoRa.endPacket();
    counter++;
  }

  delay(5000);
}