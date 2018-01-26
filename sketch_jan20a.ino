// For Yun connection to server
#include <Bridge.h>
#include <BridgeClient.h>
#include <SPI.h>

// For I2C LCD: https://bitbucket.org/fmalpartida/new-liquidcrystal/overview
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char *arduino_id = "YUN0"; // Value should be changed for different devices
const char *server = "192.168.0.100"; // Web server address

BridgeClient client;
char buffer[64];

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

// Usage of interrupts: http://www.thearduinomakerman.info/blog/2016/4/25/using-interrupts
int buttonNum[5] = {3, 2, 0, 1, 7};
int buttonSelect = 0;
int buttonFlag = 0;

void setup() 
{
  Bridge.begin();
  Serial.begin(9600); // For debugging purposes

  while (!Serial);

  for (int x = 0; x < 5; x++)
  {
    pinMode(buttonNum[x], INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(buttonNum[x]), buttonPressed, LOW);
  }

  lcd.begin(20, 4);
  lcd.backlight();
  lcd.home();
  lcd.print("Loading.");
  delay(125);
  lcd.print(".");
  delay(125);
  lcd.print(".");
  delay(250);
}

void loop() 
{
  // Using interrupts to identify the button pressed: https://www.youtube.com/watch?v=1wWT9QnN1YI
  while (buttonFlag == 0)
  {
    delay(10);
  }

  Serial.print("Button ");
  Serial.print(buttonSelect);
  Serial.print(" pressed.");
  switch(buttonSelect)
  {
    case 3:
      answer(1);
      break;
    case 2:
      answer(2);
      break;
    case 0:
      answer(3);
      break;
    case 1:
      answer(4);
      break;
    case 7:
      answer(5);
      break;
    default:
      break;
  }
  int pressed = digitalRead(buttonSelect);
  while (pressed == 0)
  {
    pressed = digitalRead(buttonSelect);
  }

  buttonFlag = 0;

  // Show server response
  while (client.available())
  {
    char c = client.read();
    Serial.print(c);
  }
  Serial.flush();
}


void buttonPressed()
{
  buttonFlag = 1;
  for (int x = 0; x < 5; x++)
  {
    int button = digitalRead(x);
    if (button == HIGH)
    {
      buttonSelect = x;
      break;
    }
  }
}


void answer(int option)
{
  // Send answer to server using JSON: https://www.youtube.com/watch?v=4QYt2Yqo5TA
  
  if (client.connect(server, 80))
  {
    Serial.println("Connected!");

    // POST URI
    sprintf(buffer, "POST /IoT/receiver.php HTTP/1.1");

    Serial.println(buffer);
    client.println(buffer);

    // Header
    sprintf(buffer, "Host: %s", server);
    Serial.println(buffer);
    client.println(buffer);

    // JSON content type
    Serial.println("Content-Type: application/json");
    client.println("Content-Type: application/json");

    // POST Body
    sprintf(buffer, "{ \"ID\" : \"%s\", \"answer\" : \" %i \" }", arduino_id, option);

    // Content length
    Serial.print("Content-Length: ");
    client.print("Content-Length: ");

    Serial.println(strlen(buffer));
    client.println(strlen(buffer));

    // END Header
    Serial.println();
    client.println();

    // Request body
    Serial.println(buffer);
    client.println(buffer);
  } else {
    Serial.println("Connection failed");
  }
}

