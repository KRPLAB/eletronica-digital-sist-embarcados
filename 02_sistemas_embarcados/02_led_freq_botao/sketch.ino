// --- Pinos ---
const int pinoLed = 5; 
const int pinoBotaoAumenta = 25;
const int pinoBotaoDiminui = 26;

// --- Controle do PWM por Software ---
// Período total do ciclo PWM em milissegundos. 
// Um valor baixo (ex: 20ms) cria uma frequência alta (50Hz), que o olho não percebe.
const int periodo = 20; 

// "cicloDeTrabalho" é o tempo que o LED ficará LIGADO dentro do período.
// Este valor irá variar de 0 (0% de brilho) até o 'periodo' (100% de brilho).
// Começa com 50% de brilho (10ms de 20ms)
int cicloDeTrabalho = 10;

void setup() {
  Serial.begin(115200);
  pinMode(pinoLed, OUTPUT);
  pinMode(pinoBotaoAumenta, INPUT_PULLUP);
  pinMode(pinoBotaoDiminui, INPUT_PULLUP);
}

void gerarPwmSoftware() {
  if (cicloDeTrabalho > 0) {
    digitalWrite(pinoLed, HIGH);
    delay(cicloDeTrabalho);
  }

  digitalWrite(pinoLed, LOW);
  delay(periodo - cicloDeTrabalho);
}

void loop() {
  if (digitalRead(pinoBotaoAumenta) == LOW) {
    cicloDeTrabalho++;
    if (cicloDeTrabalho > periodo) {
      cicloDeTrabalho = periodo;
    }
    Serial.print("Ciclo de Trabalho: ");
    Serial.println(cicloDeTrabalho);
    delay(50);
  }

  if (digitalRead(pinoBotaoDiminui) == LOW) {
    cicloDeTrabalho--;
    if (cicloDeTrabalho < 0) {
      cicloDeTrabalho = 0;
    }
    Serial.print("Ciclo de Trabalho: ");
    Serial.println(cicloDeTrabalho);
    delay(50);
  }

  gerarPwmSoftware();
}