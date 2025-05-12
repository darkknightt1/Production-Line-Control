#include <Arduino.h>

#include <Arduino_FreeRTOS.h>
#include <semphr.h>

#include "Ethernet.h"
/**********************************************************************************************************************************************************************************************************************/
#define X_LIMIT_SWITCH 32
#define Y_LIMIT_SWITCH 34
#define Z_LIMIT_SWITCH 36

#define StepperX_CLK 22
#define StepperY_CLK 24
#define StepperZ_CLK 26

#define StepperX_DIR 23
#define StepperY_DIR 25
#define StepperZ_DIR 27

#define Sorting_IR  38
#define Shooting_IR 40
#define Sorting_Proximity 46

#define Suction_Solenoid_Relay  44
#define Shooting_Solenoid_Relay 33
/**********************************************************************************************************************************************************************************************************************/

#define X_PACKAGING_DIR 0
#define X_HOMING_DIR    1

#define Y_PACKAGING_DIR 1
#define Y_HOMING_DIR    0

#define Z_PACKAGING_DIR 1
#define Z_HOMING_DIR    0

#define METAL 0
#define NON_METAL 1

#define Steppers_Delay 0.1

#define LimitSwitchPressed 0
#define LimitSwitchUnPressed 1

#define IR_Detect  0
#define IR_Not_Detect 1
#define HOMING_STATE    'H'
#define SHOOTING_STATE  'F'
#define SORTING_STATE   'S'
#define PACKAGING_STATE 'P'
#define METAL_STATE     'M'
#define NONMETAL_STATE  'N'


/**********************************************************************************************************************************************************************************************************************/
/*creating task handler*/   //used to get information to and from the task ,ex: increase task priority , task suspend,...
 TaskHandle_t Task_HandleHoming;
 TaskHandle_t Task_HandlePackaging;


 TaskHandle_t Task_HandleShooting;
// TaskHandle_t Task_HandleSorting;
 
/*creating task semaphore*/
 SemaphoreHandle_t PackagingSemaphore;

  
 SemaphoreHandle_t HomingSemaphore;

 
 //SemaphoreHandle_t SortingSemaphore;
 SemaphoreHandle_t ShootingSemaphore;
/**********************************************************************************************************************************************************************************************************************/
/*global variables*/
volatile int MetalCounter     = -1;
volatile int Non_MetalCounter = -1;
volatile int Material_State = 250;

volatile char STATE = 'U' ;
volatile char flag =0;
long long xsteps=0;
long long ysteps=0;
long long zsteps=0;

long long METAL_PRODUCT_STEPS[4][3]=
{
  {3500    ,200000, 66000}, //product 1
  {3500    ,375000, 66000}, //product 2
  {25000,200000, 66000}, //product 3
  {25000,375000, 66000}  //product 4
};

long long NON_METAL_PRODUCT_STEPS[4][3]=
{
  {3500  ,580000,66000}, //product 1
  {3500  ,765000, 66000}, //product 2
  {25000,580000 ,66000}, //product 3
  {25000,765000, 66000}  //product 4
};

/**********************************************************************************************************************************************************************************************************************/

/*creating tasks"funtions"*/
void Homing(void *paramter);
//void Y_Homing(void *paramter);
//void Z_Homing(void *paramter);

void Shooting(void *paramter);
//void Sorting(void *paramter);

void Packaging(void *paramter);
//void Y_Packaging(void *paramter);
//void Z_Packaging(void *paramter);

//void Ethernet_Communication(void *paramter);
