int inputCheck(int press);
char data;
void setup() {
	Serial.begin(9600);
}

void loop() {
	if(Serial.available() > 0) {
		data = Serial.read();
	}
	if(inputCheck(data)) {
		int time = millis();
		while(millis()-time < 1000){

		}
		int response = Serial.read();
		Serial.println(response);
	} else {
		Serial.println(0);
		delay(1000);
	}
}

int inputCheck(int press) {
  if(press == 102) {
    return 1;
  } else if(press == 98) {
    return 1;
  } else if(press == 108) {
    return 1;
  } else if(press == 114) {
    return 1;
  } else if(press == 100) {
    return 1;
  } else if(press == 104) {
    return 1;
  } 

  // Lab 5 additions:
   else if(press == 115) {  // s
    return 1;
  } else if(press == 105) { // i
    return 1;
  } else if(press == 119) { // w
    return 1;                     // These two need a response of "A" (97)
  } else if(press == 112) { // p
    return 1;
  } 


  else {
    return 0;
  }
}