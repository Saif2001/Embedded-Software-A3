
#include <Arduino.h>

#define SIGBPULSELENGTH 50

int msElapsedVar;
bool buttonState;
const int task2But = 32;
const int pot = 34;         //Pin definitions
const int sigB = 5;
const int errorLED = 33;
const int t3Input = 17;

int potValue;           
int task4Count = 0;     
int analogAvg = 0;     
float averagedVal = 0;
int oldAnalogVals[] = {0,0,0,0};
int error_code;
int nopCount;
float sqWaveDuration;
float sqWaveFreq;

static const uint8_t msg_queue_len = 4;
static QueueHandle_t msg_queue;
static SemaphoreHandle_t t9Sem;


  struct stateInfo {
    int t2ButtonState;
    float WaveFrequency;
    float potAvg;
    
    };

    
void task1(void * parameters) {
  for(;;){
  digitalWrite(sigB, HIGH);       //Function to set Signal B high for 50 micro-s
  delayMicroseconds(SIGBPULSELENGTH);
  digitalWrite(sigB, LOW);
  vTaskDelay(23/portTICK_PERIOD_MS);
    }
 
  }


void task2(void * parameters) {
  for(;;){
    buttonState = digitalRead(task2But);  //Read button state
    vTaskDelay(200/portTICK_PERIOD_MS);
    }
  }

void task3(void * parameters) {
  for(;;){
    sqWaveDuration = pulseIn(t3Input, LOW, 2500) * 2;    //Find pulse duration
    if(sqWaveDuration != 0){        //Prevents Div0 Errors
    sqWaveFreq = (1/(sqWaveDuration)) * 1000000 * 1.06;   //Find frequency and correct
  }
  vTaskDelay(1000/portTICK_PERIOD_MS);
    }
  }

void task4(void * parameters) {
  for(;;){
      potValue = analogRead(pot);   //Read pot value and increment counter
      task4Count++;
      vTaskDelay(42/portTICK_PERIOD_MS);
    }
  }

void task5(void * parameters) {

  for(;;){

    
  int valFromQueue = analogRead(pot);

  
  //if(xQueueReceive(msg_queue, (void *)&valFromQueue, 0) == pdTRUE) {
      //Serial.println(valFromQueue);
  //  }
  

if((task4Count % 4) == 0){
  oldAnalogVals[3] = valFromQueue;
  }
  if((task4Count % 4) == 1){
  oldAnalogVals[2] = oldAnalogVals[3];
  }
  if((task4Count % 4) == 2){
  oldAnalogVals[1] = oldAnalogVals[2];
  }
  if((task4Count % 4) == 3){
  oldAnalogVals[0] = oldAnalogVals[1];
  }

  //Find averaged (filtered) value
  analogAvg = (oldAnalogVals[0] + oldAnalogVals[1] + oldAnalogVals[2] + oldAnalogVals[3])/4;

  xQueueSend(msg_queue, (void *)&analogAvg, (TickType_t) 0);
  //Serial.println(analogAvg);
 
  vTaskDelay(42/portTICK_PERIOD_MS);

  
    }
  }

void task6(void * parameters) {
  for(;;){
      nopCount = 0; //Can't use for loops. Counter used with while() instead
  while(nopCount < 1000) {
    //Serial.println("EXECUTING NOP");
    __asm__ __volatile__ ("nop");
    nopCount++;     //Increment counter up to 1000
  }
  vTaskDelay(100/portTICK_PERIOD_MS);
    }
  }

void task7(void * parameters) {
  for(;;){

  int receivedAvg;

  xQueueReceive(msg_queue, (void *)&receivedAvg, (TickType_t) 5);
Serial.println(receivedAvg);
    
  if(receivedAvg >= 2048) {
    error_code = 1;
    }
  else if (receivedAvg <= 2047){
    error_code = 0;
    }
    vTaskDelay(333/portTICK_PERIOD_MS);
    }
  }

