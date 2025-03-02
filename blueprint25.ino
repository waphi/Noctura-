// Library for the screen
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define Photoresistor A1
#define LED 9

// Define screen parameters
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int bpm = 0;
int threshold = 400; // Adjust this based on sensor values
unsigned long lastPeakTime = 0;

// Timer variables for 10-second measurement
unsigned long startTime = 0;
bool measuring = true;
const int duration = 10000; // 10 seconds

// BPM storage
const int maxReadings = 20;
int bpmReadings[maxReadings];
int bpmCount = 0;

void setup() {
    Serial.begin(9600);
    delay(1000);
    pinMode(Photoresistor, INPUT);
    pinMode(LED, OUTPUT);

    startTime = millis();  // Start the timer
}

// Function to calculate BPM based on time between beats
int calculateBPM(unsigned long timeBetweenPeaks) {
    if (timeBetweenPeaks == 0) return 0; 
    return 60000 / timeBetweenPeaks; 
}

// Function to detect heartbeat and collect BPM values
void detectHeartRate() {
    if (!measuring) return;  // Stops measurement after 10 seconds

    int sensorValue = analogRead(Photoresistor);
    Serial.print("Sensor Value: "); 
    Serial.println(sensorValue); 

    unsigned long currentTime = millis();
    unsigned long timeBetweenPeaks = currentTime - lastPeakTime;

    if (sensorValue > threshold && timeBetweenPeaks > 300) {  
        lastPeakTime = currentTime;
        bpm = calculateBPM(timeBetweenPeaks);

        if (bpmCount < maxReadings) {
            bpmReadings[bpmCount] = bpm; 
            bpmCount++;
        }

        Serial.print("Heart Rate: ");
        Serial.println(bpm);
    }

    // Stop collecting after 10 seconds
    if (millis() - startTime >= duration) { 
        measuring = false; // Stops heart rate detection

        int sum = 0;
        for (int i = 0; i < bpmCount; i++) {
            sum += bpmReadings[i];
        }
        int avgBPM = bpmCount > 0 ? sum / bpmCount : 0;

        Serial.print("Average BPM: ");
        Serial.println(avgBPM);

        displayAverageBPM(avgBPM);

        delay(5000);
        startTime = millis(); 
        measuring = true; 
        bpmCount = 0;
    }
}

// Function to display BPM on the OLED screen
void displayAverageBPM(int bpmValue) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(10, 20);
    display.print("BPM: ");
    display.println(bpmValue);
    display.display();
}

// LED Blinking function
void rapidBlinking() {
    analogWrite(LED, 225);
    delay(7);
    analogWrite(LED, 0);
    delay(7);
}

void loop() {
    detectHeartRate();
    rapidBlinking();    
}
