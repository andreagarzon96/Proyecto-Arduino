/**
 * Esquema: 
 * 
 * Botones: conectados a tierra con los pines 3, 2, 0, 1, 7
 * LCD: Via I2C (SDA, SCL)
 */

#include <SPI.h>

// For I2C LCD: https://bitbucket.org/fmalpartida/new-liquidcrystal/overview
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <Bridge.h>
#include <BridgeClient.h>
#include <HttpClient.h>

#define LCD_I2C_ADDR 0x3F


const char *arduino_id = "YUN0"; // Variar al momento de carga en cada Arduino
const char *server = "192.168.0.100"; // Modificar al momento de configuracion de red
char buffer[64];

BridgeClient bClient;
HttpClient hClient;
LiquidCrystal_I2C lcd(LCD_I2C_ADDR, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

// Usage of interrupts: http://www.thearduinomakerman.info/blog/2016/4/25/using-interrupts
int buttonNum[5] = {3, 2, 0, 1, 7};
volatile int buttonSelect = 0;
volatile int buttonFlag = 0;

int q_index = 0;


void setup() {
  // put your setup code here, to run once:
  Bridge.begin();
  Serial.begin(9600);
  
  for (int x = 0; x < 5; x++)
  {
    pinMode(buttonNum[x], INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(buttonNum[x]), buttonPressed, CHANGE);
  }

  lcd.begin(20, 4);
  lcd.backlight();
  lcd.home();
  lcd.print("Loading...");
  delay(1000);
}


void loop() {
  // put your main code here, to run repeatedly:

  // Get and show the next question
  nextQuestion(q_index);
  
  // Wait for interrupt
  while (buttonFlag == 0)
  {
    delay(10);
  }
  
  // DEBUG BEGIN
  Serial.print("Button ");
  Serial.print(buttonSelect);
  Serial.println(" pressed.");
  // DEBUG END

  // Send answer to server
  switch (buttonSelect)
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

  // Wait for button release
  int pressed = digitalRead(buttonSelect);
  while (pressed == 0)
  {
    pressed = digitalRead(buttonSelect);
  }
  buttonFlag = 0;
}

// Interrupt handler: identifies button that has been pressed
void buttonPressed()
{
  buttonFlag = 1;
  for (int x = 0; x < 5; x++)
  {
    int button = digitalRead(x);
    if (button == LOW)
    {
      buttonSelect = x;
      q_index = q_index + 1;
      break;
    }
  }
}

// Gets next question and show in LCD
String nextQuestion(int id)
{
  String url = String(server) + "/proyecto_iot/question.php?id=" + String(id);
  Serial.print(url); // DEBUG
  hClient.get(url);
  String question = "";

  while (hClient.available())
  {
    char c = hClient.read();
    Serial.print(c);
    question = question + c;
  }
  Serial.flush();

  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(question);
}

// Sends answer selected to server
void answer(int n)
{
  // Send answer to server using JSON: https://www.youtube.com/watch?v=4QYt2Yqo5TA
  if (bClient.connect(server, 80))
  {
    Serial.println("Connected");

    // POST URI
    sprintf(buffer, "POST /proyecto_iot/receiver.php HTTP/1.1");

    Serial.println(buffer);
    bClient.println(buffer);

    // Header
    sprintf(buffer, "Host: %s", server);
    Serial.println(buffer);
    bClient.println(buffer);

    // JSON content type
    Serial.println("Content-Type: application/json");
    bClient.println("Content-Type: application/json");

    // POST Body
    sprintf(buffer, "{ \"ID\" : \"%s\", \"answer\" : \" %i \" }", arduino_id, n);

    // Content length
    Serial.print("Content-Length: ");
    bClient.print("Content-Length: ");

    Serial.println(strlen(buffer));
    bClient.println(strlen(buffer));

    // END Header
    Serial.println();
    bClient.println();

    // Request body
    Serial.println(buffer);
    bClient.println(buffer);
  } else {
    Serial.println("Connection failed");
  }
}

