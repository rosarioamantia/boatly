#include <LoRa.h>
#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h> // libreria per I2C

#include "esp_bt_main.h"
#include "esp_bt_device.h"

#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 5        /* Time ESP32 will go to sleep (in seconds) */

#define OLED_SDA 21 // data
#define OLED_SCL 22 // clock
#define OLED_RESET -1

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// define the pins used by the transceiver module
#define cs 18 // -> Chip Select (SPI)
#define rst 23
#define dio0 26 // non si usa perchè bisogna settarlo solo per uso interno

unsigned long previousMillis = 0;
const long interval = 5000; // Tempo di attesa in millisecondi

const uint8_t *device_mac_address;

typedef struct
{
  uint8_t IDDEV[3]; // float temp;
} NotifyPacket;

typedef struct
{
  uint8_t IDDEV[6];
} SwitchModePacket;

enum Mode
{
  OPEN_SEA,
  HARBOR
};

String* stolen_ids;
String stolen_found_ids;
bool found_stolen = false;
int counter = 0;
int insert_stolen = 0;
int stolen_boats_qty = 0;
bool withDisplay = false;
NotifyPacket presencePacket;
enum Mode mode = OPEN_SEA;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //&Wire = riferimento ad I2C
String device_id;

bool initBluetooth()
{
  if (!btStart())
  {
    Serial.println("Failed to initialize controller");
    return false;
  }

  if (esp_bluedroid_init() != ESP_OK)
  {
    Serial.println("Failed to initialize bluedroid");
    return false;
  }

  if (esp_bluedroid_enable() != ESP_OK)
  {
    Serial.println("Failed to enable bluedroid");
    return false;
  }
  return true;
}

void initDisplay()
{
  Serial.println("INIT DISPLAY");

  // inizializzazione display OLED
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
    display.print("TEST OLED OK");
    display.display();
  }
}

char buffe[sizeof(uint8_t[3])];

void onReceive(int packetSize){
  if (packetSize == 0) return; // Nessun pacchetto ricevuto  
  String receivedMessage = "";
  sprintf(buffe, "%02x%02x%02x", presencePacket.IDDEV[0], presencePacket.IDDEV[1], presencePacket.IDDEV[2]);
  
  Serial.println("Entrato onReceive");
  if(mode == HARBOR){
    while (LoRa.available()) {
      char actual_char = (char)LoRa.read();

      if(actual_char == ':'){
        if(strcmp(receivedMessage.c_str(), buffe) == 0){
          //sprintf(device_id, "%02x%02x%02x", presencePacket.IDDEV[0], presencePacket.IDDEV[1], presencePacket.IDDEV[2]);
          device_id = receivedMessage.c_str();
          Serial.print("SETTATO DEV ID: ");
          Serial.print(buffe);

          //salva i dati in modo opportuno e passa in modalità OPEN_SEA
          Serial.print("RICEVUTO MESS IN MODE HARBOR - E' MESSAGGIO PER ME - PASSO IN OPEN");
          receivedMessage = "";

        }else{
          Serial.print("RICEVUTO MESS IN MODE HARBOR - NON E' MESSAGGIO PER ME, ESCI");
          break;
        }
      }else if(actual_char == '_'){
        stolen_boats_qty = (int) receivedMessage.toInt();
        
        if(stolen_boats_qty == 0){
          break; //nessuna barca rubata
        }

        if (stolen_ids) {
          delete[] stolen_ids;
          stolen_ids = nullptr;
        }
        
        stolen_ids = new String[stolen_boats_qty];
        receivedMessage = "";
        insert_stolen = 0;
        
      }else if(actual_char == '-'){
        stolen_ids[insert_stolen] = receivedMessage;
        
        insert_stolen++;
        receivedMessage = "";
      }else if(actual_char == '$'){
        stolen_ids[insert_stolen] = receivedMessage;
        
        receivedMessage = "";

        mode = OPEN_SEA;
      }
      else{
        receivedMessage += actual_char;  // Costruisce la stringa ricevuta
      }
    }
  }else if(mode == OPEN_SEA){
    while (LoRa.available()) {
      char actual_char = (char)LoRa.read();

      if(actual_char == ':'){
        if(strcmp(receivedMessage.c_str(), buffe) == 0){
          
          Serial.print("RICEVUTO MESS IN MODE OPEN - E' MESSAGGIO PER ME - PASSO IN HARBOR");
          mode = HARBOR;

          delete[] stolen_ids;
          stolen_ids = nullptr;
    
          receivedMessage = "";
    
        }else{
          Serial.print("RICEVUTO MESS IN MODE OPEN - NON PER ME - CONTROLLO SE E' UNA RUBATA");

          if(stolen_boats_qty != 0){
            Serial.print(" STOLEN QTY:");
            Serial.print(stolen_boats_qty);
            for(int i = 0; i < stolen_boats_qty; i++){
              if(stolen_ids[i] == receivedMessage){
                found_stolen = true;
                stolen_found_ids += receivedMessage + '?';
              }
            }
          }

        }        
      }else if(actual_char == '!'){
        // manda messaggio di alert con la receivedMessage
      }
      else{
        receivedMessage += actual_char;  // Costruisce la stringa ricevuta
      }
    }
  }
}

