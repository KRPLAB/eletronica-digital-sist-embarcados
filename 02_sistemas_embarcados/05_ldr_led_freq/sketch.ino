const int ledPin = 19;
const int sensorPin = 33;
int cicloDeTrabalho = 10;

void setup() {
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
  pinMode(ledPin, OUTPUT);
  pinMode(sensorPin, INPUT);
}

void loop() {
  int var=analogRead(sensorPin);
  
  Serial.println("Tensão:");
  Serial.println(var);
  digitalWrite(ledPin, HIGH);
  delay((var * cicloDeTrabalho * 100)/4096);
  digitalWrite(ledPin, LOW);
  delay((var * cicloDeTrabalho * 100)/4096);
}