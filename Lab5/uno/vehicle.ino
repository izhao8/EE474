char data;

void setup() {
	Serial.begin(9600);
}

void loop() {
	if(Serial.available() > 0) {
		data = Serial.read();
		//Serial.println(data);
		Serial.print("t");
	}
	delay(1000);
}