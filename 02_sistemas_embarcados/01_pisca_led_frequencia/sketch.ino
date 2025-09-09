#define PINLED 17

int tempo, valor;
char someChar = '10';

void setup()
{
    Serial.begin(9600);
    pinMode(PINLED, OUTPUT);
}

void loop()
{
    someChar = Serial.read();
    String stringOne = String(someChar);
    valor = stringOne.toInt();
    tempo = valor * 100;
    Serial.println(tempo);
    digitalWrite(PINLED, HIGH);
    delay(tempo);
    digitalWrite(PINLED, LOW);
    delay(tempo);
}