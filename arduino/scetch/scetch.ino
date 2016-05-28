const byte adjPowerPin = 45;
const byte latchPin = 37;
const byte eePin = 35;
const byte emPin = 33;
const byte prrPin = 31;
const byte buttonPin = A0;

const byte powerLevel = 50;

const byte auxToPowerMillis = 100;
const byte powerToLatchMillis = 100;
const byte latchToEeMillis = 100;
const byte eeToEmMillis = 100;
const byte periodMillis = 100;

const byte latchDurationMillis = 10;

const byte ledPin = 13;

boolean laserEnabled = false;
boolean inAfterStartState = true;

void setup() {
  pinMode(adjPowerPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(eePin, OUTPUT);
  pinMode(emPin, OUTPUT);
  pinMode(prrPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if (digitalRead(buttonPin) == HIGH && laserEnabled == false && !inAfterStartState) {
    enableLaser();
  } else if (digitalRead(buttonPin) == LOW && laserEnabled == true) {
    disableLaser();
  } else if (digitalRead(buttonPin) == LOW && laserEnabled == false) {
    inAfterStartState = false;
  }
}

void disableLaser() {
  Serial.println("Disabling");
  digitalWrite(emPin, LOW);
  delay(periodMillis);
  digitalWrite(eePin, LOW);
  delay(periodMillis);
  for(byte i = 0; i < 8; i++) {
    digitalWrite(powerPins[i], LOW);
  }
  digitalWrite(prrPin, LOW);
  laserEnabled = false;
  digitalWrite(ledPin, LOW);
}

void enableLaser() {
  Serial.println("Enabling");
  digitalWrite(prrPin, HIGH);
  submitPower(powerLevel);
  delay(periodMillis);
  digitalWrite(latchPin, HIGH);
  delay(latchDurationMillis);
  digitalWrite(latchPin, LOW);
  delay(periodMillis);
  digitalWrite(eePin, HIGH);
  delay(periodMillis);
  digitalWrite(emPin, HIGH);
  laserEnabled = true;
  digitalWrite(ledPin, HIGH);
}

void submitPower(byte lvl) {
  if (lvl >= 0 && lvl <= 100) {
    analogWrite(adjPowerPin, map(lvl, 0, 100, 0, 200));
  }
}
