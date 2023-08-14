#define REMOTEXY_MODE__ETHERNET_LIB
#include <Ethernet.h>/* Ethernet shield used pins: 10(SS) */
#include <SPI.h>// SPI interface used pins: 11(MOSI), 12(MISO), 13(SCK) 
#include <RemoteXY.h>
#include <LiquidCrystal.h>
#include<Servo.h>
#include <Keypad.h>
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
Servo lock;
#define REMOTEXY_ETHERNET_MAC "DE:AD:BE:EF:EF:ED"
#define REMOTEXY_SERVER_PORT 6377

char* password="4567";
int currentposition=0;
int servo_rot = 180;
int count=0;
int cursorColumn = 4;
char password1[11]="444";
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char code;
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};
byte rowPins[ROWS] = {A4, A3, A2, A1}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {10, 9, 8, A5}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
// RemoteXY connection settings 

unsigned long current_milis=0;
unsigned long previous_milis=0;
unsigned long event_time=0;
const unsigned long read_time=10;
enum State{
  ID,
  ONLINE,
  OFFLINE,
  UNLOCKED
}states;
int buttonState = 0;
// RemoteXY configurate  
// RemoteXY configurate  

#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =
  { 255,13,0,0,0,86,0,13,24,1,
  2,0,19,20,22,11,2,26,31,31,
  79,78,0,79,70,70,0,129,0,13,
  11,33,6,16,67,111,110,110,101,99,
  116,105,111,110,0,7,36,8,45,44,
  6,16,26,2,11,1,0,20,69,19,
  19,49,31,88,0,129,0,16,37,28,
  6,16,80,97,115,115,119,111,114,100,
  0,129,0,22,60,14,6,16,76,111,
  99,107,0 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // input variables
  uint8_t switch_1; // =1 if switch ON and =0 if OFF 
  char edit_1[11];  // string UTF8 end zero  
  uint8_t button_1; // =1 if button pressed, else =0 

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY;
#pragma pack(pop)

/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////



void setup() 
{
  RemoteXY_Init (); 
  pinMode(12, INPUT);
  
  
  Serial.begin(9600);

lock.attach(A0);
   lcd.clear();lcd.begin(16, 2); 
  
SPI.begin();
states = ID;
 
 lockD();
 
  
}

void loop() 
{ 
  RemoteXY_Handler ();
  buttonState = digitalRead(12);
  current_milis=millis();
 if(current_milis-previous_milis>10)
 {
    switch(states)
    {
     case ID : 
     lcd.print(" Welcome");
             lcd.setCursor(0,1);
                  lcd.print("Press red Btn  ");
           if (RemoteXY.switch_1==1)
            {
              states=ONLINE;
              lcd.clear();
            }
                 if (buttonState == HIGH )
                 {states=OFFLINE; lcd.clear();}
                
                        
                 
             
     break;
        
     case ONLINE :
    
    lcd.setCursor(0,0);
                  lcd.print("Access ONLINE");
                  if (strcmp (RemoteXY.edit_1, password1)==0) {
                     unlockD();
                     }
                  if(RemoteXY.button_1==1){
                    for(int i=0;i<=11;i++){
                    RemoteXY.edit_1[i]=0; }
                    lockD();
                    RemoteXY.switch_1=0;
                    lcd.setCursor(3,1);
                    lcd.print("Locking...");
                    states=ID;
                    //lcd.clear();
                    }
                  if(RemoteXY.switch_1==0) 
         {states=ID;
         lcd.clear();}
           
     
     break;
     
     case OFFLINE:
       lcd.setCursor(1,0);
                  lcd.print("Access OFFLINE");
                  code = customKeypad.getKey();
                    if(code!=NO_KEY && RemoteXY.switch_1==0 )
   
{ 

 lcd.setCursor(cursorColumn, 1); // move cursor to   (cursorColumn, 0)
    lcd.print("*");                 // print key at (cursorColumn, 0)

    cursorColumn++;                 // move cursor to next position
    if(cursorColumn == 11) // if reaching limit, clear LCD
    {        
      lcd.clear();
      //clearscreen();
      cursorColumn = 4;
    }
      if (code==password[currentposition])
      {
          ++currentposition;

              if(currentposition==4)
                {
                  lcd.clear();
                 
                  currentposition=0;
                  lcd.setCursor(1,0);
                  lcd.print("Access Granted");
                  lcd.setCursor(3,1);
                  lcd.print("Unlocked");
                  
                  cursorColumn = 4;
                   unlockD();
                   states=UNLOCKED;
                   
                }
       }
      else  
      {
      currentposition=0;
      }
 
    
 
}
                  
                 
     break;
      case UNLOCKED:
      event_time=millis();
         if (buttonState == HIGH)
                 { lockD();
                  lcd.clear();
                  count++;
                  }
          if (event_time-3000>100&&count>1 && buttonState==LOW)
          {states=ID;
          count=0;}
          
      break;
        
     }
         previous_milis=millis();
Serial.println(states);
 }
}


void unlockD() {
  lock.write(-180);
  servo_rot = -180;

  Serial.println("Unlocking...");
}

void lockD() {
  lock.write(180);
  servo_rot = 180;
  
  Serial.println("Locking...");
}
void clearscreen(){ 

for(int i=0;i<=16;i++)
  {
    lcd.setCursor(i,1);
    lcd.print(" ");
    }
    for(int j=9;j<=16;j++)
  {
    lcd.setCursor(j,0);
    lcd.print(" ");
    }

 }
