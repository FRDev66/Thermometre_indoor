#include <SimpleDHT.h>
#include <LiquidCrystal.h>

// for DHT11, 
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2
int pinDHT11 = 13;
SimpleDHT11 dht11(pinDHT11);

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// Declaration Const --> Bouton
int buttonpin = 2;

void setup() {
  Serial.begin(115200);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  //lcd.print("Hello, World!");
  //lcd.noDisplay();

  pinMode(buttonpin, INPUT_PULLUP);  
}

void loop() {
  // start working...
  Serial.println("=================================");
  Serial.println("Sample DHT11...");
  
  // read without samples.
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.print(SimpleDHTErrCode(err));
    Serial.print(","); Serial.println(SimpleDHTErrDuration(err)); delay(1000);
    return;
  }
  
  Serial.print("Sample OK: ");
  Serial.print((int)temperature); Serial.print(" *C, "); 
  Serial.print((int)humidity); Serial.println(" H");

  Serial.println(digitalRead(buttonpin));
  
  if (digitalRead(buttonpin) == LOW)
  {
    
    Serial.println(digitalRead(buttonpin));
  
    //lcd.display();
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

    //lcd.clear();
  }
  else {
    lcd.clear();
    //lcd.noDisplay();
  }

  // DHT11 sampling rate is 1HZ.
  delay(10000);

  
}

