#if defined(ESP8266)
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#else
#include <WiFi.h>
#endif


//needed for library
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>         //https://github.com/tzapu/WiFiManager

#include <Update.h>

//Display
#define T4_V13
#include "T4_V13.h"
#include <TFT_eSPI.h>
#include <SPI.h>

//server
AsyncWebServer server(80);
DNSServer dns;

//Display
TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

//FileSystem
#include "SPIFFS.h"

//WaterDrop Data
#include "wd_types.h"

#define SKETCH_VERSION "0.1.36"
#define TRIGGER_PIN 38

const char* serverIndex = "<form method='POST' action='/upload' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void setupSPIFFS(){
  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
}
void setupTFT(){
    tft.init();
    tft.setRotation(3);

    if (TFT_BL > 0) {
        pinMode(TFT_BL, OUTPUT);
        digitalWrite(TFT_BL, HIGH);
    }
}
void setupWifiManager(){
  AsyncWiFiManager wifiManager(&server,&dns);
  wifiManager.setTimeout(180);
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(2);
  tft.setTextFont(1);    
  tft.fillScreen(TFT_RED);    
  tft.setCursor(0, 0);
  tft.println("Welcome");
  tft.println("");
  tft.println("Connect 'ESP...' Wifi");
    
  if(!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(5000);
  }  
  tft.setTextColor(TFT_WHITE);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);
  
  tft.println("Water Drop Photography");
  tft.println("");
  tft.print("Version: ");
  tft.println(SKETCH_VERSION);
  tft.println("");
  //if you get here you have connected to the WiFi
  tft.print("Connect: ");
  tft.println(WiFi.localIP());
}

void clearWifiCredentials(){
  tft.setTextColor(TFT_BLACK);  
  tft.fillScreen(TFT_WHITE);
  tft.setCursor(0, 0);  
  tft.println("clear Wifi Credentials");
  WiFi.disconnect(true,true);
  delay(1000);
  ESP.restart();
}

static void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  
   
  if (!index){
    tft.setTextColor(TFT_WHITE);    
    tft.fillScreen(TFT_BLUE);
    tft.setCursor(0, 0);
    tft.println("Water Drop Photography");
    tft.println("");    
    tft.println("Update in Progress...");
  
    if (!Update.begin()) { //start with max available size
          Update.printError(Serial);
        }
  }
  
  tft.fillRect(0, 60, 239, 15, TFT_BLUE);
  tft.setCursor(0, 60);
  tft.setTextColor(TFT_WHITE);
  tft.println(index+len);
  
  if (Update.write(data, len) != len) {
          Update.printError(Serial);
        }
        
  if(final){
    Serial.printf("UploadEnd: %s, %u B\n", filename.c_str(), index+len);
    Update.end(true);
    request->send(200);
    
    tft.println(" ");
    tft.println("Update in Done!");
    tft.println("Restarting...");
    
    
    ESP.restart();
  }
}

void setupWebOTA() {
  
  server.on("/upload", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(200, "text/html", serverIndex);
    });
    server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request){
        request->send(200);
    }, handleUpload);  

}

