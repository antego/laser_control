const byte adjPowerPin = 45;
const byte latchPin = 37;
const byte eePin = 35;
const byte emPin = 33;
const byte prrPin = 31;
const byte buttonPin = A0;
const byte ledPin = 13;

const byte powerLevel = 50;

const byte auxToPowerMillis = 100;
const byte powerToLatchMillis = 100;
const byte latchToEeMillis = 100;
const byte eeToEmMillis = 100;
const byte periodMillis = 100;
const byte latchDurationMillis = 10;

const char enableKey = 'e';
const char disableKey = 'd';
const char powerKey = 'p';

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
  checkSerial();
}

void checkSerial() {
  static char buffer[80];
  int len = readline(Serial.read(), buffer, 80);
  if (len > 0) {
    String message = buffer;
    if (message.charAt(0) == enableKey && laserEnabled == false) {
      enableLaser();
      submitPower((byte)message.charAt(1));
    } else if (message.charAt(0) == disableKey && laserEnabled == true) {
      disableLaser();
    } else if (message.charAt(0) == powerKey && laserEnabled == true) {
      submitPower((byte)message.charAt(1));
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
  Serial.println(disableKey);
}

void enableLaser() {
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
  Serial.println(enableKey);
  Serial.println((char)powerLevel);
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
