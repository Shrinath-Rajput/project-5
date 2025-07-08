#define BLYNK_TEMPLATE_ID "TMPL3NTEGh4bq"
#define BLYNK_TEMPLATE_NAME "iot based fuel tank"
#define BLYNK_AUTH_TOKEN "mTO4lWVpvfh4lka-T4iUzTtXcje6PSo2"

#include <Keypad.h>
#include <Preferences.h>
#include <ESP32Servo.h>
#include <SoftwareSerial.h>
#include <Ultrasonic.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// Define Blynk credentials
char auth[] = "mTO4lWVpvfh4lka-T4iUzTtXcje6PSo2";

// DHT11 sensor parameters
#define DHTPIN 15
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Define keypad, preferences, and servo objects
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {23, 22, 3, 21};
byte colPins[COLS] = {19, 18, 5, 4};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
Preferences preferences;
Servo lockServo;

// Password related variables
bool passwordSet = false;
String newPassword = "";
String savedPassword = "**"; 

// Servo motor control variables
const int servoPin = 12;
const int unlockAngle = 0;
const int lockAngle = 90;

// GSM module connection
SoftwareSerial gsmSerial(17, 16);

// Buzzer pin
const int buzzerPin = 25;

// Water flow sensor
const int flowSensorPin = 2; // Pin for flow sensor
volatile int pulseCount = 0; // Count of flow pulses
const float pulsesPerLiter = 2000; // Adjust according to your flow sensor
float totalFuelFlowLiters = 0.0; // Total fuel flow in liters

// Ultrasonic sensor for fuel level measurement
#define TRIGGER_PIN 13
#define ECHO_PIN 14
Ultrasonic ultrasonic(TRIGGER_PIN, ECHO_PIN);

// Time interval between measurements (10 seconds for testing)
const unsigned long measurementInterval = 1000;
unsigned long lastMeasurementTime = 0;

// Track incorrect password attempts
int incorrectAttempts = 0;
const int maxAttempts = 3; // Number of attempts before sending multiple SMS

// Function prototypes
void setPassword();
bool verifyPassword(String input);
void sendSMS(const String& phoneNumber, const String& message);
void soundBuzzer();
int measureTankLevel();
int measureFuelLevel();
float measureTotalFuelFlow();
void countPulse(); // Interrupt function prototype

void setup() {
  Serial.begin(115200);
  gsmSerial.begin(9600);
  Blynk.begin(auth, "network", "1122334455");
  pinMode(buzzerPin, OUTPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(flowSensorPin, INPUT_PULLUP); // Use internal pull-up resistor
  
  dht.begin();
  lockServo.attach(servoPin);
  lockServo.write(lockAngle); 
  
  preferences.begin("tank-lock", false);
  
  if (preferences.getString("password", "") != "") {
    savedPassword = preferences.getString("password", "");
    passwordSet = true;
  } else {
    setPassword();
  }
  
  preferences.end();

  attachInterrupt(digitalPinToInterrupt(flowSensorPin), countPulse, RISING);
}

void loop() {
  Blynk.run();
  unsigned long currentMillis = millis();
  
  if (currentMillis - lastMeasurementTime >= measurementInterval) {
    lastMeasurementTime = currentMillis;
    
    int tankLevel = measureTankLevel();
    Serial.print("Tank Level (cm): ");
    Serial.println(tankLevel);
    
    int fuelLevel = measureFuelLevel();
    Serial.print("Fuel Level (%): ");
    Serial.println(fuelLevel);
    Blynk.virtualWrite(V1, fuelLevel); // Send fuel level to Blynk

    float totalFlow = measureTotalFuelFlow();
    Serial.print("Total Fuel Flow (liters): ");
    Serial.println(totalFlow);
    Blynk.virtualWrite(V2, totalFlow); // Send total fuel flow to Blynk

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read from DHT sensor!");
    } else {
      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.println(" °C");
      Serial.print("Humidity: ");
      Serial.print(humidity);
      Serial.println(" %");

      Blynk.virtualWrite(V3, temperature);
      Blynk.virtualWrite(V4, humidity);
    }
  }

  char key = keypad.getKey();
  if (key != NO_KEY) {
    newPassword += key;
    Serial.print("Entered: ");
    Serial.println(newPassword);
    
    if (newPassword.length() == 4) {
      if (passwordSet) {
        if (verifyPassword(newPassword)) {
          Serial.println("Password correct. Unlocking tank...");
          unlockTank();
          sendSMS("+919325511681", "Tank unlocked.");
          delay(5000);
          lockTank();
          sendSMS("+919325511681", "Tank locked.");
        } else {
          Serial.println("Incorrect password. Please try again.");
          incorrectAttempts++;  // Increment incorrect attempt counter
          
          if (incorrectAttempts >= maxAttempts) {
            sendSMS("+919325511681", "Incorrect password entered 3 times!");
            sendSMS("+919325511681", "Please contact the admin.");
            incorrectAttempts = 0;  // Reset after sending SMS
          }
          
          soundBuzzer();
        }
      } else {
        preferences.begin("tank-lock", false);
        preferences.putString("password", newPassword);
        preferences.end();
        
        savedPassword = newPassword;
        passwordSet = true;
        
        Serial.println("Password set successfully!");
      }
      
      newPassword = "";
    }
  }
  
  delay(10);
}

void setPassword() {
  Serial.println("Enter new password (4 digits):");
  while (newPassword.length() < 4) {
    char key = keypad.getKey();
    if (key != NO_KEY) {
      newPassword += key;
      Serial.print("*");
    }
    delay(100);
  }
  Serial.println();
  
  preferences.begin("tank-lock", false);
  preferences.putString("password", newPassword);
  preferences.end();
  
  savedPassword = newPassword;
  passwordSet = true;
  
  Serial.println("Password set successfully!");
}

bool verifyPassword(String input) {
  return input.equals(savedPassword);
}

void unlockTank() {
  lockServo.write(unlockAngle);
  Serial.println("Tank unlocked.");
}

void lockTank() {
  lockServo.write(lockAngle);
  Serial.println("Tank locked.");
}

void sendSMS(const String& phoneNumber, const String& message) {
  gsmSerial.println("AT+CMGF=1");
  delay(1000);
  
  gsmSerial.print("AT+CMGS=\"");
  gsmSerial.print(phoneNumber);
  gsmSerial.println("\"");
  delay(1000);
  
  gsmSerial.println(message);
  delay(1000);
  
  gsmSerial.println((char)26);
  delay(1000);
  
  Serial.println("SMS sent");
}

void soundBuzzer() {
  digitalWrite(buzzerPin, HIGH);
  delay(1000);
  digitalWrite(buzzerPin, LOW);
}

int measureTankLevel() {
  return 50; // Replace with actual measurement code
}

int measureFuelLevel() {
  unsigned int distance = ultrasonic.read(CM);
  int tankHeight = 50; // Replace with actual tank height in cm
  int fuelLevel = map(distance, 0, tankHeight, 0, 100);
  return constrain(fuelLevel, 0, 100);
}

float measureTotalFuelFlow() {
  float flowInLiters = pulseCount / pulsesPerLiter; // Calculate flow in liters
  totalFuelFlowLiters += flowInLiters; // Increment total fuel flow
  pulseCount = 0; // Reset pulse count for the next measurement
  return totalFuelFlowLiters; // Return total flow in liters
}

void countPulse() {
  pulseCount++; // Increment pulse count
}