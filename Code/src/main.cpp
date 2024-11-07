#include <TM1637Display.h>
#include <Arduino.h>

#define CLK 4
#define DIO 3
#define LED_1 9
#define LED_2 10
#define LED_3 11
#define LED_4 13
#define buzzer 12

const int buttonPin_1 = 5; 
const int buttonPin_2 = 6;
const int buttonPin_3 = 7;

float value;
float threshold = 0;

int sensorPin = A0; // Cảm biến MQ-3 kết nối với chân A0
TM1637Display display(CLK, DIO);

boolean buttonState1 = HIGH;
boolean buttonState2 = HIGH;
boolean buttonState3 = HIGH;

boolean systemOn = true; // State variable to keep track of system status

void setup() {
  Serial.begin(9600);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(LED_4, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(buttonPin_1, INPUT_PULLUP); // New button for turning the system on/off
  pinMode(buttonPin_2, INPUT_PULLUP); // Kết nối nút nhấn với điện trở kéo lên bên ngoài
  pinMode(buttonPin_3, INPUT_PULLUP); // Kết nối nút nhấn với điện trở kéo lên bên ngoài

  threshold = 400;
  display.setBrightness(0x0f); // Đặt độ sáng tối đa
  display.clear();
  delay(2000);
}

boolean debounceButton(int buttonPin, boolean state) {
  boolean stateNow = digitalRead(buttonPin);
  if (state != stateNow) {
    delay(100); // Chờ 100ms để debounce
    stateNow = digitalRead(buttonPin);
  }
  return stateNow;
}

void button() {
  buttonState1 = debounceButton(buttonPin_1, buttonState1);
  if (buttonState1 == LOW) {
    systemOn = !systemOn; // Toggle system state
    digitalWrite(LED_1, HIGH); // Briefly light up LED 3 as feedback
    digitalWrite(LED_2, HIGH);
    digitalWrite(LED_3, HIGH);
    digitalWrite(LED_4, HIGH);

    delay(500);
    digitalWrite(LED_1, LOW); // Briefly light up LED 3 as feedback
    digitalWrite(LED_2, LOW);
    digitalWrite(LED_3, LOW);
    digitalWrite(LED_4, LOW);
     Serial.println("nut 1 da duoc nhan ");
  }
  if (!systemOn) {
    return; // If system is off, skip the rest of the button checks
  }

  buttonState2 = debounceButton(buttonPin_2, buttonState2);
  if (buttonState2 == LOW) {
    threshold += 50;
    digitalWrite(LED_3, HIGH);
    delay(500); // Đèn LED sáng trong 500ms
    digitalWrite(LED_3, LOW);
    Serial.println("nut 2 da duoc nhan ");
  }

  buttonState3 = debounceButton(buttonPin_3, buttonState3);
  if (buttonState3 == LOW) {
    threshold -= 50;
    digitalWrite(LED_4, HIGH);
    delay(500); // Đèn LED sáng trong 500ms
    digitalWrite(LED_4, LOW);
    Serial.println("nut 3 da duoc nhan ");
  }
  

  
  
}

void loop() {
  button();

  if (!systemOn) {
    display.clear(); // Clear the display if the system is off
    digitalWrite(LED_1, LOW);
    digitalWrite(LED_2, LOW);
    digitalWrite(LED_3, LOW);
    digitalWrite(LED_4, LOW);
    digitalWrite(buzzer, LOW);
    return;
  }

  int rawValue = analogRead(sensorPin);
  value = rawValue / 1024.0 * 5.0; // Chuyển đổi giá trị analog sang điện áp (0-5V)

  float alcoholValue = (value / 5.0) * 500.0; // Điều chỉnh công thức tính toán

  Serial.print("Alcohol Value: ");
  Serial.println(alcoholValue);

  display.showNumberDec(alcoholValue, false, 4, 0); // Hiển thị giá trị nồng độ cồn

  if (alcoholValue < threshold) {
    digitalWrite(LED_2, HIGH); // Bật LED 2
    digitalWrite(LED_1, LOW); // Tắt LED 1
  }
  // Khi có cồn
  else {
    digitalWrite(LED_2, LOW); // Tắt LED 2
    digitalWrite(LED_1, HIGH); // Bật LED 1
  }

  if (alcoholValue > (threshold + 100)) {
    digitalWrite(buzzer, HIGH);
    for (int i = 0; i < 20; i++) {
      digitalWrite(LED_1, HIGH);
      delay(100);
      digitalWrite(LED_1, LOW);
      delay(100);
    }
  } else {
    digitalWrite(buzzer, LOW);
  }

  delay(1000); // Thêm một khoảng trễ ngắn để tránh vòng lặp quá nhanh
}
