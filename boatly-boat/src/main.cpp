#include <LoRa.h>
#include <Arduino.h>
#include <Wire.h> // libreria per I2C
#include "model_data.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include <map>

#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

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
int insert_stolen = 0;

String* stolen_ids = nullptr;
boolean stolen_found = false;

std::map<String, int> retransmission_check;

const int MAX_RETRANSMISSIONS_QTY = 4;

namespace {
  tflite::ErrorReporter* error_reporter = nullptr;
  const tflite::Model* model = nullptr;
  tflite::MicroInterpreter* interpreter = nullptr;
  TfLiteTensor* input = nullptr;
  TfLiteTensor* output = nullptr;
  int inference_count = 0;
  
  constexpr int kTensorArenaSize = 2000;
  uint8_t tensor_arena[kTensorArenaSize];
  }  // namespace

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
        Serial.println("HARBOR MODE entrato");
        if(strcmp(receivedMessage.c_str(), device_id) == 0){
          Serial.println(" HARBOR  arrivata l'autorizzazione di uscire, setto variabile e passo in OPEN_SEA");
          mode = OPEN_SEA;
          receivedMessage = "";
        }else{
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
          retransmission_check.clear();
    
          receivedMessage = "";
          break;
        }else{
          //Sono in OPEN e mi è arrivato un id diverso dal mio, parte controllo per barche rubate
          if(stolen_boats_qty != 0){
            Serial.print(" STOLEN QTY:");
            Serial.print(stolen_boats_qty);
            for(int i = 0; i < stolen_boats_qty; i++){
              if(stolen_ids[i] == receivedMessage && (retransmission_check[receivedMessage] < MAX_RETRANSMISSIONS_QTY)){  //caso in cui capto una rubata
                Serial.print("captata rubata, contatore - attuale: ");
                Serial.println(retransmission_check[receivedMessage]);
                retransmission_check[receivedMessage]++;
                strcpy(stolen_id, receivedMessage.c_str());
                Serial.print("ok trovato: ");
                Serial.print(stolen_id);
                stolen_found = true;
              }
            }
          }
        }
        receivedMessage = "";
      }else if(actual_char == '$'){ //caso in cui qualcuno ha ritrasmesso una rubata
        if(!stolen_found && (retransmission_check[receivedMessage] < MAX_RETRANSMISSIONS_QTY)){
          Serial.print("captata rubata, contatore - attuale: ");
          Serial.println(retransmission_check[receivedMessage]);
          retransmission_check[receivedMessage]++;
          Serial.print(device_id);
          strcpy(stolen_id, receivedMessage.c_str());
          Serial.print(device_id);
          stolen_found = true;
        }
      }else{
        receivedMessage += actual_char;
      }
    }
  }
}

void checkWind(){
  int8_t fake_wind = rand() % 51;  // Input random generation for wind

  input->data.f[0] = fake_wind;

  if (interpreter->Invoke() != kTfLiteOk) {
      Serial.println("Error in execution of model");
      return;
  }

  int8_t result = output->data.f[0];
  Serial.print("CHECK WIND: ");
  Serial.print(fake_wind);

  if(result == 0){
    Serial.println(" - *NORMAL*");
  }else if(result == 1){
    Serial.println(" - **WARNING**");
  }
  else if(result >= 2){
    Serial.println(" - ***DANGER***");
  }

}

void setup() {
  Serial.begin(115200);
  LoRa.setPins(LORA_CS, LORA_RST, dio0);
  srand(time(NULL));
  mode = HARBOR;

  while (!LoRa.begin(866E6)) {
      Serial.print(".");
      delay(500);
  }
  Serial.println();

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

  /////////////////
  // Set up logging. Google style is to avoid globals or statics because of
  // lifetime uncertainty, but since this has a trivial destructor it's okay.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(model_tflite);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Model provided is schema version %d not equal "
                         "to supported version %d.",
                         model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  // This pulls in all the operation implementations we need.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::AllOpsResolver resolver;

  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
    return;
  }

  // Obtain pointers to the model's input and output tensors.
  input = interpreter->input(0);
  output = interpreter->output(0);

  // Keep track of how many inferences we have performed.
  inference_count = 0;
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 5000) {
    Serial.print("\n|||Routine execution|||\n");

    checkWind();

    String payload = "";
    payload = payload + device_id + ':';// + stolen_origin +  stolen_id;

    Serial.print(payload);
    LoRa.beginPacket();
    LoRa.print(payload);
    LoRa.endPacket();
    LoRa.receive();

    previousMillis = currentMillis;
    
  }

  if(stolen_found){
    String payload = "";
    payload = payload + device_id + ':' + stolen_id + '$' ;

    Serial.print("\n invio barca rubata trovata ");
    Serial.print(payload);
    LoRa.beginPacket();
    LoRa.print(payload);
    LoRa.endPacket();
    LoRa.receive();
    stolen_found = false;
    stolen_id[0] = '\0';
  }
}