//final
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <Keypad.h>
#include <LiquidCrystal.h>
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
int key1[] = {1, 2, 3, 4, 5};
int key2[] = {3, 2, 3, 4, 5};
int keyPrs[] = {0, 0, 0, 0, 0};
int count = 0;
int relayPin1 = 4;
int relayPin2 = 5;

boolean m1 = false;
boolean m2 = false;
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);//lcd RS Enable D4 D5 D6 D7

byte rowPins[ROWS] = {22, 23, 24, 25}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {26, 27, 28}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

int getFingerprintIDez();
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial1);//green rx1 % yolow tx1 


void setup()  
{
  lcd.begin(16, 2);
  start();
  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  
  Serial.begin(9600);
  Serial.println("Adafruit finger detect test");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1);
  }
  Serial.println("Waiting for valid finger...");
}

void loop()                     // run over and over again
{
  getFingerprintIDez();
  
  delay(50);            //don't ned to run this at full speed.
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence); 
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  pasInput();
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  if(finger.fingerID==1)
  {
  ldSet1();
  start();
  }
  
   if(finger.fingerID==2)
  {
  ldSet2();
  start();
  }
//  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID; 
}



void ldSet1()   //for id 1 control
{


  lcd.clear();
  lcd.setCursor(1, 0);
  if (!m1)
  {
    lcd.print("LD 1 started");
    digitalWrite(relayPin1, HIGH);
    delay(2000);
    m1=true;
  }
 
  else
  {
    lcd.print("LD 1 Stopted");
    digitalWrite(relayPin1, LOW);
    delay(2000);
    m1=false;
  }

}

void ldSet2()  // for id 2 control
{


  lcd.clear();
  lcd.setCursor(1, 0);
  if (!m2)
  {
    lcd.print("LD 2 started");
    digitalWrite(relayPin2, HIGH);
    delay(2000);
    m2=true;
  }
 
  else
  {
    lcd.print("LD 2 Stopted");
    digitalWrite(relayPin2, LOW);
    delay(2000);
    m2=false;
  }

}

boolean pasMatched1()  //matching password 1
{
  boolean gnt = true;
  for (int i = 0; i < 5; i++)
  {
    if (key1[i] != keyPrs[i])
    {
      gnt = false;
    }

  }
  return gnt;

}



boolean pasMatched2() //matching password 2
{
  boolean gnt = true;
  for (int i = 0; i < 5; i++)
  {
    if (key2[i] != keyPrs[i])
    {
      gnt = false;
    }

  }
  return gnt;

}

void pasInput()   //take password input
{
char k = keypad.getKey();

  if (k) {

    keyPrs[count] = k - 48;
    lcd.print("*");
    count++;

    if (count == 5)
    {
      count = 0;
      if (pasMatched1())
      {

        ldSet1();
        start();

      }

      else if (pasMatched2())
      {

        ldSet2();
        start();

      }
      
      else
      {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Access Denied");
        delay(2000);
        start();
      }
    }
  }

}

void start()   //starting display
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Password/finger:");
  lcd.setCursor(0, 1);
}
