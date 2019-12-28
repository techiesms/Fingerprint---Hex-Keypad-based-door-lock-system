#include <Adafruit_Fingerprint.h> //https://github.com/adafruit/Adafruit-Fingerprint-Sensor-Library
#include <Adafruit_GFX.h> //https://github.com/adafruit/Adafruit-GFX-Library
#include <Adafruit_SSD1306.h> //https://github.com/adafruit/Adafruit_SSD1306
#include <Keypad.h> //https://www.arduinolibraries.info/libraries/keypad
#include <HardwareSerial.h>
#include <WiFi.h>
#include <Wire.h>

//Keypad Configurations
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {13, 12, 14, 27}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {26, 25, 33, 32}; //connect to the column pinouts of the keypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);


void beep(int duration);

//Display Configurations
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

String ID_e;
char customKey;
int value;
bool done_flag = 0;
unsigned int id;
int fingerprint_flag = 1;
//initialize an instance of class NewKeypad


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial2);

unsigned int getFingerprintEnroll();
unsigned int readnumber(void);
unsigned int getFingerprintID();
int Keypad_Input(void);
void state(void);


void setup_e()
{
  Serial.begin(57600);
  Serial2.begin(115200);
  while (!Serial);
  delay(100);
  Serial.println("\n\nAdafruit Fingerprint sensor enrollment");

  // set the data rate for the sensor Serial port
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    int i;
    while (i < 7) {
      delay(10);
      Serial.print(".!E!.");
      i++;
    }
  }
}


void loop_e()                     // run over and over again
{
  Serial.println("Ready to enroll a fingerprint!");
  Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
  id = readnumber();
  if (id == 0) {// ID #0 not allowed, try again!
    return;
  }
  Serial.print("Enrolling ID #");
  //  Serial.println(id);

  while (1) {
    getFingerprintEnroll();
    break;
  }
}


unsigned int readnumber()
{
  Serial.println("Reading Number");
  int  i = 0;
  ID_e = "";
  value = 0;
  while (1)
  {
    customKey = customKeypad.getKey();

    if (customKey && i < 3)
    {
      ID_e = ID_e + customKey;
      value = ID_e.toInt();
      Serial.println(value);
      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(50, 50);            // Start at top-left corner
      display.println(value);
      display.display();

      beep(100);
      i++;
    }
    if (customKey == '#')
    {
      beep(100);
      return (value);
    }
    if (customKey == 'B')
    {
      fingerprint_flag = 1;
      ID_e = "";
      i = 0;
      beep(100);
      return (value);
    }

  }
}



unsigned int getFingerprintEnroll() 
{

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #");
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(20, 20);            // Start at top-left corner
  display.println(("Place"));
  display.setCursor(15, 40);            // Start at top-left corner
  display.println(("Finger"));
  display.display();
  Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
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

  Serial.println("Remove finger");
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(18, 20);            // Start at top-left corner
  display.println(("Remove"));
  display.setCursor(15, 40);            // Start at top-left corner
  display.println(("Finger"));
  display.display();
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(30, 0);            // Start at top-left corner
  display.println(("Place \n   same \n  Finger"));
  display.display();
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
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
  Serial.print("Creating model for #");

  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
    display.clearDisplay();
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(15, 20);            // Start at top-left corner
    display.println(("Prints \nmatched"));
    display.display();
    beep(2000);
    //delay(1000);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    display.clearDisplay();
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(15, 0);            // Start at top-left corner
    display.println(("  Prints \n  did not \n  matched"));
    display.display();
    beep(200);
    beep(200);
    delay(500);
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    display.clearDisplay();
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0, 0);            // Start at top-left corner
    display.println(("  Print \n stored to ID  "));
    display.println((id));
    display.display();
    delay(1000);
    //    return;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

}

int Keypad_Input(void)
{
  int i = 0;
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(30, 0);            // Start at top-left corner
  display.println(("Enter"));
  display.setCursor(35, 20);            // Start at top-left corner
  display.println(("Code"));
  display.display();
  i = 0;
  ID_e = "";
  value = 0;
  while (1)
  {
    customKey = customKeypad.getKey();

    if (customKey && i < 4)
    {
      ID_e = ID_e + customKey;
      value = ID_e.toInt();
      Serial.println(value);
      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(i * 35, 50);            // Start at top-left corner
      display.println(("*"));
      display.display();
      beep(100);
      i++;
    }
    if (customKey == '#')
    {
      done_flag = 1;
      Serial.println("DONE");
      i = 0;
      ID_e = "";
      return (value);
    }
    if (customKey == 'B')
    {
      fingerprint_flag = 1;
      ID_e = "";
      return (0);
    }
  }
}
