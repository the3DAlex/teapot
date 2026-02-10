#define DEBUG
const uint8_t m2  = 13;
const uint8_t m5  = 12;
const uint8_t m10 = 14;
const uint8_t p1  = 27;
const uint8_t p3  = 26;
const uint8_t p4  = 25;
const uint8_t p6  = 33;
const uint8_t p7  = 32;
const uint8_t p8  = 22;
const uint8_t p9  = 23;

const uint8_t term  = 35;
const uint8_t src  = 17;
const uint8_t rel  = 4;
const uint8_t buzz  = 16;

const uint8_t button1 = 19;
const uint8_t button2 = 21;
const uint8_t led1 = 18;
const uint8_t led2 = 5;

const uint8_t zero[]  = {p1, p3, p4, p6, p7, p8};
const uint8_t one[]   = {p3, p8};
const uint8_t two[]   = {p1, p3, p9, p6, p7};
const uint8_t three[] = {p1, p4, p9, p6, p7};
const uint8_t four[]  = {p8, p9, p6, p4};
const uint8_t five[]  = {p1, p4, p9, p8, p7};
const uint8_t six[]   = {p1, p3, p4, p9, p8, p7};
const uint8_t seven[] = {p4, p6, p7};
const uint8_t eight[] = {p1, p3, p4, p6, p7, p8, p9};
const uint8_t nine[]  = {p4, p6, p7, p8, p9};

const uint8_t* numbers[] = {zero, one, two, three, four, five, six, seven, eight, nine};
const uint8_t sizes[] = {6, 2, 5, 5, 4, 5, 6, 3, 7, 5};


const uint8_t stepsPerDegree = 33;
const uint16_t sensorOffset = 4550;
bool led1Status = false;
bool led2Status = false;
bool relStatus = false;
bool srcStatus = false;
bool buzzStatus = false;
volatile unsigned long lastInterrupt1 = 0;
volatile unsigned long lastInterrupt2 = 0;

enum class State :uint8_t
{
  Idle = 0,
  ShowTemperature,
  SetTemprature,
  PrepareBoiling,
  Boiling,
  FinishBoiling
};

State state = State::ShowTemperature;
bool showTemperature = true;
uint8_t temperature = 0;
uint8_t desiredTemperature = 0;
unsigned long before = 0;
unsigned long debugOutput = 0;
int b1PressedTimes = 0;
const uint8_t temperatures[] = {40, 50, 60, 70};
const uint8_t predefTemperaturesCount = 4;
uint8_t selectedTemperature = 0;

//-------------------------------------------------------------
void clearPines()
{
  GPIO.out_w1ts = 1 << m2;
  GPIO.out_w1ts = 1 << m5;
  GPIO.out_w1ts = 1 << m10;
  GPIO.out_w1tc = 1 << p1;
  GPIO.out_w1tc = 1 << p3;
  GPIO.out_w1tc = 1 << p4;
  GPIO.out1_w1tc.val = 1 << (p6 - 32);
  GPIO.out1_w1tc.val = 1 << (p7 - 32);
  GPIO.out_w1tc = 1 << p8;
  GPIO.out_w1tc = 1 << p9;
}

//-------------------------------------------------------------
void setPin(int pin)
{
  if (pin < 32)
  {
    GPIO.out_w1ts = 1 << pin;
  }
  else
  {
    GPIO.out1_w1ts.val = 1 << (pin - 32);
  }
}

//-------------------------------------------------------------
void drawNumber()
{
  if (!showTemperature)
  {
    return;
  }
  
  int hundreds = temperature / 100;
  if (hundreds > 0)
  {
    clearPines();
    GPIO.out_w1tc = 1 << m2;
    for (int i = 0; i < sizes[hundreds]; i++)
    {
      setPin(numbers[hundreds][i]);
    }
  }
  delay(1);
  
  int tens = (temperature / 10) % 10;
  clearPines();
  if (hundreds >= 1 || tens > 0)
  {
    GPIO.out_w1tc = 1 << m10;
    for (int i = 0; i < sizes[tens]; i++)
    {
     setPin(numbers[tens][i]);
    }
  }
  delay(1);
  
  int ones = temperature % 10;
  clearPines();
  GPIO.out_w1tc = 1 << m5;
  for (int i = 0; i < sizes[ones]; i++)
  {
    setPin(numbers[ones][i]);
  }
  delay(1);
}

//-------------------------------------------------------------
void myTask(void *pvParameters) 
{
  while (true) 
  {
    drawNumber();
    vTaskDelay(1);
  }
}

//-------------------------------------------------------------
void askSensor()
{
  int sensor = analogRead(term);
  temperature = abs(sensorOffset - sensor) / stepsPerDegree;
}

//-------------------------------------------------------------
bool askB1()
{
  return !digitalRead(button1);
}

//-------------------------------------------------------------
bool askB2()
{
  return !digitalRead(button2);
}

