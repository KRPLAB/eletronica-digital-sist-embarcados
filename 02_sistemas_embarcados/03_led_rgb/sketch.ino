#define RED 19
#define GRN 18
#define BLU 17

void setup() {
	Serial.begin(115200);
	Serial.println("Hello, ESP32!");
	pinMode(RED, OUTPUT);
	pinMode(GRN, OUTPUT);
	pinMode(BLU, OUTPUT);
}

void loop() {
	int i, j, k;
	
	for(i=1; i<255; i+=30)
	{
		for(j=1; j<255; j+=30)
		{
			for(k=1; k<255; k+=30)
			{
				analogWrite(RED,i);
				analogWrite(GRN,j);
				analogWrite(BLU,k);
				delay(20);
				// Serial.println(" RGB:");
				// Serial.println(i);
				// Serial.println(j);
				// Serial.println(k);
			}
		}
	}
}
