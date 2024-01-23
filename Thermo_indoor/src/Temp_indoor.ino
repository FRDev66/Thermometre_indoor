#include <SimpleDHT.h>
#include <LiquidCrystal.h>
#include <ESP8266WiFi.h>  
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <PubSubClient.h> //Librairie pour la gestion Mqtt 

// DEBUT SECTION DECLARATION - CONNEXION
// DECLARATION CONNEXION I - WiFi

byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// WiFi paramètre
char ssid[] = "Livebox-1F90";    // your SSID
char pass[] = "o3jwTuDzadcmQAtZ2r";       // your SSID Password

// DECLARATION CONNEXION II - BDD

IPAddress server_addr(192,168,1,61);  // IP of the MySQL *server* here
char user[] = "arduino";              // MySQL user login username
char password[] = "test";        // MySQL user login password

//MQTT
const char* mqtt_server = "192.168.1.61";//Adresse IP du Broker Mqttconst int mqttPort = 1883; //port utilisé par le Broker 
long tps=0;

// MQTT Broker  
#define MQTT_BROKER       "192.168.1.61"
#define MQTT_BROKER_PORT  1883
#define MQTT_USERNAME     "frdev66"
#define MQTT_KEY          "Lenems66!!" 

WiFiClient espClient;            // Use this for WiFi instead of EthernetClient
MySQL_Connection conn((Client *)&espClient);
MySQL_Cursor* cursor;
PubSubClient client(espClient);

// FIN SECTION DECLARATION - CONNEXION

// DEBUT SECTION DECLARATION - MESURES
// for DHT11, 
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2
int pinDHT11 = D0;
SimpleDHT11 dht11(pinDHT11);

// FIN SECTION DECLARATION - MESURES

// DEBUT SECTION DECLARATION - ECRAN LCD

// définition des broches auxquelles on a branché l'afficheur LCD
/*
const int pinRS = 4;      // broche 4 (RS) de l'afficheur branchée à GPIO04 de l'ESP8266
const int pinEnable = 5;  // broche 6 (Enable) de l'afficheur branchée à GPIO05 de l'ESP8266
const int pinD4 = 12;  // broche 11 (D4) de l'afficheur branchée à GPIO12 de l'ESP8266
const int pinD5 = 13;  // broche 12 (D5) de l'afficheur branchée à GPIO13 de l'ESP8266
const int pinD6 = 14;  // broche 13 (D6) de l'afficheur branchée à GPIO14 de l'ESP8266
const int pinD7 = 15;  // broche 14 (D7) de l'afficheur branchée à GPIO15 de l'ESP8266

// initialize the library with the numbers of the interface pins
//LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
//LiquidCrystal lcd(6, 5, 4, 3, 2, 1);
LiquidCrystal lcd(pinRS, pinEnable, pinD4, pinD5, pinD6, pinD7);
*/
// FIN SECTION DECLARATION - ECRAN LCD 

// DEBUT SECTION DECLARATION - CONSTANTE
// Declaration Const --> Bouton
//int buttonpin = 2;
//int buttonpin = D8;

// Declaration variables
unsigned long tempoMesure = 0;

// Déclaration constantes
int TypeApp = 2; // déclaration du type Appareil (2) --> utilisé dans les requête d'insertion en Base pour distinction de chaque Module

// FIN SECTION DECLARATION - CONSTANTE

void setup() {
  Serial.begin(115200);

  // DEBUT SECTION SETUP - CONNEXION
  Serial.println();

  WiFi.begin(ssid,pass);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  // End WiFi section

  Serial.println("Connecting to");
  Serial.println(server_addr);
  if (conn.connect(server_addr, 3306, user, password)) {
    delay(1000);
    //Serial.print(".");
  }
  else
    Serial.println("Connection failed.");
  //conn.close();

  // create MySQL cursor object
  cursor = new MySQL_Cursor(&conn);

  // FIN SECTION SETUP - CONNEXION

  // set up the LCD's number of columns and rows:
  //lcd.begin(16, 2);
  
  // Print a message to the LCD.
  //lcd.print("Hello, World!");
  //lcd.noDisplay();

  // initialisation bouton = input
  //pinMode(buttonpin, INPUT_PULLUP); 

  // initialisation alimentation retro-eclairage LCD sur la PIN 6
  //pinMode(7,OUTPUT);

  setup_mqtt();
  client.publish("esp/test", "Hello from ESP8266");

}

