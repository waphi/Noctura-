//library for the screen
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define Photoresistor A1
#define LED 9

//what we need to define in order for the screen to work
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int bpm = 0;
int threshold = 600;
unsigned long lastPeakTime = 0;

// Timer variables for 10-second measurement
unsigned long startTime = 0;
bool measuring = true;
const int duration = 10000; //10000ms = 10s

// BPM storage
const int maxReadings = 20; // Stores up to 20 BPM values
int bpmReadings[maxReadings];
int bpmCount = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(Photoresistor, INPUT);
  pinMode(LED, OUTPUT);

  startTime = millis(); //will start the program the moment the thing is turns on
}

void fade_led() {
  for (int i = 0; i <= 255; ++i) {
    analogWrite(LED, i);
    delay(10);
  }
}

int bpmCalculations(unsigned long timeBetweenPeaks) {
    if (timeBetweenPeaks == 0) return 0; // Avoid division by zero
    return 60000 / timeBetweenPeaks; // Convert milliseconds to BPM
}

void detectHeartRate() {
    if (!measuring) return;  // Stop measurement after 10 seconds

    int sensorValue = analogRead(Photoresistor);
    unsigned long currentTime = millis();
    unsigned long timeBetweenPeaks = currentTime - lastPeakTime;

    if (sensorValue > threshold && timeBetweenPeaks > 300) {  
        lastPeakTime = currentTime;
        bpm = calculateBPM(timeBetweenPeaks);

        if (bpmCount < maxReadings) {
            bpmReadings[bpmCount] = bpm;  // Stores BPM value
            bpmCount++;
        }

        Serial.print("Heart Rate: ");
        Serial.println(bpm);
    }
}

    // Stop collecting after 10 seconds
  if (millis() - startTime >= duration) {
      measuring = false; // Stops heart rate detection

        int sum = 0;
        for (int i = 0; i < bpmCount; i++) {
            sum += bpmReadings[i];
        }
        int avgBPM = bpmCount > 0 ? sum / bpmCount : 0;
        displayAverageBPM(avgBPM);
}

int calculateBPM(unsigned long timeBetweenPeaks) {
    if (timeBetweenPeaks == 0) return 0; // Avoid division by zero
    return 60000 / timeBetweenPeaks; // Convert milliseconds to BPM
}
//displaying the BPM to the display
void displayAverageBPM(int bpmValue) {
  if (bpmCount == 0) return; // Prevent division by zero

    int sum = 0;
    for (int i = 0; i < bpmCount; i++) {
        sum += bpmReadings[i];
    }
    int avgBPM = sum / bpmCount; // Calculate average

    Serial.print("Average BPM: ");
    Serial.println(avgBPM);

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(10, 20);
    display.print("BPM: ");
    display.println(bpmValue);
    display.display();
}

//just to test the led 
void rapidBlinking(){
  analogWrite(LED, 225);
  delay(7);
  analogWrite(LED, 225);
  delay(7);
}

void loop() {
  // put your main code here, to run repeatedly:
  detectHeartRate();
  rapidBlinking();

}
