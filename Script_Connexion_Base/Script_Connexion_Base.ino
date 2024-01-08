#include <ESP8266WiFi.h>  
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress server_addr(192,168,1,61);  // IP of the MySQL *server* here
char user[] = "arduino";              // MySQL user login username
char password[] = "test";        // MySQL user login password

// WiFi card example
char ssid[] = "Livebox-1F90";    // your SSID
char pass[] = "o3jwTuDzadcmQAtZ2r";       // your SSID Password

WiFiClient client;            // Use this for WiFi instead of EthernetClient
MySQL_Connection conn((Client *)&client);
MySQL_Cursor* cursor;

// Sample query
char INSERT_SQL[] = "INSERT INTO station_meteo.temperature (id_mesure_temp, date_mesure, mesure) VALUES (NULL, current_timestamp(), '15.8');";

void setup() {
  Serial.begin(115200);
  //while (!Serial); // wait for serial port to connect. Needed for Leonardo only

  // Begin WiFi section

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
}

void loop() {
  if (conn.connected())
    cursor->execute(INSERT_SQL);

  delay(5000);
}