void loop() {
  reconnect();
  client.loop(); 
  // Prise de Mesure - TOUTES LES 10 secondes
  if((millis() - tempoMesure) >= 900000){
    // start working...
    Serial.println("=================================");
    Serial.println("Sample DHT11...");
    
    // read without samples.
    byte temperature = 0;
    byte humidity = 0;
    int err = SimpleDHTErrSuccess;
    if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
      Serial.print("Read DHT11 failed, err="); 
      Serial.print(SimpleDHTErrCode(err));
      Serial.print(","); 
      Serial.println(SimpleDHTErrDuration(err)); 
      delay(1000);
      return;
    }
    
    // Affichage des Données sur le Serial
    Serial.print("Sample OK: ");
    Serial.print((int)temperature); 
    Serial.print(" *C, "); 
    Serial.print((int)humidity); 
    Serial.println(" H");
  

    //Serial.println(digitalRead(buttonpin));
    
    /*
    if (digitalRead(buttonpin) == LOW)
    {
      Serial.println(digitalRead(buttonpin)); // Affichage du Statut Bouton sur le Serial
      digitalWrite(6, HIGH); // Allumage Retro-Eclairage LCD
      lcd.display(); // Activation Display LCD
      // set the cursor to column 0, line 1
      // (note: line 1 is the second row, since counting begins with 0):
      lcd.setCursor(0, 0);
      // print the number of seconds since reset:
      //lcd.print(millis() / 1000);
      lcd.print("Temperature = ");
      lcd.print(temperature);
      lcd.setCursor(1,1);
      lcd.print("Humidity ");
      lcd.setCursor(12,1);
      lcd.print("=");
      lcd.setCursor(14,1);
      lcd.print(humidity);
    }
    else {
      //lcd.clear();
      lcd.noDisplay(); // Desactivation Display LCD
      digitalWrite(6, LOW); // Eteindre Retro-Eclairage LCD
    }
    */
    mqtt_publish("esp/temperature",temperature);
    mqtt_publish("esp/humidite",humidity);

    if (conn.connected()) {
     //cursor->execute(INSERT_SQL);
      insertTemperature(temperature); // Appel de la fonction d'insert Mesure Température en Base
      insertHumidity(humidity); // Appel de la fonction d'insert Mesure Humidité en Base
    }

    tempoMesure = millis();
    // DHT11 sampling rate is 1HZ.
    //delay(10000);
  }
  
}

// Déclaration de la fonction d'insert Mesure Température en Base
void insertTemperature(int temperature) {
  String insertvaluestemperature = "INSERT INTO station_meteo.st_mes_temperature (id_mesure_temp, date_mesure, mesure, it_app_module) VALUES (NULL, current_timestamp()," + String(temperature) +"," + String(TypeApp) + ");";
  int str_len_temp = insertvaluestemperature.length() + 1; // calcul de la taille du buffer du tableau Query
  char query_temp[str_len_temp]; // Déclaration du Tableau Query 
  insertvaluestemperature.toCharArray(query_temp,str_len_temp); // Transformation Requête String --> Char
  Serial.println(insertvaluestemperature);
  cursor->execute(query_temp); // Exécution de la Requête SQL
  Serial.println("Insertion OK");
  //delete cursor;
}

// Déclaration de la fonction d'insert Mesure Humidité en Base
void insertHumidity(int humidity) {
  String insertvalueshumidite = "INSERT INTO station_meteo.st_mes_humidite (id_mesure_hum, date_mesure, mesure, it_app_module) VALUES (NULL, current_timestamp()," + String(humidity) +"," + String(TypeApp) + ");";
  int str_len_hum = insertvalueshumidite.length() + 1; // calcul de la taille du buffer du tableau Query
  char query_hum[str_len_hum]; // Déclaration du Tableau Query 
  insertvalueshumidite.toCharArray(query_hum,str_len_hum); // Transformation Requête String --> Char
  Serial.println(insertvalueshumidite);
  cursor->execute(query_hum); // Exécution de la Requête SQL
  Serial.println("Insertion OK");
  //delete cursor;
}

void setup_mqtt() {
  client.setServer(MQTT_BROKER, MQTT_BROKER_PORT);
  reconnect();
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Connection au serveur MQTT ...");
    if (client.connect("ESPClient", MQTT_USERNAME, MQTT_KEY)) {
      Serial.println("MQTT connecté");
    }
    else {
      Serial.print("echec, code erreur= ");
      Serial.println(client.state());
      Serial.println("nouvel essai dans 2s");
    delay(2000);
    }
  }
}

//Fonction pour publier un float sur un topic
void mqtt_publish(String topic, float t) {
  char top[topic.length()+1];
  topic.toCharArray(top,topic.length()+1);
  char t_char[50];
  String t_str = String(t);
  t_str.toCharArray(t_char, t_str.length() + 1);
  client.publish(top,t_char);
}


