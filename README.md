# project-5
Fuel Tank Monitoring System Using IOT
🚀 A smart IoT-based system to monitor fuel levels, control tank access, detect flow rates, and provide remote data monitoring and security using ESP32, Ultrasonic sensor, Servo motor, Keypad, DHT11, Flow sensor, Blynk, and GSM module.

📌 Key Features

🔐 Password-protected servo motor lock (set & verify via keypad)

📱 Remote monitoring via Blynk (Fuel level, Temperature, Humidity, Flow rate)

🌡️ DHT11 sensor for environmental monitoring (Temp & Humidity)

📏 Ultrasonic sensor to detect fuel tank level

💧 Flow sensor to measure fuel flow in liters

📲 GSM module sends SMS alerts for:

Successful unlock/lock

Multiple incorrect password attempts


🔊 Buzzer alert on failed authentication

🧠 EEPROM (Preferences library) stores password securely

🔧 Tech Stack

Component	Description

ESP32	Main microcontroller
Ultrasonic	Fuel level detection
Servo Motor	Tank lock/unlock mechanism
4x4 Keypad	Password input system
DHT11 Sensor	Temperature and humidity sensing
Flow Sensor	Measures total fuel dispensed in liters
Blynk IoT App	Remote data visualization (Level, Temp, Flow)
SIM800L	GSM SMS alerts
Buzzer	Alerts user for wrong attempts
MySQL (optional)	For backend if expanded further


📷 Live Data on Blynk Dashboard

V1: Fuel Level (%)

V2: Total Fuel Flow (Liters)

V3: Temperature (°C)

V4: Humidity (%)


🔐 Password and Security Flow

Set a 4-digit password on first boot

Unlock the tank by entering the correct password

3 incorrect attempts → SMS alert to admin

Servo opens/closes tank based on authentication


📲 SMS Alerts via GSM Module

✅ Tank unlocked

🔒 Tank locked

⚠️ 3 wrong attempts trigger emergency SMS


⚙️ Hardware Setup

Servo Motor → Pin 12

Ultrasonic Sensor → Trigger (13), Echo (14)

Keypad Rows → Pins 23, 22, 3, 21

Keypad Columns → Pins 19, 18, 5, 4

DHT11 Sensor → Pin 15

Buzzer → Pin 25

Flow Sensor → Pin 2

GSM (SIM800L) → RX (17), TX (16)

🧠 How It Works

1. System boots → checks password set in EEPROM
2. User enters 4-digit password via keypad
3. If correct:
   → Unlock tank (servo rotates)
   → SMS alert sent to admin
4. If incorrect:
   → Buzzer sounds
   → After 3 failed attempts → SMS warning sent
5. Meanwhile:
   → Ultrasonic sensor checks fuel height
   → Flow sensor calculates fuel dispensed
   → DHT11 monitors temperature/humidity
   → Blynk updates dashboard every second

📁 Code Structure (Main Highlights)

setup(): Initializes all hardware components and Blynk

loop(): Handles sensor readings, keypad input, servo control, and alerts

measureFuelLevel(): Calculates tank level from ultrasonic distance

measureTotalFuelFlow(): Uses pulse count to compute liters

sendSMS(): Sends AT commands to GSM module

verifyPassword(): Checks entered PIN vs saved PIN

soundBuzzer(): Activates buzzer on wrong password

📱 Screenshots 

Add photos of the circuit, Blynk dashboard, SMS alerts, etc.


🔐 Future Improvements

Add Firebase or MySQL backend for data history

Integrate Over-the-Air (OTA) updates

Create a mobile app dashboard UI

Add real-time alerts via Telegram/WhatsApp



---

🧑‍💻 Author

Shrinath Rajput
👨‍💻 AIML Engineer | IoT Developer
