// wire 라이브러리
#include <Wire.h>
// 액정 라이브러리
#include <LiquidCrystal_I2C.h>
// 습도 센서 라이브러
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
const int BUZZER_PIN = 5;
const int MOTOR_PIN = 6;

const int COUNT_NUM = 10;

// 온습도 센서
float humidity = 0;
float temperature = 0;

// dust sensor
unsigned long duration;
unsigned long mainCalcTime;
// 먼지센서 샘플링 시간 - 5sec
unsigned long sampleTime = 5000;
unsigned long lowPulseOccupancy = 0;
float ratio = 0;
float concentration = 0;
float dustDensity = 0;
boolean mainCheck = false;
unsigned int dustState = 0;
unsigned int buzzerCount = 0;

// initial DHT11
DHT11 dht11(DHT_PIN);

// initial Serial
void initialSerial() {
  Serial.begin(9600);
}

// initial pin
void initialPin() {
  pinMode(DUST_PIN, INPUT);
  //pinMode(BUZZER_PIN, INPUT);
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
  lcd.print("EJLOVE!!");
}


/**
   신뢰할 수 있는 먼지밀도 계산하기
   대부분의 아날로그센서의 경우 값이 튀는 현상이 있는데,
   이것을 보정하기 위해 여러번 값을 누적한 후,
   평균값을 내어 신뢰할 수 있는 먼지밀도를 구합니다.
*/
void calcSensor() {
  duration = pulseIn(DUST_PIN, LOW);
  lowPulseOccupancy += duration;

  // 먼지센서 샘플링 시간 - 2sec 동안 duration / lowPulseOccupancy 로 확인
  if ((millis() - mainCalcTime) > sampleTime) {
    ratio = lowPulseOccupancy / (sampleTime * 10.0);
    // using spec sheet curve
    concentration = 1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62;

    dustDensity = (concentration * 100) / 13000;
    lowPulseOccupancy = 0.0;

    /*
    dustState = 0;
    if (dustDensity > 150) {
      dustState = 3;
    } else if (dustDensity > 80) {
      dustState = 2;
    } else if (dustDensity > 30) {
      dustState = 1;
    }
    */

    mainCalcTime = millis();
    dht11.read(humidity, temperature);
  }
}


// print lcd
void printLcd() {
  //lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(dustDensity);
  lcd.write(3);
  lcd.print("g/m");
  lcd.write(4);
  lcd.setCursor(10, 0);

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
  mainCalcTime = millis();
}

void loop() {
  // 부저 + 팬 + 
  calcSensor();
  printLcd();
}
