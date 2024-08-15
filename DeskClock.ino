// Kerem DÖLEKSÖZ

// Libraries used for various functionalities
#include <LiquidCrystal.h>  // Library for controlling LCD displays
#include <dht11.h>           // Library for DHT11 temperature and humidity sensor
#include <DS3231.h>          // Library for DS3231 real-time clock
#include <Wire.h>            // Library for communication

// Initialize a LiquidCrystal object for controlling the LCD display
LiquidCrystal lcd(8, 7, 6, 5, 4, 3);  // Pins used for the LCD display

// Define the pin to which the DHT11 sensor is connected
#define DHT11PIN 11
const int buttonPin = 2;  // Pin used for the button

dht11 DHT11;    // Create an instance of the dht11 class for DHT11 sensor
DS3231 myRTC;   // Create an instance of the DS3231 class for RTC (Real-Time Clock)

// Variables for clock formatting
bool h12Flag;   // Flag for 12-hour clock format
bool pmFlag;    // Flag for AM/PM
bool century = false; // Flag for century format

int lastSeconds = 0;    // Store the last recorded seconds
volatile int counter = 0;   // Counter for button presses
int lastCounter = 0;    // Store the last counted button press
volatile unsigned long millisTemp;  // Store current millis() value
volatile unsigned long millisAnchor = 0;  // Store the last millis() value for button debounce

// Setup function - runs once when the device starts up
void setup() {
  Serial.begin(9600);   // Initialize serial communication
  lcd.begin(16, 2);     // Initialize the LCD display
  Wire.begin();         // Initialize the communication
  pinMode(buttonPin, INPUT);  // Set the button pin as input
  attachInterrupt(digitalPinToInterrupt(buttonPin), button, RISING); // Attach interrupt for button press
  myRTC.setMonth(2);    // Set the month for RTC
  myRTC.setClockMode(false);  // Set clock mode for RTC
}

// Loop function - runs continuously after setup
void loop() {
  millisTemp = millis();  // Get current millis() value

  // Check if the button counter has changed
  if (lastCounter != counter) {
    lcd.clear();  // Clear the LCD display
    lastCounter = counter;  // Update the last counter value
  }

  // Determine the action based on the value of counter
  int div = counter % 3;
  if (div == 1) {
    timeExtended();  // Display extended time information
  } else if (div == 2) {
    temperatureExtended();  // Display extended temperature information
  } else if (div == 0) {
    temperatureAndtime();   // Display both temperature and time
  }
}

// Function to display temperature and time on the LCD
void temperatureAndtime() {
  int seconds = myRTC.getSecond();  // Get seconds from RTC
  int hours = myRTC.getHour(h12Flag, pmFlag);  // Get hours from RTC
  int minutes = myRTC.getMinute();  // Get minutes from RTC

  // Set the cursor to the top left corner of the LCD
  lcd.home();

  // Read data from the DHT11 sensor
  int chk = DHT11.read(DHT11PIN);

  // Display temperature
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.setCursor(2, 0);
  lcd.print((double)DHT11.temperature, 1);
  lcd.setCursor(6, 0);
  lcd.print("C");

  // Display humidity
  lcd.setCursor(9, 0);
  lcd.print("H:");
  lcd.setCursor(11, 0);
  lcd.print((float)DHT11.humidity, 1);
  lcd.setCursor(15, 0);
  lcd.print("%");

  // If seconds changed, display time
  if (seconds != lastSeconds) {
    lcd.setCursor(0, 1);
    lcd.print("Time:");
    lcd.setCursor(6, 1);
    if (hours < 10) {
      lcd.print("0");
    }
    lcd.print(hours, DEC);
    lcd.print(":");
    lcd.setCursor(9, 1);
    if (minutes < 10) {
      lcd.print("0");
    }
    lcd.print(minutes, DEC);
    lcd.print(":");
    lcd.setCursor(12, 1);
    if (seconds < 10) {
      lcd.print("0");
    }
    lcd.print(seconds, DEC);
    lastSeconds = seconds;  // Update last seconds
  }
}

// Function to display extended temperature information on the LCD
void temperatureExtended() {
  lcd.home();

  // Read data from the DHT11 sensor
  int chk = DHT11.read(DHT11PIN);

  // Display temperature
  lcd.setCursor(0, 0);
  lcd.print("Tmprture:");
  lcd.setCursor(10, 0);
  lcd.print((double)DHT11.temperature, 1);
  lcd.setCursor(15, 0);
  lcd.print("C");

  // Display humidity
  lcd.setCursor(0, 1);
  lcd.print("Humidity:");
  lcd.setCursor(10, 1);
  lcd.print((float)DHT11.humidity, 1);
  lcd.setCursor(15, 1);
  lcd.print("%");
}

// Function to display extended time information on the LCD
void timeExtended() {
  int seconds = myRTC.getSecond();  // Get seconds from RTC
  int hours = myRTC.getHour(h12Flag, pmFlag);  // Get hours from RTC
  int minutes = myRTC.getMinute();  // Get minutes from RTC

  // If seconds changed, display time
  if (seconds != lastSeconds) {
    lcd.setCursor(0, 1);
    lcd.print("Time:");
    lcd.setCursor(6, 1);
    if (hours < 10) {
      lcd.print("0");
    }
    lcd.print(hours, DEC);
    lcd.print(":");
    lcd.setCursor(9, 1);
    if (minutes < 10) {
      lcd.print("0");
    }
    lcd.print(minutes, DEC);
    lcd.print(":");
    lcd.setCursor(12, 1);
    if (seconds < 10) {
      lcd.print("0");
    }
    lcd.print(seconds, DEC);
    lastSeconds = seconds;  // Update last seconds
  }

  // Display date
  lcd.setCursor(0, 0);
  lcd.print("Date: ");
  lcd.print(myRTC.getDate());
  lcd.print("/");
  lcd.setCursor(9, 0);
  lcd.print(myRTC.getMonth(century), DEC);
  lcd.print("/");
  lcd.setCursor(11, 0);
  lcd.print("20");
  lcd.print(myRTC.getYear());
  lcd.setCursor(16, 0);
  lcd.print(" ");
}

// Interrupt service routine for button press
void button() {
  // Button debounce - increase counter if debounce time has passed
  if ((millisTemp - millisAnchor) >= 300) {
    counter++;  // Increase counter
    millisAnchor = millisTemp;  // Update millisAnchor
  }
}
