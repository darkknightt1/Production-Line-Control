#include "RTOS.h"


void setup()
{
  Serial.begin(9600);
  /*intializing the tasks*/
  xTaskCreate(Homing , "Task1", 300, NULL, 2, &Task_HandleHoming);


  xTaskCreate(Shooting , "Task4", 300 , NULL, 2, &Task_HandleShooting);


  xTaskCreate(Packaging , "Task6", 400, NULL, 2, &Task_HandlePackaging);


  pinMode(X_LIMIT_SWITCH, INPUT_PULLUP);
  pinMode(Y_LIMIT_SWITCH, INPUT_PULLUP);
  pinMode(Z_LIMIT_SWITCH, INPUT_PULLUP);

  pinMode(StepperX_CLK, OUTPUT);
  pinMode(StepperY_CLK, OUTPUT);
  pinMode(StepperZ_CLK, OUTPUT);

  pinMode(StepperX_DIR, OUTPUT);
  pinMode(StepperY_DIR, OUTPUT);
  pinMode(StepperZ_DIR, OUTPUT);


  pinMode(Sorting_IR, INPUT);
  pinMode(Sorting_Proximity, INPUT);

  pinMode(Shooting_Solenoid_Relay, OUTPUT);
  pinMode(Shooting_IR, INPUT);


  pinMode(Suction_Solenoid_Relay, OUTPUT);
  digitalWrite(Shooting_Solenoid_Relay, HIGH);


  PackagingSemaphore = xSemaphoreCreateBinary();

  HomingSemaphore = xSemaphoreCreateBinary();


  ShootingSemaphore= xSemaphoreCreateBinary();
  xSemaphoreGive(HomingSemaphore );
  ETHERNET_init();
}

void loop()
{
  // nothing written here
}
void vApplicationIdleHook(void)
{
  Serial.println("Idle");
}
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
  Serial.println("ovf");
}
void vApplicationMallocFailedHook(void)
{ 
  Serial.println("malloc");
}
extern "C"
{
  void vApplicationIdleHook(void);
  void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName);
  void vApplicationMallocFailedHook(void);
  
}
void Homing(void *paramter)
{
  (void) paramter;
  while (1)
  {
    xSemaphoreTake(HomingSemaphore, portMAX_DELAY);
    Serial.println("Z-H-start");
    digitalWrite(StepperZ_DIR, Z_HOMING_DIR);
    STATE = 'O';
    ETHERNET_send(STATE);delay(5);ETHERNET_send(STATE);
    while (digitalRead(Z_LIMIT_SWITCH) == LimitSwitchUnPressed )
    {
      digitalWrite(StepperZ_CLK, HIGH);
      delayMicroseconds(15);//vTaskDelay(Steppers_Delay);
      digitalWrite(StepperZ_CLK, LOW);
      delayMicroseconds(15);//vTaskDelay(Steppers_Delay);
    }
    Serial.println("Z-H-finished");
    STATE = 'O';
    ETHERNET_send(STATE);delay(5);ETHERNET_send(STATE);
    digitalWrite(StepperX_DIR, X_HOMING_DIR);
    while (digitalRead(X_LIMIT_SWITCH) == LimitSwitchUnPressed )//this is the start of homing
    {
      digitalWrite(StepperX_CLK, HIGH);
      delayMicroseconds(35);//vTaskDelay(Steppers_Delay);
      digitalWrite(StepperX_CLK, LOW);
      delayMicroseconds(35);//vTaskDelay(Steppers_Delay);
    }
    Serial.println("X-finished");
    STATE = 'O';
    ETHERNET_send(STATE);delay(5);ETHERNET_send(STATE);
    digitalWrite(StepperY_DIR, Y_HOMING_DIR);
    while (digitalRead(Y_LIMIT_SWITCH) == LimitSwitchUnPressed )
    {
      digitalWrite(StepperY_CLK, HIGH);
      delayMicroseconds(15);// vTaskDelay(Steppers_Delay);
      digitalWrite(StepperY_CLK, LOW);
      delayMicroseconds(15);// vTaskDelay(Steppers_Delay);
    }
    Serial.println("Y-H-finished");

    xSemaphoreGive(ShootingSemaphore);
    vTaskDelay(1);
  }

}

