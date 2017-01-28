#include <OneWire.h>
#include <DallasTemperature.h>

#define DELAY 100
#define ONE_WIRE_BUS 4
#define TEMPERATURE_ALARM_HI 28
#define TEMPERATURE_ALARM_LO 10
#define USE_SERIAL_FOR_TEMPERATURE_SERVO true
#define STATE_CHANGE_DELAY 60                                    // how often to allow a heater state change (s), watch out for overflow

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

boolean isHeaterOn = false, isHeaterOnLast = false, turnHeaterOn = false;
float serialTemperatureSetPoint;
float T_DMD = 0.;

long int lastSampleTime = 0 - (STATE_CHANGE_DELAY*1000.);

boolean IsNumeric(String str) {
  for(char i = 0; i < str.length(); i++) {
    if ( !(isDigit(str.charAt(i)) || str.charAt(i) == '.' )) {
      return false;
    }
  }
  return true;
}

String readSerialInput() {
  String msg;
  while (Serial.available() > 0) {
    char c = Serial.read();
    msg+=c;
  }
  return msg;
}

void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT);    // relay in1 
  pinMode(3, OUTPUT);    // relay in2
  pinMode(5, OUTPUT);    // pot 5v
  pinMode(8, OUTPUT);    // led
  
  digitalWrite(5, HIGH);
  
  sensors.begin();
}

void loop() {
  // get temperature (ACT)
  sensors.requestTemperatures();
  
  float T[4];
  T[0] = sensors.getTempCByIndex(0);  
  T[1] = sensors.getTempCByIndex(1);  
  T[2] = sensors.getTempCByIndex(2);  
  T[3] = sensors.getTempCByIndex(3);   
  float T_ACT = (T[0] + T[1] + T[2] + T[3])/4.;
  
  // ascertain what the demand temperature is, and whether we need to turn the heater on
  if ((T_ACT < TEMPERATURE_ALARM_LO) || (T_ACT > TEMPERATURE_ALARM_HI)) {  // check for stupid input
    turnHeaterOn = false;
  } else {
    if (!USE_SERIAL_FOR_TEMPERATURE_SERVO) {                     // find DMD temperature using the potentiometer
      float threshV = 0.1;
      float diff = TEMPERATURE_ALARM_HI - TEMPERATURE_ALARM_LO;  // temperature range over which potentiometer ranges
      float vRange = 5.0 - threshV;                              // and the corresponding voltage range
      float scaling_f = diff/vRange;                             
      float V0 = (analogRead(A5)/1024.)*vRange;
      if (V0 > threshV) {                                        // the potentiometer has been turned clockwise..
          T_DMD = TEMPERATURE_ALARM_LO + (V0*scaling_f);         // ..so set the demand temperature
          turnHeaterOn = T_DMD - T_ACT > 0 ? true : false;       // and check if we need to turn the heater on
      } else {
          turnHeaterOn = false;
      }
    } else {
      String msg = readSerialInput();                            // read anything in the serial buffer
      if (msg.indexOf("SETP?") >= 0) { 
        if (IsNumeric(msg.substring(5))) {
          T_DMD = msg.substring(5).toFloat();
          turnHeaterOn = T_DMD - T_ACT > 0 ? true : false;       // check if we need to turn the heater on
        }
      }
    }
  }
  
  if ((millis() - lastSampleTime) >= (STATE_CHANGE_DELAY*1000.)) {
    // take action, if required
    if (turnHeaterOn && !isHeaterOnLast) {                         // turn heater on, when currently off
      digitalWrite(8, HIGH);
      digitalWrite(2, HIGH);
      digitalWrite(3, HIGH);
      isHeaterOn = true;
    } else if (!turnHeaterOn && isHeaterOnLast) {                  // turn heater off, when currently on
      digitalWrite(8, LOW);
      digitalWrite(2, LOW);
      digitalWrite(3, LOW); 
      isHeaterOn = false;
    }
    lastSampleTime += STATE_CHANGE_DELAY*1000.;
  }
  
  Serial.println((String)T[0] + '\t' + (String)T[1] + '\t' + 
                 (String)T[2] + '\t' + (String)T[3] + '\t' + 
                 (String)T_ACT + '\t' + (String)T_DMD + '\t' + 
                 + isHeaterOn + '\t' + turnHeaterOn + '\t' + 
                 (String)((STATE_CHANGE_DELAY*1000.)-(millis() - lastSampleTime)));

  isHeaterOnLast = isHeaterOn;
}