// Replaces placeholder with LED state value
String processor(const String& var){
  String RetVal= String();
  if(var == "NBDEV"){    
    RetVal= String(NB_DEV);
  }
 
  if(var == "VERSION"){    
    RetVal= SKETCH_VERSION;
  }

  if (var == "DEVLIST"){
    for (int i = 0 ; i < NB_DEV ; ++i)
    {
      RetVal += "<div class=\"col-sm-3 p-1\"><div class=\"p-2 WD_TYPE" + String(wd_device[i].type) + "\"><H4>" + wd_device[i].name + "</H4>";

      // Pin
      RetVal += "<div class=\"input-group mb-1\"><div class=\"input-group-prepend\"><span class=\"input-group-text\">";
      RetVal += "Broche";
      RetVal += "</span></div>";      
      RetVal += "<input class=\"form-control\" type=\"number\" name=\"pin"+ wd_device[i].name + "\" value=\"" + String(wd_device[i].pin) + "\">";
      RetVal += "</div>";
      
      //RetVal += "<tr><td>Broche :</td><td><input type=\"number\" name=\"pin"+ wd_device[i].name + "\" value=\"" + String(wd_device[i].pin) + "\"></td></tr>";
      for (int j = 0 ; j < NB_DROP ; ++j) {

        //Trigger
        RetVal += "<div class=\"drop\">";
        RetVal += "<div class=\"input-group mb-1\">";
        RetVal += "<div class=\"input-group-prepend\">";
        RetVal += "<span class=\"input-group-text\">";
        RetVal += "Ext Trig ";
        RetVal += "</span></div>";
  
        RetVal += "<select  class=\"custom-select\" name=\"trig" + String(j) + wd_device[i].name + "\" id=\"trig" + String(j) + wd_device[i].name + "\">";
        RetVal += "<option value=\""+ String(NB_TRIGGER) +"\"";
        RetVal += wd_trig[i*NB_DROP+j]==NB_TRIGGER?"selected":"";
        RetVal += ">-- NO TRIG --</option>";

          for (int k = 0 ; k < NB_TRIGGER ; ++k){
            RetVal += "<option value=\"" + String(k) + "\"";
            RetVal += wd_trig[i*NB_DROP+j]==k?"selected":"";
            RetVal += ">"+ wd_trigger[k].name + "</option>";
          }
        RetVal += "</select></div>";
        
        // Délai
        RetVal += "<div class=\"input-group mb-1\"><div class=\"input-group-prepend\"><span class=\"input-group-text\">";
        RetVal += "Délai " + String(j+1);
        RetVal += "</span></div>";      
        RetVal += "<input class=\"form-control\" type=\"number\" name=\"delai" + String(j) + wd_device[i].name + "\" value=\"" + String(wd_delai[i*NB_DROP+j]) + "\">";
        RetVal += "</div>";
      
        // Durée
        RetVal += "<div class=\"input-group \"><div class=\"input-group-prepend\"><span class=\"input-group-text\">";
        RetVal += "Durée " + String(j+1);
        RetVal += "</span></div>";      
        RetVal += "<input class=\"form-control\" type=\"number\" name=\"duree" + String(j) + wd_device[i].name + "\" value=\"" + String(wd_duree[i*NB_DROP+j]) + "\">";
        RetVal += "</div></div>";
      }
      
      RetVal += "</div></div>";
    }
  }
  return RetVal;
}

