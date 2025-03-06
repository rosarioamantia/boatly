#include <LoRa.h>
#include <Arduino.h>
#include <Wire.h> // libreria per I2C

#include "esp_bt_main.h"
#include "esp_bt_device.h"

#define OLED_SDA 21 // data
#define OLED_SCL 22 // clock
#define OLED_RESET -1

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// define the pins used by the transceiver module
#define cs 18 // -> Chip Select (SPI)
#define rst 23
#define dio0 26 // non si usa perchè bisogna settarlo solo per uso interno

float temp = 10;
char device_id[8];
char stolen_id[8];
unsigned long previousMillis = 0;
int stolen_boats_qty = 0;
String* stolen_ids = nullptr;
int insert_stolen = 0;
boolean stolen_found = false;

enum Mode
{
  OPEN_SEA,
  HARBOR
};
Mode mode;

bool initBluetooth()
{
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

  return true; 
}

void onReceive(int packetSize){
  if (packetSize == 0) return;
  String receivedMessage = "";

  
  if(mode == HARBOR){
    //Serial.print("RICEVUTO MESSAGGIO IN MODE HARBOR");

    while(LoRa.available()){
      char actual_char = (char)LoRa.read();
 
      if(actual_char == ':'){
        Serial.println("HARBOR entrato");
        if(strcmp(receivedMessage.c_str(), device_id) == 0){//   receivedMessage == device_id){
          //Sono in HARBOR e mi è arrivata l'autorizzazione di uscire, setto variabile e passo in OPEN_SEA
          Serial.println(" HARBOR  arrivata l'autorizzazione di uscire, setto variabile e passo in OPEN_SEA");
          mode = OPEN_SEA;
          receivedMessage = "";
        }else{
          //Sono in HARBOR e mi è arrivato un id diverso dal mio, non faccio niente
          Serial.println(" HARBOR nessuna azione");
          break;
        }
      }else if(actual_char == '_'){
        stolen_boats_qty = receivedMessage.toInt();
        Serial.print("STOLEN BOATS QTY:");
        Serial.println(stolen_boats_qty);
        
        if(stolen_boats_qty == 0){
          Serial.println(" HARBOR nessuna barca rubata");
          break; //nessuna barca rubata // in teoria non ci si entrerà mai, controllo di sicurezza
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
      }
      else{
        receivedMessage += actual_char;
      }
    }
    
  }else if(mode == OPEN_SEA){
    Serial.println("RICEVUTO MESSAGGIO IN MODE IN OPEN_SEA");

    while(LoRa.available()){
      char actual_char = (char)LoRa.read();
 
      if(actual_char == ':'){
        Serial.println("OPEN: entrato");
        if(strcmp(receivedMessage.c_str(), device_id) == 0){//   receivedMessage == device_id)
          //Sono in OPEN e mi è arrivata l'autorizzazione di rientrare, setto variabile e passo in HARBOR
          Serial.println("OPEN ricevuto rientro, passo in mod HARBOR ");
          mode = HARBOR;

          delete[] stolen_ids;
          stolen_ids = nullptr;
    
          receivedMessage = "";
          break;
        }else{
          //Sono in OPEN e mi è arrivato un id diverso dal mio, parte controllo per barche rubate
          if(stolen_boats_qty != 0){
            Serial.print(" STOLEN QTY:");
            Serial.print(stolen_boats_qty);
            for(int i = 0; i < stolen_boats_qty; i++){
              if(stolen_ids[i] == receivedMessage){
                strcpy(stolen_id, receivedMessage.c_str());
                Serial.print("ok trovato: ");
                Serial.print(stolen_id);
                stolen_found = true;
              }
            }
          }
        }
        receivedMessage = "";
      }else if(actual_char == '$'){
        Serial.print("OKOK  ");
        if(!stolen_found){
          Serial.print(" SI:");
          Serial.print(device_id);
          strcpy(stolen_id, receivedMessage.c_str());
          Serial.print(" SI:");
          Serial.print(device_id);
          stolen_found = true;
        }
      }else{
        receivedMessage += actual_char;
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  LoRa.setPins(LORA_CS, LORA_RST, dio0);
  mode = HARBOR;

  while (!LoRa.begin(866E6)) {
      Serial.print(".");
      delay(500);
  }
  Serial.println();

  //LoRa.setSyncWord(0xcc);
  Serial.println("LoRa Initializing OK!");

  initBluetooth();
  const uint8_t* bluetooth_addr = esp_bt_dev_get_address();
  uint8_t TEST_IDDEV[3];

  /*
  for (int i = 0; i < 3; i++) {
    TEST_IDDEV[i] = (char) bluetooth_addr[i];
  }
  */

  sprintf(device_id, "%02x%02x%02x", (char) bluetooth_addr[0], (char) bluetooth_addr[1], (char) bluetooth_addr[2]);  

  Serial.print("device_id: ");
  Serial.println(device_id);
  
  esp_bluedroid_disable();
  esp_bluedroid_deinit();

  LoRa.onReceive(onReceive);
  LoRa.receive();
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 5000) {
    Serial.print("\nESEGUO azione periodica...");

    String payload = "";
    payload = payload + device_id + ':';// + stolen_origin +  stolen_id;

    Serial.print(payload);
    LoRa.beginPacket();        // Inizia il pacchetto
    LoRa.print(payload);       // Invia la stringa
    LoRa.endPacket();
    LoRa.receive();

    previousMillis = currentMillis;
    
  }

  if(stolen_found){
    String payload = "";
    payload = payload + device_id + ':' + stolen_id + '$' ;// + stolen_origin +  stolen_id;

    Serial.print("\n OKOKOK INVIO: ");
    Serial.print(payload);
    LoRa.beginPacket();        // Inizia il pacchetto
    LoRa.print(payload);       // Invia la stringa
    LoRa.endPacket();
    LoRa.receive();
    stolen_found = false;
    stolen_id[0] = '\0';
  }
}