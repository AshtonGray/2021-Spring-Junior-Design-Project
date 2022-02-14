
/*  Ashton Gray
 *  32000589
 *  ECE 304 Junior Design Project
 *  Covid Sensor Sketch
 */

// ------- Libraries -------
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> // OLED Display
#include <Adafruit_I2CDevice.h> // temp sensor

// ------- Initializations -------
Adafruit_SSD1306 display(128,32, &Wire, 4);

// ------- Variables -------

// Define pins
#define BUTTONNO 13
#define BUTTONYES 12
#define BUTTONSTART 11
#define REDLED 10
#define YELLOWLED 9
#define GREENLED 8
#define TRIG 7
#define ECHO 6
#define MOTOR 5

// Temp
int tempDig;
float tempC;
float tempF;

// Sonic Sensor
float duration, distance;

// Counter for people
unsigned int counter;
unsigned int maxPeople;

// ------------ Functions -----------

// Check if Entering
int entering(int counter){
  // Maximum Capacity
  if (counter == maxPeople){
    display.clearDisplay();
    display.setCursor(1,1);
    display.print("Sorry,");
    display.println("");
    display.println("Maximum Capacity");
    display.println("");
    display.println("Please Wait");
    display.display();
    delay(5000); // wait 5 seconds
    return counter;
  }
  // intro
  display.clearDisplay();
  display.setCursor(1,1);
  display.print("Please answer the");
  display.println("");
  display.println("following questions.");
  display.display();
  delay(3000); // wait 3 seconds
  display.clearDisplay();
  display.setCursor(1,1);
  display.print("In the last 14 days,");
  display.println("");
  display.println("have you experienced");
  display.println("any of the following:");
  display.display();
  delay(3000); // wait 3 seconds
  // Question 1
  while(true){
    display.clearDisplay();
    display.setCursor(1,1);
    display.print("Loss of Smell/Taste?");
    display.println("");
    display.println("Muscle Aches?");
    display.println("Sore Throat / Cough?");
    display.display();
    if (digitalRead(BUTTONYES) == LOW){
      noEntry();
      return counter;
    }
    else if (digitalRead(BUTTONNO) == LOW)
      break;
  }
  // Question 2
  delay(1000); // wait 1 second
  while(true){
    display.clearDisplay();
    display.setCursor(1,1);
    display.print("Shortness of Breath?");
    display.println("");
    display.println("Chills / Headache?");
    display.println("Nausea or vomiting?");
    display.display();
    if (digitalRead(BUTTONYES) == LOW){
      noEntry();
      return counter;
    }
    else if (digitalRead(BUTTONNO) == LOW)
      break;
  }
  // Question 3
  delay(1000); // wait 1 second
  while(true){
    display.clearDisplay();
    display.setCursor(1,1);
    display.print("Have you been in");
    display.println("");
    display.println("close contact with");
    display.println("someone w/ Covid-19?");
    display.display();
    if (digitalRead(BUTTONYES) == LOW){
      noEntry();
      return counter;
    }
    else if (digitalRead(BUTTONNO) == LOW)
      break;
  }
  // Question 4
  delay(1000); // wait 1 second
  while(true){
    display.clearDisplay();
    display.setCursor(1,1);
    display.print("Have you been asked");
    display.println("");
    display.println("to quarantine by a");
    display.println("medical professional?");
    display.display();
    if (digitalRead(BUTTONYES) == LOW){
      noEntry();
      return counter;
    }
    else if (digitalRead(BUTTONNO) == LOW)
      break;
  }
  // Take Temperature
  display.clearDisplay();
  display.setCursor(1,1);
  display.print("Please Take Your");
  display.println("");
  display.println("Temperature:");
  display.display();
  while(tempF < 65.0)
    tempF = takeTemp();
    
  tempF = 0; // reset temperature so doesn't save on next person
  display.clearDisplay();
  display.setCursor(1,1);
  display.print("You may enter");
  display.display();
  delay(2000); // wait 5 seconds
  openDoor();
  counter += 1; // person walks through door
  return counter;
}

