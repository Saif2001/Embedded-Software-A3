//Saif Saleem H00272857
//This program is a FreeRTOS conversion of Embedded Software Assignment 2, with some additional changes according to labsheet specifications. 

#include <Arduino.h>

#define SIGBPULSELENGTH 50

//GPIO Pin Definitions
int msElapsedVar;
bool buttonState;
const int task2But = 32;
const int pot = 34;         
const int sigB = 5;
const int errorLED = 33;
const int t3Input = 17;

//Variables for use by multiple functions
int potValue;           
int task4Count = 0;     
int analogAvg = 0;     
float averagedVal = 0;
int oldAnalogVals[] = {0,0,0,0};
int error_code;
int nopCount;
float sqWaveDuration;
float sqWaveFreq;

//Queue and semaphore instantiation
static const uint8_t analogQueue_len = 4;
static QueueHandle_t analogQueue;
static SemaphoreHandle_t t9Sem;

//Struct for Task 9
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
      sqWaveFreq = (1/(sqWaveDuration)) * 1000000;   //Find frequency and correct
      }
  vTaskDelay(1000/portTICK_PERIOD_MS);
    }
  }

void task4(void * parameters) {
  for(;;){
      potValue = analogRead(pot);   //Read pot value and increment counter
      task4Count++;                 //Increment counter for use in Task 5
      vTaskDelay(42/portTICK_PERIOD_MS);
    }
  }

void task5(void * parameters) {
  for(;;){
    int valFromQueue = analogRead(pot);
  
    //Shift old values by 1 in array
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
  
    //Send averaged value to Task 7
    xQueueSend(analogQueue, (void *)&analogAvg, (TickType_t) 0);
    Serial.print("");     //An empty serial print is necessary for some reason. Program fails without it
    vTaskDelay(42/portTICK_PERIOD_MS);
    }
  }

void task6(void * parameters) {
  for(;;){
      nopCount = 0; //Can't use for loops. Counter used with while() instead
      while(nopCount < 1000) {
        __asm__ __volatile__ ("nop");
        nopCount++;     //Increment counter up to 1000
      }
    vTaskDelay(100/portTICK_PERIOD_MS);
    }
  }

void task7(void * parameters) {
  for(;;){

    int receivedAvg;
    //Receive analog average from Task 5
    xQueueReceive(analogQueue, (void *)&receivedAvg, (TickType_t) 10);

    //Full range is 4095
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
  //Actuate LED if over half the range value
  if(error_code == 1){
    digitalWrite(errorLED, HIGH);
  }
  //Else, ensure it is turned off
  if(error_code == 0){
    digitalWrite(errorLED, LOW);
    }
    vTaskDelay(333/portTICK_PERIOD_MS);
    }
  }


void task9(void * parameters) {
  for(;;){
 
//Struct variables updated
    stateInfo s1 = {.t2ButtonState = buttonState, .WaveFrequency = sqWaveFreq, .potAvg = analogAvg};
    
    //Button has to be pressed for Task 9
    //Print data from struct
    if(buttonState == 1){
      Serial.print("Button State: "); Serial.print(s1.t2ButtonState);
      Serial.println("");
      Serial.print("Wave Frequency: "); Serial.print(s1.WaveFrequency);
      Serial.println("");
      Serial.print("Averaged Analog Value: "); Serial.print(s1.potAvg);
      Serial.println("");
    }
    
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

  //Queue and semaphore creation
  analogQueue = xQueueCreate(analogQueue_len, sizeof(int));
                   
  t9Sem = xSemaphoreCreateCounting(1, 0);


    //Task creation 
    //Priorities approximately ranked by frequencies
    //Stack sizes optimised
    xTaskCreate(
      task1,  //Function name
      "Task 1", //Task name
      1700,     //Stack size
      NULL,     //Task Parameters
      1,          //Priority
      NULL      //Task handle
      );

      xTaskCreate(
      task2,  //Function name
      "Task 2", //Task name
      1600,     //Stack size
      NULL,     //Task Parameters
      2,          //Priority
      NULL      //Task handle
      );

      xTaskCreate(
      task3,  //Function name
      "Task 3", //Task name
      1600,     //Stack size
      NULL,     //Task Parameters
      3,          //Priority
      NULL      //Task handle
      );

            xTaskCreate(
      task4,  //Function name
      "Task 4", //Task name
      1400,     //Stack size
      NULL,     //Task Parameters
      1,          //Priority
      NULL      //Task handle
      );

            xTaskCreate(
      task5,  //Function name
      "Task 5", //Task name
      1600,     //Stack size
      NULL,     //Task Parameters
      1,          //Priority
      NULL      //Task handle
      );

            xTaskCreate(
      task6,  //Function name
      "Task 6", //Task name
      1600,     //Stack size
      NULL,     //Task Parameters
      2,          //Priority
      NULL      //Task handle
      );

            xTaskCreate(
      task7,  //Function name
      "Task 7", //Task name
      1600,     //Stack size
      NULL,     //Task Parameters
      3,          //Priority
      NULL      //Task handle
      );


            xTaskCreate(
      task8,  //Function name
      "Task 8", //Task name
      1600,     //Stack size
      NULL,     //Task Parameters
      3,          //Priority
      NULL      //Task handle
      );

      xTaskCreate(
      task9,  //Function name
      "Task 9", //Task name
      1600,     //Stack size
      NULL,     //Task Parameters
      1,          //Priority
      NULL      //Task handle
      );
      
      }

      void loop(){vTaskDelay(portMAX_DELAY);}
