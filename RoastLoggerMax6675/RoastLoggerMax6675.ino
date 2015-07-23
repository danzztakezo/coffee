#define SPEED 115200
#define PRECISION 2
#define INT_MS 250
#define INT_N 4

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

float t1, t2 = 0;
unsigned int n1, n2 = 0;

void setup() {
  Serial.begin(SPEED);
  setupPins(CS1, SCK1, SO1);
  //setupPins(CS2, SCK2, SO2);
}

void loop() {
  unsigned long cur = millis();
  if (cur - prev >= INT_MS) {
    prev = cur;
    if (n >= INT_N) {
      printTemp("t1", t1, n1);
      //printTemp("t2", t2, n2);
      n = 0;
    } else {
      sumTemp(readTemp(CS1, SCK1, SO1), t1, n1);
      //sumTemp(readTemp(CS2, SCK2, SO2), t2, n2);
      n++;
    }
  }
}

void setupPins(int cs, int sck, int so) {
  pinMode(cs, OUTPUT);
  pinMode(sck, OUTPUT);
  pinMode(so, INPUT);
  digitalWrite(cs, HIGH);
}

void printTemp(String label, float& sum, unsigned int& n) {
  Serial.print(label); Serial.print("="); Serial.println(n > 0 ? sum / n : -1, PRECISION);
  sum = 0;
  n = 0;
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
