const byte adjPowerPin = 3;
const byte latchPin = 4;
const byte eePin = 5;
const byte emPin = 6;
const byte prrPin = 7;
const byte buttonPin = A0;
const byte ledPin = 13;
const byte guideLaserPin = 8;

byte powerLevel = 20;

const byte auxToPowerMillis = 100;
const byte powerToLatchMillis = 100;
const byte latchToEeMillis = 100;
const byte eeToEmMillis = 100;
const byte periodMillis = 100;
const byte latchDurationMillis = 10;

const char enableKey = 'e';
const char disableKey = 'd';
const char powerKey = 'p';
const char powerStatusKey = 's';

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
  pinMode(guideLaserPin, OUTPUT);
  digitalWrite(guideLaserPin, HIGH);
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
  checkSerial();
}

void checkSerial() {
  static char buffer[80];
  int len = readline(Serial.read(), buffer, 80);
  if (len > 0) {
    String message = buffer;
    if (message.charAt(0) == powerKey) {
      String powerLvlString = String(message);
      long lvl = powerLvlString.substring(1, len).toInt();
      if (lvl >= 0 && lvl <= 100) {
        powerLevel = (byte) lvl;
        submitPower(powerLevel);
      } else {
        Serial.println("Error: Invalid power level, should be greater than 0 and smaller than 100");
      }
    } else if (message.charAt(0) == powerStatusKey) {
      Serial.println(powerLevel, DEC);
    }
  }
}

void disableLaser() {
  digitalWrite(emPin, LOW);
  delay(periodMillis);
  digitalWrite(eePin, LOW);
  delay(periodMillis);
  digitalWrite(adjPowerPin, LOW);
  digitalWrite(prrPin, LOW);
  laserEnabled = false;
  digitalWrite(ledPin, LOW);
  digitalWrite(guideLaserPin, HIGH);
  Serial.println(disableKey);
}

void enableLaser() {
  digitalWrite(guideLaserPin, LOW);
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
  Serial.print(enableKey);
  Serial.println(powerLevel);
}

void submitPower(byte lvl) {
  if (lvl >= 0 && lvl <= 100) {
    analogWrite(adjPowerPin, map(lvl, 0, 100, 0, 200));
  }
}

int readline(int readch, char *buffer, int len)
{
  static int pos = 0;
  int rpos;

  if (readch > 0) {
    switch (readch) {
      case '\n': // Ignore new-lines
        break;
      case '\r': // Return on CR
        rpos = pos;
        pos = 0;  // Reset position index ready for next time
        return rpos;
      default:
        if (pos < len - 1) {
          buffer[pos++] = readch;
          buffer[pos] = 0;
        }
    }
  }
  // No end of line has been found, so return -1.
  return -1;
}