void task8(void * parameters) {
  for(;;){
  if(error_code == 1){
    digitalWrite(errorLED, HIGH);
  }
  if(error_code == 0){
    digitalWrite(errorLED, LOW);
    }
    vTaskDelay(333/portTICK_PERIOD_MS);
    }
  }


void task9(void * parameters) {
  for(;;){

  
  //Serial.println("");
  //Serial.print("Digital Button State,");    //Table headings
  //Serial.print("\t\t");   //Necessary to space columns
  //Serial.print("Frequency (Hz),");
  //Serial.print("\t\t");
  //Serial.print("Average Potentiometer Value,");
  //Serial.println("");
  //Serial.print(buttonState);    //Variable values
  //Serial.print(",\t\t\t\t");    //Additional spacing as necessary
  //Serial.print(sqWaveFreq);
  //Serial.print(",\t\t\t");
  //Serial.print(analogAvg);
  //Serial.print(",");
 

stateInfo s1 = {.t2ButtonState = buttonState, .WaveFrequency = sqWaveFreq, .potAvg = analogAvg};


//sprintf(buf, "%d", s1.potAvg);

//Serial.println(strcat("Button State: %d", sprintf(buf, "%d", s1.potAvg)));

Serial.print("Button State: "); Serial.print(s1.t2ButtonState);
Serial.println("");
Serial.print("Wave Frequency: "); Serial.print(s1.WaveFrequency);
Serial.println("");
Serial.print("Averaged Analog Value"); Serial.print(s1.potAvg);
Serial.println("");


  
  vTaskDelay(5000/portTICK_PERIOD_MS);
    }
  }
  

  void setup(){

  pinMode(task2But, INPUT); //Pin mode definitions
  pinMode(pot, INPUT);
  pinMode(errorLED, OUTPUT);
  pinMode(sigB, OUTPUT);
  pinMode(t3Input, INPUT);
  
  Serial.begin(9600);


  msg_queue = xQueueCreate(msg_queue_len, sizeof(int));


                          
  t9Sem = xSemaphoreCreateCounting(1, 0);

    
    xTaskCreate(
      task1,  //Function name
      "Task 1", //Task name
      2000,     //Stack size
      NULL,     //Task Parameters
      1,          //Priority
      NULL      //Task handle
      );

      xTaskCreate(
      task2,  //Function name
      "Task 2", //Task name
      2000,     //Stack size
      NULL,     //Task Parameters
      2,          //Priority
      NULL      //Task handle
      );

      xTaskCreate(
      task3,  //Function name
      "Task 3", //Task name
      2000,     //Stack size
      NULL,     //Task Parameters
      3,          //Priority
      NULL      //Task handle
      );

            xTaskCreate(
      task4,  //Function name
      "Task 4", //Task name
      2000,     //Stack size
      NULL,     //Task Parameters
      1,          //Priority
      NULL      //Task handle
      );

            xTaskCreate(
      task5,  //Function name
      "Task 5", //Task name
      2000,     //Stack size
      NULL,     //Task Parameters
      1,          //Priority
      NULL      //Task handle
      );

            xTaskCreate(
      task6,  //Function name
      "Task 6", //Task name
      2000,     //Stack size
      NULL,     //Task Parameters
      2,          //Priority
      NULL      //Task handle
      );

            xTaskCreate(
      task7,  //Function name
      "Task 7", //Task name
      2000,     //Stack size
      NULL,     //Task Parameters
      3,          //Priority
      NULL      //Task handle
      );


            xTaskCreate(
      task8,  //Function name
      "Task 8", //Task name
      2000,     //Stack size
      NULL,     //Task Parameters
      3,          //Priority
      NULL      //Task handle
      );

      xTaskCreate(
      task9,  //Function name
      "Task 9", //Task name
      2000,     //Stack size
      NULL,     //Task Parameters
      1,          //Priority
      NULL      //Task handle
      );
      
      }

      void loop(){vTaskDelay(portMAX_DELAY);}
