// ---- IR 센서 4개 핀 설정 ----
const int IR_PIN[4] = {A0, A1, A2, A3};   // IR 센서 4개 입력 핀
const int MOTOR_PIN = 9;                 // 모터 제어 핀

// ---- 거리 기준 ----
const float STOP_DISTANCE_CM = 10.0;     // 10cm 이내면 정지

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < 4; i++) {
    pinMode(IR_PIN[i], INPUT);
  }

  pinMode(MOTOR_PIN, OUTPUT);
  digitalWrite(MOTOR_PIN, HIGH);  // 시작 시 모터 ON
}

void loop() {
  bool obstacleDetected = false;

  for (int i = 0; i < 4; i++) {
    int irValue = analogRead(IR_PIN[i]);
    float distance = sensorToDistanceCm(irValue);

    Serial.print("Sensor ");
    Serial.print(i);
    Serial.print(" | Value: ");
    Serial.print(irValue);
    Serial.print(" | Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    // 하나라도 10cm 이하라면 정지 조건 활성화
    if (distance <= STOP_DISTANCE_CM) {
      obstacleDetected = true;
    }
  }

  // ---- 모터 제어 ----
  if (obstacleDetected) {
    digitalWrite(MOTOR_PIN, LOW);   // 정지
  } else {
    digitalWrite(MOTOR_PIN, HIGH);  // 계속 동작
  }

  delay(100);
}

// ---- IR 센서 값을 거리(cm)로 변환하는 함수 ----
// 실제 센서 모델에 맞게 보정 필요
float sensorToDistanceCm(int irValue) {
  if (irValue < 100) irValue = 100;
  float distance = 4800.0 / (irValue - 20);  // 예시식
  return distance;
}