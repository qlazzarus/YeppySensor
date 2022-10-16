#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT11.h>

// init lcd
LiquidCrystal_I2C lcd(0x27, 16, 2);

// lcd images
byte temperatureImage[] = {0x04, 0x0A, 0x0A, 0x0A, 0x0E, 0x1F, 0x1F, 0x0E};
byte humidityImage[] = {0x04, 0x0E, 0x0E, 0x1F, 0x1F, 0x1F, 0x0E};
byte doImage[] = {0x1C, 0x14, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00};
byte mircoImage[] = {0x11, 0x11, 0x11, 0x13, 0x15, 0x18, 0x10, 0x10};
byte threeImage[] = {0x18, 0x04, 0x18, 0x04, 0x18, 0x00, 0x00, 0x00};

// digital pin
const int DHT_PIN = 2;
const int DUST_PIN = 8;
const int MOTOR_PIN = 5; // relay will be here

// humidity sensor
float humidity = 0;
float temperature = 0;

// dust sensor
unsigned long duration;
unsigned long calcTime;
unsigned long sampleTime = 2000;
unsigned long lowPulseOccupancy = 0;
float ratio = 0;
float concentration = 0;
float dustDensity = 0;

// sample moving average filter
const int COUNT_NUM = 10;
float dustArray[COUNT_NUM];
int readIndex = 0;
float dustTotal = 0;
float dustAverage = 0;
boolean dustCycle = false;


// initial DHT11
DHT11 dht11(DHT_PIN);

// initial Serial
void initialSerial() {
  Serial.begin(9600);
}

// initial pin
void initialPin() {
  pinMode(DUST_PIN, INPUT);
  //pinMode(MOTOR_PIN, INPUT);
}

// initial lcd
void initialLcd() {
  lcd.begin();
  lcd.backlight();

  lcd.createChar(0, humidityImage);
  lcd.createChar(1, temperatureImage);
  lcd.createChar(2, doImage);
  lcd.createChar(3, mircoImage);
  lcd.createChar(4, threeImage);

  lcd.home();
  lcd.print("EJLOVE");
  lcd.setCursor(0, 1);
  lcd.print("YeppySensor");
}

void initialAverage() {
  for (int i = 0; i < COUNT_NUM; i++) {
    dustArray[i] = 0;
  }
}


void calcSensor() {
  duration = pulseIn(DUST_PIN, LOW);
  lowPulseOccupancy += duration;

  if ((millis() - calcTime) > sampleTime) {
    ratio = lowPulseOccupancy / (sampleTime * 10.0);
    // using spec sheet curve
    concentration = 1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62;
    dustDensity = (concentration * 100) / 13000;

    // sometimes return 0.00
    if (dustDensity > 0.009) {
      dustTotal -= dustArray[readIndex];
      dustArray[readIndex] = dustDensity;
      dustTotal += dustDensity;
      readIndex++;
  
      if (readIndex >= COUNT_NUM) {
        readIndex = 0;
        dustCycle = true;
      }
  
      dustAverage = dustTotal / COUNT_NUM;
    }
    
    lowPulseOccupancy = 0.0;
    calcTime = millis();
    dht11.read(humidity, temperature);
  }
}


// print lcd
void printLcd() {
  //lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(dustAverage);
  lcd.write(3);
  lcd.print("g/m");
  lcd.write(4);
  lcd.print("  ");

  lcd.setCursor(0, 1);
  lcd.write(0);
  lcd.print(" ");
  lcd.print(humidity);
  lcd.print("%");
  lcd.write(1);
  lcd.print(" ");
  lcd.print(temperature);
  lcd.write(2);
  lcd.print("C ");
}

void debugLcd() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(lowPulseOccupancy);
}

void setup() {
  initialSerial();
  initialPin();
  initialLcd();
  initialAverage();
  calcTime = millis();
}

void loop() {
  calcSensor();
  if (dustDensity > 0 && dustCycle == true) {
    printLcd();
  }
}
