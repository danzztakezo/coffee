#define SPEED 19200
#define PRECISION 1
#define INT_MS 250
#define INT_N 4
#define MAXCMD 128
#define NC 4

/*
 * Connect the first MAX6675 board to digital pins:
 * 2 - CS
 * 3 - SCK
 * 4 - SO
 */
#define CS1 2
#define SCK1 3
#define SO1 4

/*
 * Connect the second MAX6675 board to digital pins:
 * 5 - CS
 * 6 - SCK
 * 7 - SO
 */
#define CS2 5
#define SCK2 6
#define SO2 7

unsigned long prev = 0;
unsigned int n = 0;

float c1, c2 = -1;

float t1, t2 = 0;
unsigned int n1, n2 = 0;

String cmd = "";
unsigned int chan[] = { 1, 2, 0, 0 };

void setup() {
  Serial.begin(SPEED);
  setupPins(CS1, SCK1, SO1);
  //setupPins(CS2, SCK2, SO2);
}

void loop() {
  checkSerial();
  unsigned long cur = millis();
  if (cur - prev >= INT_MS) {
    prev = cur;
    if (n >= INT_N) {
      c1 = averageTemp(t1, n1);
      //c2 = averageTemp(t2, n2);
      n = 0;
    } else {
      sumTemp(readTemp(CS1, SCK1, SO1), t1, n1);
      //sumTemp(readTemp(CS2, SCK2, SO2), t2, n2);
      n++;
    }
  }
}

void checkSerial() {
  if (Serial.available() > 0) {
    char c = Serial.read();
    if ((c == '\n') || (c == '\r')) {
      if (cmd == "READ") {
        Serial.print(0.0, PRECISION);
        for (int i = 0; i < NC; i++) {
          if (chan[i] > 0) {
            Serial.print(",");
            float t = 0;
            if (chan[i] == 1) {
              t = c1;
            } else if (chan[i] == 2) {
              t = c2;
            }
            Serial.print(t, PRECISION);
          } 
        }
        Serial.println();
      } else if (cmd.startsWith("CHAN") && cmd.length() == 9) {
        Serial.print("# Active channels set to ");
        for (int i = 0; i < NC; i++) {
          chan[i] = cmd.charAt(i + 5) - '0';
          Serial.print(chan[i]);
        }
        Serial.println();
      }
      cmd = "";
    } else {
      if (cmd.length() > MAXCMD) {
        cmd = "";
      }
      cmd += c;
    }
  }
}

void setupPins(int cs, int sck, int so) {
  pinMode(cs, OUTPUT);
  pinMode(sck, OUTPUT);
  pinMode(so, INPUT);
  digitalWrite(cs, HIGH);
}

float averageTemp(float& sum, unsigned int& n) {
  float t = n > 0 ? sum / n : -1;
  sum = 0;
  n = 0;
  return t;
}

void sumTemp(float temp, float& sum, unsigned int& n) {
  if (temp >= 0) {
    sum += temp;
    n++;
  }
}

float readTemp(int cs, int sck, int so) {
  digitalWrite(cs, LOW);
  delayMicroseconds(1);

  unsigned int value = 0;
  for (int i = 0; i < 14; i++) {
    digitalWrite(sck, HIGH);
    delayMicroseconds(1);
    value = (value << 1) | digitalRead(so);
    digitalWrite(sck, LOW);
    delayMicroseconds(1);
  }

  digitalWrite(cs, HIGH);
  delayMicroseconds(1);
  
  if (value & 0x1) {
    return -1;
  } else {
    return 0.25 * (value >> 1);
  }
}