void setupWebMain() {
   // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  
  server.on("/go", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  server.on("/go", HTTP_POST, [](AsyncWebServerRequest *request){
    int paramsNr = request->params();
    unsigned long ulMaxTime = 0L;
    
    Serial.print("Nb Parameters: ");
    Serial.println(paramsNr);
    
    for(int k=0;k<NB_TRIGGER+1;k++){
      wd_trig_MaxTime[k] = 0L;
      wd_trigger[k].en = false;
     }
    
    for(int i=0;i<NB_DEV;i++){
      wd_device[i].pin = atoi(request->getParam("pin"+ wd_device[i].name,true )->value().c_str());
      for (int j = 0 ; j < NB_DROP ; ++j )
      {
        //delai
        wd_delai[i*NB_DROP+j] = atol(request->getParam("delai"+ String(j)+ wd_device[i].name,true )->value().c_str());
        //duree
        wd_duree[i*NB_DROP+j] = atol(request->getParam("duree"+ String(j)+ wd_device[i].name,true )->value().c_str());
        //Trigger
        wd_trig[i*NB_DROP+j] = atol(request->getParam("trig"+ String(j)+ wd_device[i].name,true )->value().c_str());

        // Max Time
        if (wd_trig_MaxTime[wd_trig[i*NB_DROP+j]] < (wd_duree[i*NB_DROP+j]+wd_delai[i*NB_DROP+j]))
        {
          wd_trig_MaxTime[wd_trig[i*NB_DROP+j]] = (wd_duree[i*NB_DROP+j]+wd_delai[i*NB_DROP+j])+100;
        }
      } 
    }
    process_drop();
    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
}


 void process_drop( void ) {
    unsigned long gulStartTime = 0L;
    unsigned long gultemp = 0L;
    unsigned long gulCurrent = 0L;
    bool bEndLoop = false;
    
    for (int i = 0 ; i < NB_DEV ; ++i) {
      if (wd_device[i].type == WD_OUT)
      {        
        pinMode(wd_device[i].pin, OUTPUT);
        digitalWrite(wd_device[i].pin, LOW);
      }
      else
      {
        pinMode(wd_device[i].pin, INPUT);
      }
    }
    
    Serial.println("Start Drop");
    for ( int k = 0 ; k < NB_TRIGGER+1 ; ++k){
      wd_trig_Current[k] = 0L;
      Serial.print("MaxTime");
      Serial.println(wd_trig_MaxTime[k]);              
    }
      
    wd_trig_Start[NB_TRIGGER] = millis();
    while( bEndLoop == false ) {
      gulCurrent = millis();
      for ( int k = 0 ; k < NB_TRIGGER ; ++k){
        if (wd_trigger[k].en == 1)
        {
          wd_trig_Current[k] = gulCurrent-wd_trig_Start[k];
        }
      }
      wd_trig_Current[NB_TRIGGER] = gulCurrent-wd_trig_Start[NB_TRIGGER];
      
      for (int i = 0 ;  i < NB_DEV ; ++i) {
        for (int j = 0 ; j < NB_DROP ; ++j) {
          
          if (wd_duree[i*NB_DROP+j] > 0){
            // Get Current fn Trig
            gultemp = wd_delai[i*NB_DROP+j];  
            gulCurrent = wd_trig_Current[wd_trig[i*NB_DROP+j]];
                        
            if ( gulCurrent >= gultemp ) {
              if (gulCurrent < (gultemp+wd_duree[i*NB_DROP+j])) {
                if (wd_device[i].en == 0) {
                  Serial.print(gulCurrent);
                  Serial.println(wd_device[i].name);
                  wd_device[i].en = 1+i*NB_DROP+j;
                }
                digitalWrite(wd_device[i].pin, HIGH);
              }
              else {
                if (wd_device[i].en == (1+i*NB_DROP+j)) {
                  Serial.print(gulCurrent);
                  Serial.print(wd_device[i].name);
                  Serial.println("Done");              
                  wd_device[i].en = 0;
                }
                digitalWrite(wd_device[i].pin, LOW);
              }
            }
            
          }
        }
      }

      bEndLoop = true;
      if (wd_trig_Current[NB_TRIGGER] < 4000L) { // < 4s
        for ( int k = 0 ; k < NB_TRIGGER+1 ; ++k){
          if (wd_trig_Current[k] < wd_trig_MaxTime[k]){
            bEndLoop = false;
          }
        }
      }
      else {
        Serial.println("TimeOut");
      }
      delay(1);
    }    
    Serial.println("End Drop");
 }
 
void setupWebServer() {
  setupWebOTA();
  setupWebMain();
  server.onNotFound(notFound);
  server.begin();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(TRIGGER_PIN,INPUT);
  for ( int k = 0 ; k < NB_TRIGGER ; ++k){
    pinMode(wd_trigger[k].pin,INPUT);
  }

  for ( int k = 0 ; k < NB_DROP*NB_DEV ; ++k){
    wd_trig[k] = NB_TRIGGER;
  }
      
  setupSPIFFS();
  setupTFT();
  setupWifiManager();
  setupWebServer();
}

  unsigned long gstart;
void loop() {
  // is configuration portal requested?
  if ( digitalRead(TRIGGER_PIN) == LOW ) {
    clearWifiCredentials();
  }
  gstart = millis();
  
  for ( int k = 0 ; k < NB_TRIGGER ; ++k){
    if ( (digitalRead(wd_trigger[k].pin) == LOW ) && (wd_trigger[k].en == false ) ){
      wd_trig_Start[k] = gstart;
      wd_trigger[k].en = true;
      Serial.println(wd_trigger[k].name);
    }
  }
      
  delay(1);
  
}