// Can't enter message
void noEntry(){
  display.clearDisplay();
  display.setCursor(1,1);
  display.print("You may not enter.");
  display.display();
  delay(5000); // wait 5 seconds
}

// Check Exiting
int exiting(int counter){
  if (counter > 0){ // only if there are people inside
    digitalWrite(TRIG, LOW);
    delayMicroseconds(5);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);
    duration = pulseIn(ECHO, HIGH);
    distance = ((duration/2.0) / 74.0) / 12.0; // distance in ft
    delay(750);
    // Open door if detected someone
    if (distance < 4){
      openDoor();
      counter -= 1;
    }
  }
  return counter;  
}

// Take Temperature
float takeTemp(){
  tempDig = analogRead(A0);
  tempC = (((tempDig * 5.0) / 1024.0) - 0.5) * 100;
  tempF = (tempC * 9.0 / 5.0) + 32.0;
  //Serial.println(tempF); print temp if needed
  return tempF;
}

// Open / Close door
void openDoor(){
  // unlock
  digitalWrite(MOTOR, HIGH);
  delay(2000);
  digitalWrite(MOTOR, LOW);
  // walk through
  delay(8000);
  // lock
  digitalWrite(MOTOR, HIGH);
  delay(2000);
  digitalWrite(MOTOR, LOW);
}

void occupancyLED(int counter){
  if (counter < 0.4 * maxPeople){
    digitalWrite(GREENLED, HIGH);
    digitalWrite(YELLOWLED, LOW);
    digitalWrite(REDLED, LOW);
  }
  else if (counter < 0.8 * maxPeople){
    digitalWrite(GREENLED, LOW);
    digitalWrite(YELLOWLED, HIGH);
    digitalWrite(REDLED, LOW);
  }
  else{
    digitalWrite(GREENLED, LOW);
    digitalWrite(YELLOWLED, LOW);
    digitalWrite(REDLED, HIGH);
  }
}

// Change maxPeople
unsigned int changeMax(unsigned int maxPeople){
  while(true){
    display.clearDisplay();
    display.setCursor(1,1);
    display.print("Current Maximum");
    display.println("");
    display.println("Capacity: ");
    display.println(maxPeople);
    display.display();
    delay(500);
    if (digitalRead(BUTTONSTART) == LOW)
      return maxPeople;
    else if (digitalRead(BUTTONYES) == LOW)
      maxPeople += 5;
    else if (digitalRead(BUTTONNO) == LOW){
      if (maxPeople != 5)
        maxPeople -= 5;
    }
  }
}


void setup() {
  // Serial output visual
  Serial.begin(9600);

  // OLED Display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  // INPUT / OUTPUT
  pinMode(BUTTONNO, INPUT);
  pinMode(BUTTONYES, INPUT);
  pinMode(BUTTONSTART, INPUT);
  pinMode(REDLED, OUTPUT);
  pinMode(YELLOWLED, OUTPUT);
  pinMode(GREENLED, OUTPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO,INPUT);
  pinMode(MOTOR, OUTPUT);

  // HIGH / LOW
  digitalWrite(BUTTONNO, HIGH);
  digitalWrite(BUTTONYES, HIGH);
  digitalWrite(BUTTONSTART, HIGH);

  maxPeople = 5;
}

void loop() {
  // Check person trying to exit (Sonar)
  counter = exiting(counter);

  // Check if person trying to enter (button pressed)
  if (digitalRead(BUTTONSTART) == LOW){
    counter = entering(counter);
    display.clearDisplay();
    display.display();
  }

  // LEDs
  occupancyLED(counter);
  
  // Change Maximum People
  if (digitalRead(BUTTONYES) == LOW && digitalRead(BUTTONNO) == LOW){
    maxPeople = changeMax(maxPeople);
    display.clearDisplay();
    display.display();
    delay(500);
  }
  
}
