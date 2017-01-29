#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 4 
#define USE_SERIAL_FOR_TEMPERATURE_SERVO true
#define PIN_RELAY_IN1 2
#define PIN_RELAY_IN2 3
#define PIN_POT 5
#define PIN_LED 8

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

boolean isHeaterOn, isHeaterOnLast, turnHeaterOn;
float T_DMD;
int TEMPERATURE_ALARM_LO = 10;
int TEMPERATURE_ALARM_HI = 28;
int POLLING_DELAY = 60;    // how often to poll the sensors and check for a heater state change
long lastSampleTime;
float T[4];

boolean IsNumeric(String str, boolean allowDecimal = false) {
  for(char i = 0; i < str.length(); i++) {
    if ((!isDigit(str.charAt(i))) || ((str.charAt(i) == '.') && (!allowDecimal))) {
      return false;
    } 
  }
  return true;
}

String readSerialInput(float timeout=1) {
  String msg;
  unsigned long start = millis();
  while (millis()-start < (timeout*1000)) {
    while (Serial.available() > 0) {
      char c = Serial.read();
      if (c=='\n') {
        Serial.flush();
        return msg;
      } else {
        msg+=c;
      }
    }
  }
  Serial.flush();
  return "";
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_RELAY_IN1, OUTPUT);
  pinMode(PIN_RELAY_IN2, OUTPUT);
  pinMode(PIN_POT, OUTPUT);   
  pinMode(PIN_LED, OUTPUT);
  
  digitalWrite(5, HIGH);
  
  T[0] = 0; T[1] = 0; T[2] = 0; T[3] = 0;
  isHeaterOn = false;
  isHeaterOnLast = false;
  turnHeaterOn = false;
  lastSampleTime = 0L - POLLING_DELAY;
  
  sensors.begin();
}

void loop() {
  // ascertain what the demand temperature is and set a demand (DMD) temperature
  turnHeaterOn = false;
  if (!USE_SERIAL_FOR_TEMPERATURE_SERVO) {                     // find DMD temperature using the potentiometer
    float threshV = 0.1;
    float diff = TEMPERATURE_ALARM_HI - TEMPERATURE_ALARM_LO;  // temperature range over which potentiometer ranges
    float vRange = 5.0 - threshV;                              // and the corresponding voltage range
    float scaling_f = diff/vRange;                             
    float V0 = (analogRead(A5)/1024.)*vRange;
    if (V0 > threshV) {                                        // the potentiometer has been turned clockwise..
      T_DMD = TEMPERATURE_ALARM_LO + (V0*scaling_f);           // ..so set the demand temperature
    }
  } else {
    String msg = readSerialInput();                            // read anything in the serial buffer
    if (msg.indexOf("SETSETPO?") >= 0) {                       // - set setpoint                    
      if (IsNumeric(msg.substring(9)), true) {
        T_DMD = msg.substring(9).toFloat();
      }
    } else if (msg.indexOf("GETSETPO?") >= 0) {                // - get setpoint
      Serial.println(T_DMD);
    } else if (msg.indexOf("GETTEMPC?") >= 0) {                // - get temperature
      if (IsNumeric(msg.substring(9))) {
        int index = msg.substring(9).toInt();
        Serial.println(T[index]);
      } 
    } else if (msg.indexOf("SETALALO?") >= 0) {                // - set alarm lo                   
      if (IsNumeric(msg.substring(9)), true) {
        TEMPERATURE_ALARM_LO = msg.substring(9).toFloat();
      }
    } else if (msg.indexOf("GETALALO?") >= 0) {                // - get alarm lo
      Serial.println(TEMPERATURE_ALARM_LO);
    } else if (msg.indexOf("SETALAHI?") >= 0) {                // - set alarm hi                  
      if (IsNumeric(msg.substring(9)), true) {
        TEMPERATURE_ALARM_HI = msg.substring(9).toFloat();
      }
    } else if (msg.indexOf("GETALAHI?") >= 0) {                // - get alarm hi
      Serial.println(TEMPERATURE_ALARM_HI);
    } else if (msg.indexOf("ISHEATON?") >= 0) {                // - is heater on?
      Serial.println(isHeaterOn);
    } else if (msg.indexOf("NEXTSTAT?") >= 0) {                // - when is the next state change due? (ms)
      Serial.println(((POLLING_DELAY)-((millis()/1000L) - lastSampleTime)));
    } 
  }

  // take action if it's time to poll
  if (((millis()/1000L) - lastSampleTime) >= POLLING_DELAY) {
    // get temperature (ACT)
    sensors.requestTemperatures();
    T[0] = sensors.getTempCByIndex(0);  
    T[1] = sensors.getTempCByIndex(1);  
    T[2] = sensors.getTempCByIndex(2);  
    T[3] = sensors.getTempCByIndex(3);   
    float T_ACT = (T[0] + T[1] + T[2] + T[3])/4.;
    
    // set the flag to turn the heater on if demand temperature is greater than actual
    if ((T_ACT > TEMPERATURE_ALARM_LO) && (T_ACT < TEMPERATURE_ALARM_HI)) {   
      turnHeaterOn = T_DMD - T_ACT > 0 ? true : false;             // check if we need to turn the heater on
    }
     
    if (turnHeaterOn && !isHeaterOnLast) {                         // turn heater on, when currently off
      digitalWrite(PIN_LED, HIGH);
      digitalWrite(PIN_RELAY_IN1, HIGH);
      digitalWrite(PIN_RELAY_IN2, HIGH);
      isHeaterOn = true;
    } else if (!turnHeaterOn && isHeaterOnLast) {                  // turn heater off, when currently on
      digitalWrite(PIN_LED, LOW);
      digitalWrite(PIN_RELAY_IN1, LOW);
      digitalWrite(PIN_RELAY_IN2, LOW); 
      isHeaterOn = false;
    }
    lastSampleTime += POLLING_DELAY;
    isHeaterOnLast = isHeaterOn;
  }
  
}



