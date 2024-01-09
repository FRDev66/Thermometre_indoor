#include <SimpleDHT.h>
#include <LiquidCrystal.h>
#include <ESP8266WiFi.h>  
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

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

WiFiClient client;            // Use this for WiFi instead of EthernetClient
MySQL_Connection conn((Client *)&client);
MySQL_Cursor* cursor;

// FIN SECTION DECLARATION - CONNEXION

// DEBUT SECTION DECLARATION - MESURES
// for DHT11, 
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2
int pinDHT11 = 13;
SimpleDHT11 dht11(pinDHT11);

// FIN SECTION DECLARATION - MESURES

// DEBUT SECTION DECLARATION - ECRAN LCD
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// FIN SECTION DECLARATION - ECRAN LCD 

// DEBUT SECTION DECLARATION - CONSTANTE
// Declaration Const --> Bouton
int buttonpin = 2;

// Declaration variables
unsigned long tempoMesure = 0;

// Déclaration constantes
int TypeApp = 2; // déclaration du type Appareil (2) --> utilisé dans les requête d'insertion en Base pour distinction de chaque Module

// FIN SECTION DECLARATION - CONSTANTE

// EXEMPLE REQUETE SQL
// Sample query
char INSERT_SQL[] = "INSERT INTO station_meteo.temperature (id_mesure_temp, date_mesure, mesure) VALUES (NULL, current_timestamp(), '15.8');";


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
  lcd.begin(16, 2);
  // Print a message to the LCD.
  //lcd.print("Hello, World!");
  //lcd.noDisplay();

  // initialisation bouton = input
  pinMode(buttonpin, INPUT_PULLUP); 

  // initialisation alimentation retro-eclairage LCD sur la PIN 6
  pinMode(6,OUTPUT);
}

void loop() {
  
  // Prise de Mesure - TOUTES LES 10 secondes
  if((millis() - tempoMesure) >= 10000){
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

    if (conn.connected())
      cursor->execute(INSERT_SQL);
  

    tempoMesure = millis();
    // DHT11 sampling rate is 1HZ.
    //delay(10000);
  }
  
}


