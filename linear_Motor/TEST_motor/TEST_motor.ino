// =========================
// TB6600 FULL MOTION SYSTEM (S-curve + Position Control)
// =========================

#define ENA_PIN   4
#define DIR_PIN   5
#define STEP_PIN  6

#define LIMIT_LEFT   7
#define LIMIT_RIGHT  8

#define BTN_START    9
#define BTN_STOP     10
#define ESTOP_PIN    11

#define POT_PIN      A0

// Motion parameters
long currentPos = 0;      // 현재 위치
long targetPos = 0;       // 목표 위치

int baseMinDelay = 800;   // 최소 속도
int baseMaxDelay = 200;   // 최대 속도
int jerkSteps = 80;       // S-curve 가속/감속 단계

bool running = false;
bool direction = LOW;


// =========================
// BASIC MOTOR CONTROL
// =========================

void motorOn()  { digitalWrite(ENA_PIN, LOW); }
void motorOff() { digitalWrite(ENA_PIN, HIGH); }

void stepOnce(int delayTime) {
  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(delayTime);
  digitalWrite(STEP_PIN, LOW);
  delayMicroseconds(delayTime);
}

bool leftHit()  { return digitalRead(LIMIT_LEFT)  == LOW; }
bool rightHit() { return digitalRead(LIMIT_RIGHT) == LOW; }
bool estopHit() { return digitalRead(ESTOP_PIN)   == LOW; }


// =========================
// S-curve delay profile
// =========================

int sCurveDelay(int i, int total) {
  float x = (float)i / total;
  float s = 3*x*x - 2*x*x*x;  // cubic S-curve
  return baseMinDelay - (baseMinDelay - baseMaxDelay) * s;
}


// =========================
// ABSOLUTE POSITION MOVE (S-curve)
// =========================

void moveTo(long target) {

  long distance = target - currentPos;
  if (distance == 0) return;

  direction = (distance > 0) ? HIGH : LOW;
  digitalWrite(DIR_PIN, direction);

  long steps = abs(distance);
  motorOn();

  // 1) S-curve acceleration
  for (int i = 0; i < jerkSteps; i++) {
    if (leftHit() || rightHit() || estopHit()) return;
    int d = sCurveDelay(i, jerkSteps);
    stepOnce(d);
    currentPos += (direction == HIGH ? 1 : -1);
  }

  // 2) constant speed
  for (long i = 0; i < steps - jerkSteps*2; i++) {
    if (leftHit() || rightHit() || estopHit()) return;
    stepOnce(baseMaxDelay);
    currentPos += (direction == HIGH ? 1 : -1);
  }

  // 3) S-curve deceleration
  for (int i = jerkSteps; i > 0; i--) {
    if (leftHit() || rightHit() || estopHit()) return;
    int d = sCurveDelay(i, jerkSteps);
    stepOnce(d);
    currentPos += (direction == HIGH ? 1 : -1);
  }
}


// =========================
// HOMING
// =========================

void homing() {

  // Move left until limit
  direction = LOW;
  digitalWrite(DIR_PIN, LOW);
  motorOn();

  while (!leftHit()) {
    if (estopHit()) return;
    stepOnce(600);
  }

  delay(300);

  // Move right slightly
  direction = HIGH;
  for (int i = 0; i < 50; i++) stepOnce(600);

  currentPos = 0;  // home = 0
}


// =========================
// SETUP
// =========================

void setup() {

  pinMode(ENA_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);

  pinMode(LIMIT_LEFT, INPUT_PULLUP);
  pinMode(LIMIT_RIGHT, INPUT_PULLUP);

  pinMode(BTN_START, INPUT_PULLUP);
  pinMode(BTN_STOP, INPUT_PULLUP);
  pinMode(ESTOP_PIN, INPUT_PULLUP);

  motorOff();

  homing();  // create absolute coordinate system
}


// =========================
// MAIN LOOP
// =========================

void loop() {

  if (estopHit()) {
    motorOff();
    running = false;
    return;
  }

  if (digitalRead(BTN_START) == LOW) {
    running = true;
    delay(200);
  }

  if (digitalRead(BTN_STOP) == LOW) {
    running = false;
    motorOff();
    delay(200);
  }

  if (!running) return;

  // speed control
  int pot = analogRead(POT_PIN);
  baseMaxDelay = map(pot, 0, 1023, 150, 800);

  // Example: move between 0 and 10000 steps
  static bool forward = true;

  if (forward) targetPos = 10000;
  else         targetPos = 0;

  moveTo(targetPos);

  if (leftHit() || rightHit()) {
    forward = !forward;
  }

  if (abs(currentPos - targetPos) < 5) {
    forward = !forward;
    delay(300);
  }
}