//-------------------------------------------------------------
void led1On() { GPIO.out_w1ts = 1 << led1; led1Status = true;}
void led1Off(){ GPIO.out_w1tc = 1 << led1; led1Status = false;}
void led2On() { GPIO.out_w1ts = 1 << led2; led2Status = true;}
void led2Off(){ GPIO.out_w1tc = 1 << led2; led2Status = false;}
void srcOn()  { GPIO.out_w1ts = 1 << src; srcStatus = true;}
void srcOff() { GPIO.out_w1tc = 1 << src; srcStatus = false;}
void relOn()  { GPIO.out_w1ts = 1 << rel; relStatus = true;}
void relOff() { GPIO.out_w1tc = 1 << rel; relStatus = false;}
void buzzOn() { GPIO.out_w1ts = 1 << buzz; buzzStatus = true;}
void buzzOff(){ GPIO.out_w1tc = 1 << buzz; buzzStatus = false;}

//-------------------------------------------------------------
void setup() 
{
  pinMode(m2, OUTPUT);
  pinMode(m5, OUTPUT);
  pinMode(m10, OUTPUT);
  pinMode(p1, OUTPUT);
  pinMode(p3, OUTPUT);
  pinMode(p4, OUTPUT);
  pinMode(p6, OUTPUT);
  pinMode(p7, OUTPUT);
  pinMode(p8, OUTPUT);
  pinMode(p9, OUTPUT);
  pinMode(term, INPUT);
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  
  xTaskCreatePinnedToCore(myTask, "myTask", 4096, NULL, 1, NULL, 0);

#ifdef DEBUG:  
  Serial.begin(115200);
#endif
}

//-------------------------------------------------------------
void loop() 
{
    unsigned long now = millis();
    
    switch (state)
    {
      case State::Idle:
      {
        if (askB1() || askB2())
        {
            state = State::ShowTemperature;
            before = now;
        }
        break;
      }
      case State::ShowTemperature:
      {
        if (now - before < 5000)
        {
          askSensor();
          showTemperature = true;
          if (now - before > 1000)
          { 
            if (askB1())
            {
              state = State::PrepareBoiling;
              desiredTemperature = 100;
              led1On();
              before = now;
            }
            else
            {
              if (askB2())
              {
                state = State::SetTemprature;
                before = now;
              }
            }
          }
        }
        else
        {
          showTemperature = false;
          before = now;
          state = State::Idle;
        }
        break;
      }
      case State::SetTemprature:
      {
        desiredTemperature = temperatures[selectedTemperature];
        temperature = temperatures[selectedTemperature];
        showTemperature = true;
        
        if (now - before > 1000)
        {
          if (askB2())
          {
            selectedTemperature ++;
            if (selectedTemperature >= predefTemperaturesCount)
            {
              selectedTemperature = 0;
            }
            before = now;
          }
        }
        
        if (now - before > 5000)
        {
          state = State::PrepareBoiling;
          led1Off();
          led2On();
          before = now;
        } 
        break;
      }
      case State::PrepareBoiling:
      {
        buzzOn();
        srcOn();
        if (now - before > 1000)
        {
          state = State::Boiling;
          relOn();
          before = now;
          buzzOff();
        }
        break;
      }
      case State::Boiling:
      {
        askSensor();
        showTemperature = true;
 
        if (askB1())
        {
          b1PressedTimes ++;
        }
        else
        {
          b1PressedTimes = 0;
        }
        
        if (temperature >= desiredTemperature || b1PressedTimes > 10000)
        {
          state = State::FinishBoiling;
          relOff();
          before = now;
          b1PressedTimes = 0;
        }
        
        break;
      }
      case State::FinishBoiling:
      {
        buzzOn();
        if (now - before > 1000)
        {
          desiredTemperature = 0;
          state = State::ShowTemperature;
          srcOff();
          led1Off();
          led2Off();
          buzzOff();
          before = now;
        }
        break;
      }
    }

#ifdef DEBUG:
    if (now - debugOutput > 1000)
    {
      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.print(" : ");
      Serial.print(showTemperature);
      Serial.print(" || Desired Temperature: ");
      Serial.print(desiredTemperature);
      Serial.print(" || b1 Pressed Times: ");
      Serial.print(b1PressedTimes);
      Serial.print(" || src: ");
      Serial.print(srcStatus);
      Serial.print(" || rel: ");
      Serial.print(relStatus);
      Serial.print(" || led1: ");
      Serial.print(led1Status);
      Serial.print(" || led2: ");
      Serial.print(led2Status);
      Serial.print(" || buzz: ");
      Serial.print(buzzStatus);
      Serial.print(" || state: ");
      switch (state)
      {
        case State::Idle:
        {
          Serial.println("Idle");
          break;
        }
        case State::ShowTemperature:
        {
          Serial.println("ShowTemperature");
          break;
        }
        case State::SetTemprature:
        {
          Serial.println("SetTemprature");
          break;
        }
        case State::PrepareBoiling:
        {
          Serial.println("PrepareBoiling");
          break;
        }
        case State::Boiling:
        {
          Serial.println("Boiling");
          break;
        }
        case State::FinishBoiling:
        {
          Serial.println("FinishBoiling");
          break;
        }
      }
      debugOutput = now;
    }
#endif

}
