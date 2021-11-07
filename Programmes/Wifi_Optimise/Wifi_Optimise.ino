#include "ThingSpeak.h"
#include <ESP8266WiFi.h>

#include<SoftwareSerial.h>

char Data;

int Information_serre=0;
int Canal=0;


//----------------  Fournir les informations de connection   ---------------------
char ssid[] = "Eco-SCA";             // Nom du réseau wifi 
char pass[] = "Serre19100";         // Mot de passe du réseau
unsigned long myChannelNumber = 993496;  
const char * myWriteAPIKey = "DZRN7QMKSN5D6H9C";    // Entrer le code ThingSpeak
//------------------------------------------------------------------

WiFiClient  client;


void setup() {
  //Initialise la communication et attend que la connection soit faite
  Serial.begin(9600);
  while (!Serial) {
    ; // Attend que la connection soit faite
  }

  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client); 
}

void loop() {

  // Connecte ou reconnecte au réseau
  if(WiFi.status() != WL_CONNECTED){
    //Serial.print("En attente de connexion aux SSID: ");
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);      
      Serial.print("N"); 
      //Serial.print(".");
      delay(5000);
 
    } 
    //Serial.println("\nConnecté.");
    Serial.print("O");
    //Serial.print(".");    
  }
  if (Serial.available()){
    while (Serial.available()){
      Data=Serial.read(); 
      //Serial.println(Data);
      if (Data=='T') { 
        Information_serre=Serial.parseInt();
        //Serial.println(Information_serre);
        Canal=1;
      }  
      if (Data=='H') { 
        Information_serre=Serial.parseInt();
        //Serial.println(Information_serre);
        Canal=2;
      }  
      if (Data=='t') { 
        Information_serre=Serial.parseInt();
        //Serial.println(Information_serre);
        Canal=3;
      }
      if (Data=='L') { 
        Information_serre=Serial.parseInt();
        //Serial.println(Information_serre);
        Canal=4;
      }
      if (Data=='N') { 
        Information_serre=Serial.parseInt();
        //Serial.println(Information_serre);
        Canal=5;
      }
      ThingSpeak.setField(Canal,Information_serre);    
    } 
  }

  // Ecrit dans ThingSpeak, il y a 8 canaux par chaine, c'est Canal de 1 à 8
  int wifi_envoie = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  
  // Envoie un  code de retour pour savoir si l'information a bien été transmise
  if(wifi_envoie == 200){
    //Serial.println("Channel update successful.");
  }
  else{
    //Serial.println("Problem updating channel. HTTP error code " + String(wifi_envoie));
  }

  
  delay(20000); // Attend 20 secondes
}
