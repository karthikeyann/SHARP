
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
  Serial.println("RDY");
}
// COMMAND SHEET
// 1 byte. 4 MSB command 4 LSB pin no.
// MSB commands
// 0 -> OFF
// 1 -> ON
// 2 -> TOGGLE
// 3 -> STATUS
// extra commands
// 4 -> ALL commands
//    0x40 -> OFF ALL
//    0x41 -> ON  ALL
//    0x42 -> TOGGLE ALL
//    0x43 -> STATUS ALL
// 0x81 -> PING
// 0x82 -> RESET

// Serial reponses.
// OK, ER, ####
// the loop function runs over and over again forever
void loop() {
  if (Serial.available() > 0) {
    String request = Serial.readStringUntil('\n');
    if(request.length()!=1) {
      Serial.println("ER");
    }
    char code = request.charAt(0);
    char command = code & 0xF0;
    int pin = code & 0x0F;
    //PING
    if (code == 0x00) {
      Serial.println("OK");
    }
    // Wrong pin no.
    else if ( (code<4) && (pin==0 || pin>4) ) {
      Serial.println("ER");
    }
    //OFF
    else if (command == 0x00) {
      R_value[pin-1]=LOW;
      digitalWrite(R_pins[pin-1],LOW);
      Serial.println("OK");
    }
    //ON
    else if (command == 0x10) {
      R_value[pin-1]=HIGH;
      digitalWrite(R_pins[pin-1],HIGH);
      Serial.println("OK");
    }
    //TOGGLE
    else if (command == 0x20) {
      R_value[pin-1]=!R_value[pin-1];
      digitalWrite(R_pins[pin-1],R_value[pin-1]);
      Serial.println("OK");
    }
    //STATUS
    else  if (command == 0x30) {
      if(R_value[pin-1]==HIGH)
        Serial.println("01");
      else if (R_value[pin-1]==LOW)
        Serial.println("00");
      else
        Serial.println("UN");
      Serial.println("OK");
    }
    // ALL commands
    else if (command == 0x40) {
      char sub_cmd=pin;
      for(int i=0;i<4;i++) {
        if(sub_cmd==0) {
          R_value[i]=LOW;
          digitalWrite(R_pins[i],LOW);
        } else if (sub_cmd==1) {
          R_value[i]=HIGH;
          digitalWrite(R_pins[i],HIGH);
        } else if (sub_cmd==2) {
          R_value[i]=!R_value[i];
          digitalWrite(R_pins[i],R_value[i]);
        } else if (sub_cmd==3) {
        Serial.print(String(R_value[i]));
        }
      }
      if(sub_cmd==3)
        Serial.println("");
      else
        Serial.println("OK");
    }
    //PING, RESET.
    else if (command == 0x80) {
      if(code==0x81)
        Serial.println("OK");
      else if(code==0x82) {
        Serial.println("OK");
      }
    }
    else {
      Serial.print("Unknown command:");
      Serial.println(request);
    }
  }
  delay(10);              // wait for a second
}