void setup()
{
  // esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.begin(115200);
  
  LoRa.setPins(cs, rst, dio0);

  // replace the LoRa.begin(---E-) argument with your location's frequency
  // 433E6 for Asia
  // 866E6 for Europe
  // 915E6 for North America
  while (!LoRa.begin(866E6))
  {
    Serial.println(".");
    delay(500);
  }

  // Change sync word (0xF3) to match the receiver (optional)
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  // LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
  initDisplay();
  initBluetooth();

  // ottenere l'indirizzo MAC univoco del modulo Bluetooth integrato nel dispositivo
  device_mac_address = esp_bt_dev_get_address();
  for (int i = 3; i < 6; i++)
  {
    presencePacket.IDDEV[i - 3] = (char)device_mac_address[i];
  }
  esp_bluedroid_disable();
  esp_bluedroid_deinit();

  // Imposta la callback per ricezione pacchetti
  LoRa.onReceive(onReceive);
  LoRa.receive(); // Modalità ricezione continua
}

// sender
void loop() {
  unsigned long currentMillis = millis();

  if(mode == HARBOR){
    // Invia presence packet ogni tot secondi
    if (currentMillis - previousMillis >= interval) {
      Serial.print("HARBOR azione periodica...");
      previousMillis = currentMillis;

      Serial.print("Sending packet: ");
      Serial.println(counter);
      
      //Send LoRa packet to receiver
      LoRa.beginPacket();
      
      uint8_t * buff = (uint8_t *)(&presencePacket);
      for(int i = 0; i < sizeof(NotifyPacket); i++){
        LoRa.write(buff[i]);
        Serial.println("CICLO X");
        Serial.println(buff[i]);
      }
      Serial.printf("ID Sensore: %02x%02x%02x\n", buff[0], buff[1], buff[2]);
      LoRa.endPacket();
      LoRa.receive();
      counter++;
    }
  }

  if(mode == OPEN_SEA){
    // Esegui un'azione ogni tot secondi
    if (currentMillis - previousMillis >= interval) {
      Serial.println("OPEN_SEA azione periodica..."); 
      previousMillis = currentMillis;

      //Send LoRa packet to receiver
      /*
      LoRa.beginPacket();
      
      String message = device_id + ":";  // Stringa di esempio -> numero_rubate:id_barca:id
      Serial.print("Invio: ");
      Serial.println(message);

      LoRa.beginPacket();        // Inizia il pacchetto
      LoRa.print(message);       // Invia la stringa
      LoRa.endPacket();          // Chiude il pacchetto e invia

      LoRa.receive();
      */

      if(!found_stolen){
        Serial.print("FOUND STOLEN:");
        Serial.print(found_stolen);
      }else{
        LoRa.beginPacket();
        
        Serial.print("Invio barche rubate: ");
        Serial.println(device_id + stolen_found_ids + '!');

        LoRa.beginPacket();        // Inizia il pacchetto
        LoRa.print(stolen_found_ids);       // Invia la stringa
        LoRa.endPacket();          // Chiude il pacchetto e invia

        LoRa.receive();
        found_stolen = false;
        stolen_found_ids = "";
      }
    }
  }
}