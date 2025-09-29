#define A 18
#define B 19
#define C 16
#define D 4
#define E 0
#define F 17
#define G 5

int displays[] = {27, 14, 12};
int num_displays = 3;
int digitos[3];

void setup() {
	Serial.begin(115200);
	Serial.println("Iniciando sequencia de displays...");

	for (int i = 0; i < num_displays; i++) {
		pinMode(displays[i], OUTPUT);
		digitalWrite(displays[i], HIGH);
	}

	pinMode(A, OUTPUT);
	pinMode(B, OUTPUT);
	pinMode(C, OUTPUT);
	pinMode(D, OUTPUT);
	pinMode(E, OUTPUT);
	pinMode(F, OUTPUT);
	pinMode(G, OUTPUT);
}


void loop() {
	int numero = 89;

	int temp_num = numero;
	for (int i = 0; i < num_displays; i++) {
		digitos[i] = temp_num % 10;
		temp_num /= 10;
	}

	for (int i = 0; i < num_displays; i++) {
		clearSegments();
		showNumber(digitos[i], displays[i]);
		digitalWrite(displays[i], HIGH);
	}
}


void clearSegments() {
	digitalWrite(A, LOW);
	digitalWrite(B, LOW);
	digitalWrite(C, LOW);
	digitalWrite(D, LOW);
	digitalWrite(E, LOW);
	digitalWrite(F, LOW);
	digitalWrite(G, LOW);
}


void showNumber(int val, int display_pin) {
	switch (val) {
	case 0:
		digitalWrite(A, HIGH);
		digitalWrite(B, HIGH);
		digitalWrite(C, HIGH);
		digitalWrite(D, HIGH);
		digitalWrite(E, HIGH);
		digitalWrite(F, HIGH);
		break;
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
	case 4:
		digitalWrite(B, HIGH);
		digitalWrite(C, HIGH);
		digitalWrite(F, HIGH);
		digitalWrite(G, HIGH);
		break;
	case 5:
		digitalWrite(A, HIGH);
		digitalWrite(C, HIGH);
		digitalWrite(D, HIGH);
		digitalWrite(F, HIGH);
		digitalWrite(G, HIGH);
		break;
	case 6:
		digitalWrite(A, HIGH);
		digitalWrite(C, HIGH);
		digitalWrite(D, HIGH);
		digitalWrite(E, HIGH);
		digitalWrite(F, HIGH);
		digitalWrite(G, HIGH);
		break;
	case 7:
		digitalWrite(A, HIGH);
		digitalWrite(B, HIGH);
		digitalWrite(C, HIGH);
		break;
	case 8:
		digitalWrite(A, HIGH);
		digitalWrite(B, HIGH);
		digitalWrite(C, HIGH);
		digitalWrite(D, HIGH);
		digitalWrite(E, HIGH);
		digitalWrite(F, HIGH);
		digitalWrite(G, HIGH);
		break;
	case 9:
		digitalWrite(A, HIGH);
		digitalWrite(B, HIGH);
		digitalWrite(C, HIGH);
		digitalWrite(D, HIGH);
		digitalWrite(F, HIGH);
		digitalWrite(G, HIGH);
		break;
	}

	digitalWrite(display_pin, LOW);
}
