/*************************************************************************
  Includes
*************************************************************************/

#include <DualVNH5019MotorShield.h>

/*************************************************************************
  Global Variables + Defines
*************************************************************************/

DualVNH5019MotorShield md;
#define WATER_SENSOR_PIN A5
#define CURRENT_LIMIT_MA 5000

/*************************************************************************
  State Machine
*************************************************************************/

typedef enum
{
  STATE_EXTENDING,
  STATE_RETRACTING,
  STATE_EXTENDED,
  STATE_RETRACTED

} SystemState;

SystemState CurrentState = STATE_RETRACTED;

/*************************************************************************
  Actuator Functions
*************************************************************************/

void extendActuator(void)
{
  md.setM1Speed(-400);
}

void retractActuator(void)
{
  md.setM1Speed(400);
}

void stopActuator(void)
{
  md.setM1Speed(0);
}

int getActuatorCurrent(void)
{
  Serial.println(abs(md.getM1CurrentMilliamps()));
  return abs(md.getM1CurrentMilliamps());
}

/*************************************************************************
  Water Sensor Functions
*************************************************************************/

bool waterSensorIsOn(void)
{
  int sensorValue = analogRead(WATER_SENSOR_PIN);

  if (sensorValue > 512)
    return true;
  else
    return false;
}

bool waterSensorIsOnLong(void)
{
  bool waterDetected = false;
 
  for (int i = 0; i < 30000; i++)
  {
    int sensorValue = analogRead(WATER_SENSOR_PIN);

    if (sensorValue > 512)
      waterDetected = true;

    delay(1);
  }

  if (waterDetected == true)
    return true;
  else
    return false;
}

/*************************************************************************
  Setup
*************************************************************************/

void setup()
{
  Serial.begin(115200);
  md.init();
  pinMode(WATER_SENSOR_PIN, INPUT);
}

/*************************************************************************
  Loop
*************************************************************************/

void loop()
{
  delay(200);

  switch (CurrentState)
  {
    case STATE_RETRACTED:
      {
        if (waterSensorIsOn() == true)
        {
          extendActuator();
          CurrentState = STATE_EXTENDING;
          Serial.println("Current State: Extending");
        }
      }
      break;
    case STATE_EXTENDING:
      {
        if (getActuatorCurrent() > CURRENT_LIMIT_MA || getActuatorCurrent() < 50)
        {
          stopActuator();
          CurrentState = STATE_EXTENDED;
          Serial.println("Current State: Extended");
        }
      }
      break;
    case STATE_EXTENDED:
      {
        if (waterSensorIsOnLong() == false)
        {
          retractActuator();
          CurrentState = STATE_RETRACTING;
          Serial.println("Current State: Retracting");
        }
      }
      break;
    case STATE_RETRACTING:
      {
        if (getActuatorCurrent() > CURRENT_LIMIT_MA || getActuatorCurrent() < 50)
        {
          stopActuator();
          CurrentState = STATE_RETRACTED;
          Serial.println("Current State: Retracted");
        }
      }
      break;
  }
}
