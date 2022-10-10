// wire 라이브러리
#include <Wire.h>
// 액정 라이브러리
#include <LiquidCrystal_I2C.h>
// 습도 센서 라이브러
// #include <DHT11.h>

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

// dust sensor
unsigned long duration;
unsigned long startTime;
// 먼지센서 샘플링 시간 - 2sec
unsigned long sampleTime = 2000;
unsigned long lowPulseOccupancy = 0;
float ratio = 0;
float concentration = 0;
float dustDensity = 0;
float dustState = 0;
boolean dustCalculateRun = false;
boolean dustCalculateDone = false;
unsigned int buzzerCount = 0;

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
  lcd.print("EJLOVE Loading..");
}

// print lcd
void printLcd() {
  lcd.clear();
}

/* 신뢰할 수 있는 먼지밀도 계산하기
   대부분의 아날로그센서의 경우 값이 튀는 현상이 있는데, 
   이것을 보정하기 위해 여러번 값을 누적한 후, 
   평균값을 내어 신뢰할 수 있는 먼지밀도를 구합니다.
*/
void calcDustDensity() {
  duration = pulseIn(DUST_PIN, LOW);
  lowPulseOccupancy = lowPulseOccupancy + duration;

  if ((millis() - startTime) > sampleTime) {
    dustCalculateRun = false;
    dustCalculateDone = true;

    ratio = lowPulseOccupancy / (sampleTime * 10.0);
    concentration = 1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) * 520 * ratio + 0.62;
    dustDensity = concentration * 100 / 1000;
    lowPulseOccupancy = 0;
    dustState = 0;
    
    if (dustDensity > 150) {
      dustState = 3;
    } else if (dustDensity > 80) {
      dustState = 2;
    } else if (dustDensity > 30) {
      dustState = 1;
    }
  }
}

void setup() {
  initialPin();
  initialLcd();
  startTime = millis();
}

void loop() {
  // @TODO
  // 1. 미세먼지 측정
  // 2. 이후 LCD 표기 갱신
  // 3. 특정 수치가 되면 팬 동작 (동작확인이 필요)
  // printLcd();
  /*
  if (dustCalculateRun == true) {
    
  } else {
    // @TODO buzzer && fan control

    // 먼지센서 초기화
    dustCalculateRun = true;
    startTime = millis();
  }
  */
}
