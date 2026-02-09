const int m2  = 13;
const int m5  = 12;
const int m10 = 14;
const int p1  = 27;
const int p3  = 26;
const int p4  = 25;
const int p6  = 33;
const int p7  = 32;
const int p8  = 22;
const int p9  = 23;
const int term  = 34;
const int button1 = 19;
const int button2 = 21;
const int led1 = 18;
const int led2 = 5;

const int zero[]  = {p1, p3, p4, p6, p7, p8};
const int one[]   = {p3, p8};
const int two[]   = {p1, p3, p9, p6, p7};
const int three[] = {p1, p4, p9, p6, p7};
const int four[]  = {p8, p9, p6, p4};
const int five[]  = {p1, p4, p9, p8, p7};
const int six[]   = {p1, p3, p4, p9, p8, p7};
const int seven[] = {p4, p6, p7};
const int eight[] = {p1, p3, p4, p6, p7, p8, p9};
const int nine[]  = {p4, p6, p7, p8, p9};

const int* numbers[] = {zero, one, two, three, four, five, six, seven, eight, nine};
const int sizes[] = {6, 2, 5, 5, 4, 5, 6, 3, 7, 5};

const int delayTime = 1;
volatile int number = 0;
const int stepsPerDegree = 33;
const int sensorOffset = 4550;
volatile bool b1Pressed = false;
volatile bool b2Pressed = false;
volatile unsigned long lastInterrupt1 = 0;
volatile unsigned long lastInterrupt2 = 0;

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
  int hundreds = number / 100;
  if (hundreds > 0)
  {
    clearPines();
    GPIO.out_w1tc = 1 << m2;
    for (int i = 0; i < sizes[hundreds]; i++)
    {
      setPin(numbers[hundreds][i]);
    }
  }
  delay(delayTime);
  
  int tens = (number / 10) % 10;
  clearPines();
  if (hundreds >= 1 || tens > 0)
  {
    GPIO.out_w1tc = 1 << m10;
    for (int i = 0; i < sizes[tens]; i++)
    {
     setPin(numbers[tens][i]);
    }
  }
  delay(delayTime);
  
  int ones = number % 10;
  clearPines();
  GPIO.out_w1tc = 1 << m5;
  for (int i = 0; i < sizes[ones]; i++)
  {
    setPin(numbers[ones][i]);
  }
  delay(delayTime);
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
void button1Press() 
{
  unsigned long now = millis();

  if (now - lastInterrupt1 > 50) 
  {
    b1Pressed = !b1Pressed;
    b2Pressed = false;
    lastInterrupt1 = now;
  }
}

//-------------------------------------------------------------
void button2Press() 
{
  unsigned long now = millis();

  if (now - lastInterrupt1 > 50) 
    {
    b2Pressed = !b2Pressed;
    b1Pressed = false;
    lastInterrupt2 = now;
  }
}

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

  attachInterrupt(digitalPinToInterrupt(button1), button1Press, FALLING);
  attachInterrupt(digitalPinToInterrupt(button2), button2Press, FALLING);
  
  Serial.begin(115200);
}

void loop() 
{
    //-- ask sensor
    int sensor = analogRead(term);
    number = abs(sensorOffset - sensor) / stepsPerDegree;

    //-- ask buttons
    if (b1Pressed)
    {
      GPIO.out_w1ts = 1 << led1;
    }
    else
    {
      GPIO.out_w1tc = 1 << led1;
    }

    if (b2Pressed)
    {
      GPIO.out_w1ts = 1 << led2;
    }
    else
    {
      GPIO.out_w1tc = 1 << led2;
    }

}
