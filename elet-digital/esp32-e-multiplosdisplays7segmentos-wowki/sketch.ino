#define A 18
#define B 19
#define C 16
#define D 4
#define E 0
#define F 17
#define G 5

int displays[] = {12, 14, 27};  
int valores[] = {1, 2, 3}; 

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando sequencia de displays...");

  for (int i = 0; i < 3; i++) {
    pinMode(displays[i], OUTPUT);
  }

  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  pinMode(E, OUTPUT);
  pinMode(F, OUTPUT);
  pinMode(G, OUTPUT);

  clearAll();
}


void loop() {
  for (int i = 0; i < 3; i++) {
    showNumber(valores[i], displays[i]);
    delay(1000);
    clearAll();
  }
}


void clearAll() {
  digitalWrite(displays[0], HIGH);
  digitalWrite(displays[1], HIGH);
  digitalWrite(displays[2], HIGH);

  digitalWrite(A, LOW); digitalWrite(B, LOW); digitalWrite(C, LOW);
  digitalWrite(D, LOW); digitalWrite(E, LOW); digitalWrite(F, LOW);
  digitalWrite(G, LOW);
}

void showNumber(int val, int display_pin) {
  digitalWrite(display_pin, LOW);

  switch (val) {
    case 1:
      digitalWrite(B, HIGH);
      digitalWrite(C, HIGH);
      break;
    case 2:
      digitalWrite(A, HIGH);
      digitalWrite(B, HIGH);
      digitalWrite(D, HIGH);
      digitalWrite(E, HIGH);
      digitalWrite(G, HIGH);
      break;
    case 3:
      digitalWrite(A, HIGH);
      digitalWrite(B, HIGH);
      digitalWrite(C, HIGH);
      digitalWrite(D, HIGH);
      digitalWrite(G, HIGH);
      break;
  }
}
