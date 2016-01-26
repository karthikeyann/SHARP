
const int R_pins[4] = {5, 6, 7, 8};
boolean R_value[4];
void setup() {
  // initialize digital pin R_0 as an output.
  Serial.begin(115200);
  for(int i=0;i<4;i++) {
    pinMode(R_pins[i], OUTPUT);
    digitalWrite(R_pins[i],LOW);
    R_value[i] = LOW;
  }
}

// the loop function runs over and over again forever
void loop() {
  if (Serial.available() > 0) {
    String request = Serial.readStringUntil('\n');
    boolean command = false;
    int pin=0;
    if (request.indexOf("--STATUS") != -1)  {
      Serial.println("STATUS hio");
      for(int i=0;i<4;i++) {
        Serial.print("Relay " + String(i+1) + " = ");
        Serial.println(R_value[i]);
      }
    } else if (request.indexOf("--RESET") != -1)  {
      Serial.println("RESET");
      for(int i=0;i<4;i++) {
        Serial.println("Relay " + String(i+1) + " = LOW");
         R_value[i] = LOW;
        digitalWrite(R_pins[i],LOW);
      }
    } else if (request.indexOf("--SET") != -1)  {
      Serial.println("SET");
      for(int i=0;i<4;i++) {
        Serial.println("Relay " + String(i+1) + " = HIGH");
        R_value[i] = HIGH;
        digitalWrite(R_pins[i],HIGH);
      }
    } else if (request.indexOf("--TOGGLE") != -1)  {
      Serial.println("TOGGLE");
      for(int i=0;i<4;i++) {
        R_value[i] = !R_value[i];
        Serial.println("Relay " + String(i+1) + " = "+ String(R_value[i]));
        digitalWrite(R_pins[i],R_value[i]);
      }
    } else if (request.indexOf("--LED ") != -1)  {
      command = true;
      //01 02 03 04
      int id = request.substring(6,8).toInt();
      if ( id==0 || id > 4 )  {
        Serial.println("Wrong format for LED control:" + request.substring(6,8));
        command = false;
      }
      else {
        pin = R_pins[id-1];
        boolean value = R_value[id-1];
        String todo = request.substring(9);
        if (todo.indexOf("ON") != -1)
          value = HIGH;
        else if (todo.indexOf("OFF")!= -1)
          value = LOW;
        else if (todo.indexOf("TOGGLE")!= -1)
          value = !value;
        else
          command = false;
        //// COMMAND EXECUTED HERE ////
        if(command) {
          digitalWrite(pin, value);
          R_value[id-1] = value;
          Serial.println("Relay " + String(id) + " = " + String(value));
          Serial.println("DONE: " + request);
        }
      }
    } else {
      Serial.print("Unknown command:");
      Serial.println(request);
    }
  }
  delay(100);              // wait for a second
}
