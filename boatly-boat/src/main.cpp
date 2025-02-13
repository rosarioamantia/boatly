#include <LoRa.h>
#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h> // libreria per I2C

#define OLED_SDA 21 // data
#define OLED_SCL 22 // clock
#define OLED_RESET -1

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

//define the pins used by the transceiver module
#define cs 18 // -> Chip Select (SPI)
#define rst 23
#define dio0 26 // non si usa perchè bisogna settarlo solo per uso interno

int counter = 0;
bool withDisplay = false;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //&Wire = riferimento ad I2C

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
}

//sender
void loop() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Sto inviando: " + String(counter));
  display.display();

  Serial.print("Sending packet: ");
  Serial.println(counter);
  
  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print("hello ");
  LoRa.print(counter);
  LoRa.endPacket();
  counter++;
  delay(10000);
  }
