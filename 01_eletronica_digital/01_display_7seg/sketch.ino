#define G 12
#define F 32
#define E 16
#define A 33
#define B 25
#define D 4
#define C 14
#define DP 27


void setup() {
 pinMode(G, OUTPUT);
 pinMode(F, OUTPUT);
 pinMode(E, OUTPUT);
 pinMode(A, OUTPUT);
 pinMode(B, OUTPUT);
 pinMode(D, OUTPUT);
 pinMode(C, OUTPUT);
 pinMode(DP, OUTPUT);
 Serial.begin(115200);
}


void loop() {
 for(int i = 0; i <= 9; i++){
  delay(1000);
  showNumber(i);
 }
}




void clearDisplay() {
 digitalWrite(A, LOW);
 digitalWrite(B, LOW);
 digitalWrite(C, LOW);
 digitalWrite(D, LOW);
 digitalWrite(E, LOW);
 digitalWrite(F, LOW);
 digitalWrite(G, LOW);
 digitalWrite(DP, LOW);
}


void showNumber(int val) {
 clearDisplay();

 if (val == 0) {
   digitalWrite(A, HIGH);
   digitalWrite(B, HIGH);
   digitalWrite(C, HIGH);
   digitalWrite(D, HIGH);
   digitalWrite(E, HIGH);
   digitalWrite(F, HIGH);
   digitalWrite(DP, HIGH);
 } else if (val == 1) {
   digitalWrite(B, HIGH);
   digitalWrite(C, HIGH);
   digitalWrite(DP, HIGH);
 } else if (val == 2) {
   digitalWrite(A, HIGH);
   digitalWrite(B, HIGH);
   digitalWrite(D, HIGH);
   digitalWrite(E, HIGH);
   digitalWrite(G, HIGH);
   digitalWrite(DP, HIGH);
 } else if (val == 3) {
   digitalWrite(A, HIGH);
   digitalWrite(B, HIGH);
   digitalWrite(C, HIGH);
   digitalWrite(D, HIGH);
   digitalWrite(G, HIGH);
   digitalWrite(DP, HIGH);
 } else if (val == 4) {
   digitalWrite(B, HIGH);
   digitalWrite(C, HIGH);
   digitalWrite(F, HIGH);
   digitalWrite(G, HIGH);
   digitalWrite(DP, HIGH);
 } else if (val == 5) {
   digitalWrite(A, HIGH);
   digitalWrite(C, HIGH);
   digitalWrite(D, HIGH);
   digitalWrite(F, HIGH);
   digitalWrite(G, HIGH);
   digitalWrite(DP, HIGH);
 } else if (val == 6) {
   digitalWrite(A, HIGH);
   digitalWrite(C, HIGH);
   digitalWrite(D, HIGH);
   digitalWrite(E, HIGH);
   digitalWrite(F, HIGH);
   digitalWrite(G, HIGH);
   digitalWrite(DP, HIGH);
 } else if (val == 7) {
   digitalWrite(A, HIGH);
   digitalWrite(B, HIGH);
   digitalWrite(C, HIGH);
   digitalWrite(DP, HIGH);
 } else if (val == 8) {
   digitalWrite(A, HIGH);
   digitalWrite(B, HIGH);
   digitalWrite(C, HIGH);
   digitalWrite(D, HIGH);
   digitalWrite(E, HIGH);
   digitalWrite(F, HIGH);
   digitalWrite(G, HIGH);
   digitalWrite(DP, HIGH);
 } else if (val == 9) {
   digitalWrite(A, HIGH);
   digitalWrite(B, HIGH);
   digitalWrite(C, HIGH);
   digitalWrite(D, HIGH);
   digitalWrite(F, HIGH);
   digitalWrite(G, HIGH);
   digitalWrite(DP, HIGH);
 }
}