void Shooting(void *paramter)
{
  (void) paramter;
  while (1)
  {
    Serial.println("in shooting");
    xSemaphoreTake(ShootingSemaphore, portMAX_DELAY);
    while ( digitalRead(Shooting_IR) == IR_Not_Detect );
    while (digitalRead(Sorting_IR) == IR_Not_Detect)
    {
      STATE = 'F';
      ETHERNET_send(STATE);//delay(2);ETHERNET_send(STATE);
      Serial.println("inin shooting");
      digitalWrite(Shooting_Solenoid_Relay, LOW);
      delay(350);//vTaskDelay(30);
      digitalWrite(Shooting_Solenoid_Relay, HIGH);
      delay(1350);
    }

    
    if (MetalCounter == 3 && Non_MetalCounter == 3 )
    {
      Serial.println("finished");
      vTaskSuspendAll();
      while (1)
      {
        STATE = 'Z';
        //ETHERNET_send(STATE);
      }
    }
    Serial.println("in insorting");
    STATE = 'S';
    ETHERNET_send(STATE);//delay(2);ETHERNET_send(STATE);
    if (digitalRead(Sorting_Proximity) == METAL)//if metal
    {
      MetalCounter++;
      Material_State = METAL;
      flag = 1;
      Serial.print(MetalCounter);
      Serial.println(" METAL");
    }
    else
    {
      Non_MetalCounter++;
      Material_State = NON_METAL;
      flag = 0;
      Serial.print(Non_MetalCounter);
      Serial.println(" NONMETAL");
    }


    zsteps = 0;
    digitalWrite(StepperZ_DIR, Z_PACKAGING_DIR);
    while (zsteps < 69000) //go to the product
    {
      digitalWrite(StepperZ_CLK, HIGH);
      delayMicroseconds(15);//vTaskDelay(Steppers_Delay);
      digitalWrite(StepperZ_CLK, LOW);
      delayMicroseconds(15);//vTaskDelay(Steppers_Delay);
      zsteps++;
    }
    delay(350);//vTaskDelay(60);
    digitalWrite(Suction_Solenoid_Relay, HIGH); //hold it
    delay(350);//vTaskDelay(50);
    digitalWrite(StepperZ_DIR, Z_HOMING_DIR);//go to Z-Home
    zsteps = 0;

    while (zsteps < 38000) //may change
    {
      digitalWrite(StepperZ_CLK, HIGH);
      delayMicroseconds(15);//vTaskDelay(Steppers_Delay);
      digitalWrite(StepperZ_CLK, LOW);
      delayMicroseconds(15);//vTaskDelay(Steppers_Delay);
      zsteps++;
    }

    xSemaphoreGive(PackagingSemaphore);
    vTaskDelay(1);//to get out of the function if it is not needed
  }
}




void Packaging(void *paramter)
{
  (void) paramter;
  while (1)
  {
    xSemaphoreTake(PackagingSemaphore, portMAX_DELAY);
    Serial.println("X-P-Start");
    STATE = 'P';
    ETHERNET_send(STATE);//delay(2);ETHERNET_send(STATE);

    long long dest = 0;
    if (Material_State == METAL)
      dest = METAL_PRODUCT_STEPS [MetalCounter][0];

    else
      dest = NON_METAL_PRODUCT_STEPS [Non_MetalCounter][0];

    xsteps = 0;
    digitalWrite(StepperX_DIR, X_PACKAGING_DIR);
    while ( xsteps <  dest )
    {
      digitalWrite(StepperX_CLK, HIGH);
      delayMicroseconds(35);//vTaskDelay(Steppers_Delay);
      digitalWrite(StepperX_CLK, LOW);
      delayMicroseconds(35);//vTaskDelay(Steppers_Delay);
      xsteps++;
    }
    Serial.println("X-P-finished");

    Serial.println("Y-P-Start");
    dest = 0;
    if (Material_State == 0) dest = METAL_PRODUCT_STEPS [MetalCounter][1];

    else  dest = NON_METAL_PRODUCT_STEPS [Non_MetalCounter][1];

    ysteps = 0;
    digitalWrite(StepperY_DIR, Y_PACKAGING_DIR);

    while (ysteps < dest )
    {
      digitalWrite(StepperY_CLK, HIGH);
      delayMicroseconds(15);//vTaskDelay(Steppers_Delay);
      digitalWrite(StepperY_CLK, LOW);
      delayMicroseconds(15);//vTaskDelay(Steppers_Delay);
      ysteps++;
    }
    Serial.println("Y-P-finished");
    Serial.println("Z-P-Start");
    zsteps = 0;
    digitalWrite(StepperZ_DIR, Z_PACKAGING_DIR);


    while (zsteps < 33000)
    {
      digitalWrite(StepperZ_CLK, HIGH);
      delayMicroseconds(15);//vTaskDelay(Steppers_Delay);
      digitalWrite(StepperZ_CLK, LOW);
      delayMicroseconds(15);//vTaskDelay(Steppers_Delay);
      zsteps++;
    }

    if (flag == 1) STATE = 'M';
    else STATE = 'N';
    ETHERNET_send(STATE);//delay(2);ETHERNET_send(STATE);
    delay(350);//vTaskDelay(60);
    digitalWrite(Suction_Solenoid_Relay , LOW); // release the product
    delay(350);//vTaskDelay(50);

    xSemaphoreGive(HomingSemaphore);

    Serial.println("Z-P-finished");
    vTaskDelay(1);
  }
}
