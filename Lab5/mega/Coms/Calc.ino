long number1;
long number2; 

char calSignal; 

long result;

void calc() {
    // while there are dada being sent to arduino,
    Serial.println();
    tft.println("Send me a calculation"); 
      int set = millis();
    delay(4000);
    number1 = Serial.parseInt();
    // number1 will be the first number
    
    // Note the use of "Serial.parseInt, so,
    // in case you use 23, it stores in 
    // number1 the number 23
    
    // if we used Serial.read(), it would
    // only store 2
    
    calSignal = Serial.read(); // calSignal will be the first
    // info after the first number
    
    number2 = Serial.parseInt(); // stores the second
    // number in number2j
    
    resolucao(); // Custom function to solve the calculations
    tft.print(number1);tft.print(" ");
    tft.print(calSignal);tft.print(" ");
    tft.print(number2);
    tft.println();
    tft.println("Result = ");
    tft.println(result);
    // Prints the result of the calculation
  
}

void resolucao() { // Custom function that
  // solves the calculations
  
  switch (calSignal) {
    // Here we use "switch...case" to save some space on
    // the sketch. It's, basicaly, a function that verifies
    // various "if" statements.
    
    // Here, it verifies what's the value held by 
    // calSigna. Basicaly, it verifies the "signal"
    // of the calculation
    
    case '+' : // if calSignal is '+'
    result = number1 + number2; // sums the numbers
    // and makes result hold the value of the calculation
    break; // break to exit the "case"
    case '-' : // if calSignal is '+'
    result = number1 - number2; // subtracts the numbers
    // and makes result hold the value of the calculation
    break; // break to exit the "case"
    case '*' : // if calSignal is '+'
    result = number1 * number2; // multiplies the numbers
    // and makes result hold the value of the calculation
    break; // break to exit the "case"
    case '/' : // se calSignal for '/'
    result = number1 / number2; // divides the numbers
    // and makes result hold the value of the calculation
    // PS: in case the division isn't exact, the result 
    // will be the nearest integrer
    break; // break to exit the "case"
    default : // If it's not any of these...
    result = 0;
  }
}
