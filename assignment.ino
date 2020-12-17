

//There are six functions defined here.
//makeconnection() - For wifi connection
//sdcardconnect() - For SD connection
//makeHTTPRequest() - For taking the json webpage 
//savetosd() - saving the json file to sd card 
//parsejson()- parsing the json file from sd card

Tasks
//searching the key
//finding query time



//Core
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif


#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <mySD.h>
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

//Wifi
const char ssid[] = "SecondFloor";       
const char password[] = "DLink1234";



File root;

#define TEST_HOST "https://fastag-internal.parkzap.com/"

// define two tasks for Blink & AnalogRead
void SearchForKey( void *pvParameters );
void TimeForQuery( void *pvParameters );

// the setup function runs once when you press reset or power the board
void makeconnection()                                   //Wifi connection
{
  Serial.begin(115200);
  delay(400);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(500);
    Serial.print("Connecting Wifi: ");
  }
  Serial.println("WiFi connected");  
  
}

void sdcardconnect();                                   //Declaration for SD Card

void setup() 
{
  
  makeconnection(); //Wifi will be connected
  flag = 0h
  // Now set up two tasks to run independently.
  xTaskCreatePinnedToCore(
    SearchForKey
    ,  "SearchForKey"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    TimeForQuery
    ,  "TimeForQuery"
    ,  1024  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
   xTickType xTimeBefore, xTotalTimeSuspended;
   for( ;; )
   {
      /* Take a time before suspending. */
      xTimeBefore = xTaskGetTickCount();
      /* Suspend the task. */
      vTaskSuspend( NULL );
      /* The other task will have run an unsuspended P2 for this line to execute. */
      xTotalTimeSuspended = xTaskGetTickCount() - xTimeBefore;
   }
}

void makeHTTPRequest()                                   // Html get
{
  HTTPClient http;
  http.begin("https://fastag-internal.parkzap.com/account/mockable_test/"); //Specify the URL
  int httpCode = http.GET();                                        //Make the request
  if (httpCode > 0)
  { //Check for the returning code
     void savetosd()
     {
        File myfile = SD.open("key_log.txt");
    
      // if the file is available, read the file
        if (myfile) 
        {
            while (EOF)
            {
              myfile.write(httpCode);
            }
            myfile.close();
        }  
          // if the file cannot be opened give error report
        else 
        {
            Serial.println("error opening the text file");
        } 
  } 
  }
  else 
  {
    Serial.println("Error on HTTP request");
    flag =  1;
  }
  http.end(); //Free the resources
  
}


void parsejson()
{
  const size_t capacity = JSON_OBJECT_SIZE(1000);
  DynamicJsonBuffer jsonBuffer(capacity);
  JsonObject& root = jsonBuffer.createObject();  
  JsonObject& parsed = JSONBuffer.parseObject(httpCode);
  File myfile = SD.open("key_log.txt");
    
      // if the file is available, read the file
        if (myfile) 
        {
            while (myfile.available)
            {
              JsonObject& parsed = JSONBuffer.parseObject(myfile);
              if (!parsed.success()) //Check for errors in parsing
              {    
                Serial.println("Parsing failed");
                delay(5000);
                return;
              }  
            }
            myfile.close();
        }  
          // if the file cannot be opened give error report
        else 
        {
            Serial.println("error opening the text file");
        } 
}

void loop()
{
  if(flag)                                 //When server is down, parsing is done from sd card
  {
    parsejson();                           // key_log.txt is opened from parsejson() Also to account for irresponsive server 
  }
  
}

//Save website data to local
//Get key and search it in local data and give out corresponding number
//New data is saved when machine starts.

void SearchForKey(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  
  for (;;) // A Task shall never return or exit.
  {
    int ch;
    Serial.println("Enter key");
    key_user = Serial.read();

  }
}

void TimeForQuery(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  for (;;)
  {
    // read the input on analog pin A3:
    int timeforQuery;
    
    // print out the value you read:
    Serial.println(timeforQuery);
    vTaskDelay(10);  // one tick delay (15ms) in between reads for stability
  }
}




void sdcardconnect(){
  Serial.begin(115200);
  Serial.print("Initializing SD card...");
  if (!SD.begin(26, 14, 13, 27)) 
  {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  
  root = SD.open("/");
  if (root) 
  {    
    printDirectory(root, 0);
    root.close();
  } 
  else
  {
    Serial.println("error opening test.txt");
  }
  root = SD.open("key_log.txt", FILE_WRITE);

  payload = makeHTTPRequest();
  
  if (root) {
    root.println(payload);
    root.flush();
   /* close the file */
    root.close();
  } else {
    /* if the file open error, print an error */
    Serial.println("error opening key_log.txt");
  }
  delay(1000); 
}
}
