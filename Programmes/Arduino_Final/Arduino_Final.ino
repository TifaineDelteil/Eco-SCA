#include<SoftwareSerial.h>
#include <OneWire.h> //Librairie du bus OneWire
#include <DallasTemperature.h> //Librairie du capteur
#include <Servo.h>
#include <Adafruit_NeoPixel.h> // appel de la bibliothèque 


#include <dht.h>
#include "dht.h"

#define nb_led 24 // le module comporte 24 led

// I/O Numérique

// Le port 2 est pris par la sonde mais mis lors de l'initialisation ##
const int Ruban_LED = 3;  // affectation des broches
const int Chauffage = 5;
const int TX = 6; 
const int RX = 7;
const int servo = 9;
const int Relais = 10;

// I/O Analogique

const int LDR = A0;  // Photoréistance
const int dht_apin = A1; // Capteur de température et d'humidité de l'air
const int capteur_niveau = 2; // Capteur de niveau d'eau

// Variable à initaliser

float temperature_eau = 0;
float lumiere = 0;
float pourcentage_niveau = 0;
int temporisation_arrosage = 0;
int compteur_alerte =0;
char Connection = "";
int Position_act = 90;

SoftwareSerial Wif(RX,TX);
OneWire oneWire(2); //Bus One Wire sur la pin 2 de l'arduino
DallasTemperature sensors(&oneWire); //Utilistion du bus Onewire pour les capteurs
DeviceAddress sensorDeviceAddress; //Vérifie la compatibilité des capteurs avec la librairie
dht DHT;
Servo Servomoteur; //créer un objet Servo pour contrôler le servomoteur
Adafruit_NeoPixel module = Adafruit_NeoPixel(nb_led, Ruban_LED, NEO_GRB + NEO_KHZ800);  // création de l'objet module

// ################################################################ Setup ################################################################

void setup(){  
 
  Serial.begin(9600);
  Wif.begin(9600);  // Initialisation de la communication avec le module wifi
  delay(1000);

//  ......................... Servomoteur  
  Servomoteur.attach(servo);   //attache le servomoteur à la broche D9
  Servomoteur.write(90);    //met le servomoteur à la position 0°
//  ......................... Chauffage
  pinMode(Relais,OUTPUT);
  digitalWrite(Relais,HIGH);
//  ......................... Pompe
  pinMode(Chauffage,OUTPUT);
  digitalWrite(Chauffage,LOW);
//  ......................... Photorésistance
  pinMode(LDR, INPUT);
// ......................... Sonde de température
 sensors.begin(); //Activation des capteurs
 sensors.getAddress(sensorDeviceAddress, 0); //Demande l'adresse du capteur à l'index 0 du bus
 sensors.setResolution(sensorDeviceAddress, 12); //Résolutions possibles: 9,10,11,12
// ......................... Ruban de LED
  module.begin();  // initialisation de module
}

// ################################################################ Loop ################################################################

void loop(){
      // Permet d'informer l'utilisateur de l'état de la connexion
  if (Wif.available()){
    while (Wif.available()){
      Connection = "";
      Connection = Wif.read();
    }
  }
    
    // Exécution des fonctions complémentaire
  //Serial.print("Niveau d'eau :   ");
  capteur_niveau_eau();
  //Serial.print("Température extérieur :   ");
  capteur_temperature();
  //Serial.print("Luminosité :   ");
  capteur_luminosite();
  Ruban_de_Led();
  //Serial.print("Température de l'eau :   ");
  sonde();

      // Envoie des donnés

  String Temperature_envoie=("T")+String(DHT.temperature);
  Wif.print(Temperature_envoie);
  Serial.print("Température de l'air : ");
  Serial.print(DHT.temperature);
  Serial.println(" °C");
  
  String Humidite_envoie=("H")+String(DHT.humidity);
  Wif.print(Humidite_envoie);
  Serial.print("Humidité de l'air : ");
  Serial.print(DHT.humidity);
  Serial.println(" %");
  
  String Temperature_eau_envoie=("t")+String(temperature_eau);
  Wif.print(Temperature_eau_envoie);
  Serial.print("Température de l'eau : ");
  Serial.print(temperature_eau);
  Serial.println(" °C");
  
  String Luminosite_envoie=("L")+String(lumiere);
  Wif.print(Luminosite_envoie);
  Serial.print("Luminosité : ");
  Serial.print(lumiere);
  Serial.println(" %");

  String Niveau_eau_envoie=("N")+String(pourcentage_niveau);
  Wif.print(Niveau_eau_envoie);
  Serial.print("Niveau d'eau : ");
  Serial.print(pourcentage_niveau);
  Serial.println(" %");
  
  Serial.println();
  Serial.println();
  Serial.println();
  
  delay(25000);    // Délais entre chaque envoie de donné pour laisser le temps au module wifi de les traiter
}

// ################################################################ Fonctions complémentaire ################################################################
void capteur_temperature(){
    DHT.read11(dht_apin);
    
    if (DHT.temperature < 25){
      chauffage(LOW);
      if (DHT.temperature <25 && Position_act == 1){
        servomoteur(0);
        Position_act = 0;
        Serial.println("la");
      }
    else if (DHT.temperature >= 25 && Position_act == 0) {
          servomoteur(1);
          Position_act = 1;
      }
    }
    
    if ((((DHT.humidity < 30 && temporisation_arrosage > 100) || temporisation_arrosage >= 1000))&&(pourcentage_niveau >= 15)){
      temporisation_arrosage = 0;
      Arrossage(10000);
    }
    else {
      temporisation_arrosage++;
    }
  Serial.println(temporisation_arrosage);
}

void Arrossage ( int temps_arrossage) {
  digitalWrite(Relais, LOW);
  delay(temps_arrossage);
  digitalWrite(Relais, HIGH);
}

void capteur_luminosite(){
  float valeur_luminosite = analogRead(LDR);  //valeur analogique de la luminosité
  lumiere= ((1023-valeur_luminosite)/1023)*100;   //valeur lumonisté (%)
  }
  

void sonde(void){
 temperature_eau=sensors.getTempCByIndex(0);  //Récupération de la température en celsius du capteur n°0
 sensors.requestTemperatures(); //Demande la température aux capteurs
}

void capteur_niveau_eau(){
  float val_niveau = analogRead (capteur_niveau);// Lecture de la valeur du capteur
  pourcentage_niveau = (val_niveau/620)*100;
}


void servomoteur(int Position) {
// de 0° à 80°
  if (Position == 1){
    for (int position =0; position <=90; position++) { 
      Servomoteur.write(position);
      Serial.println("Ouverte");
      delay(15);  
    }
  }  
// de 80° à 0°
  else {
    for (int position = 90; position >=0; position--) {
      Servomoteur.write(position);
      Serial.println("Fermée");
      delay(15);
    }
  }
}

void chauffage(char Etat) {
  digitalWrite(Chauffage,Etat);
}

void Ruban_de_Led() {
  if (lumiere<=60){
  module.setBrightness(100-lumiere);
    for(int i=0;i<7;i++){ // 8 premières led en bleu
      module.setPixelColor(i,0,0,150); 
      module.show(); 
    }
    for(int i=7;i<14;i++){  // 8 led suivantes en blanc
      module.setPixelColor(i,150,150,150); 
      module.show(); 
    }
    for(int i=14;i<21;i++){ // 8 dernières led en rouge
      module.setPixelColor(i,150,0,0); 
      module.show(); 
    }
  }
  else{
        for(int i=0;i<24;i++){ // Toutes les led en rouge
          module.setPixelColor(i,0,0,0); 
          module.show(); 
        }
   }
   Etat();
}

void Etat(){  // Affiche sur les trois dernières led une couleur en fonction de l'état de deux paramètre, le niveau d'eau et la connexion
  if (Connection == 'N' && pourcentage_niveau <= 15) {
      compteur_alerte =0;
      while(compteur_alerte<10) {
        for(int i=21;i<24;i++){ // Toutes les led en rouge
          module.setPixelColor(i,150,0,0); 
          module.show(); 
        }
        delay(50);
        for(int i=21;i<24;i++){ // Toutes les led en jaune
          module.setPixelColor(i,255,140,0); 
          module.show();
        }
        delay(50);
        compteur_alerte++;
      }  
  }
  else if (Connection == 'N' && pourcentage_niveau > 15){
    compteur_alerte =0;
      while(compteur_alerte<20) {
        for(int i=21;i<24;i++){ // Toutes les led en éteinte
          module.setPixelColor(i,0,0,0); 
          module.show(); 
          delay(75);
        }
        for(int i=21;i<24;i++){ // Toutes les led en jaune
          module.setPixelColor(i,255,140,0); 
          module.show();
          delay(75);
        }
        compteur_alerte++;
      }  
  }
  else if (Connection == 'O' && pourcentage_niveau <= 15){
    compteur_alerte =0;
      while(compteur_alerte<20) {
        for(int i=21;i<24;i++){ // Toutes les led en éteinte
          module.setPixelColor(i,0,0,0); 
          module.show(); 
        }
        delay(50);
        for(int i=21;i<24;i++){ // Toutes les led en rouge
          module.setPixelColor(i,150,0,0); 
          module.show();
        }
        delay(50);
        compteur_alerte++;
      }  
  }
  else {
     for(int i=21;i<24;i++){ // Toutes les led en verte
        module.setPixelColor(i,0,255,0); 
        module.show(); 
        }    
  }
}
