#include <UTFT.h>
#include <UTouch.h>
#include <avr/pgmspace.h>
#include <SD.h>
#include <UTFT_tinyFAT.h>
#include <tinyFAT.h>
#include<UTFT_Geometry.h>
#include "Adafruit_Thermal.h"
#include <Adafruit_Fingerprint.h>
#if ARDUINO >= 100
#include <SoftwareSerial.h>
#else
#include <NewSoftSerial.h>
#endif

#if ARDUINO >= 100
SoftwareSerial mySerial(A12, A14);
#else
NewSoftSerial mySerial(A12, A14);
#endif

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);


int printer_RX_Pin = A10;  // This is the green wire
int printer_TX_Pin = A8;  // This is the yellow wire

Adafruit_Thermal printer(printer_RX_Pin, printer_TX_Pin);




int totalStudents;
int getFingerprintIDez();

typedef struct person  {
  String name;
  int rollNumber;
  int ID;
}
Person;

Person student[10]; 



File myFile;
int kev=0, noItems;
int sum=0, inc=0, Gsum=0;
File root;
int day;
int meal;
const int chipSelect = 53;
char menu[]="00.txt";
char SBill[]="00000.txt";
String SName;

UTFTtf      myGLCD(ITDB32S, 38,39,40,41);
UTouch      myTouch(6,5,4,3,2);
UTFT_Geometry geo(&myGLCD);



extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];
extern uint8_t SmallFont[];


int picsize_x, picsize_y;
boolean display_rendertime=false;  // Set this to true if you want the rendertime to be displayed after a picture is loaded
boolean display_filename=false;  // Set this to false to disable showing of filename

int currCoupon;
int totCoupon;
int x, y;
int i;
int k=0;

typedef struct menu{
  String name;
  int quant;
  int rate;
  int veg;        //veg=1 for veg, veg=0 for non-veg
} 
Menu;


Menu coupon[6];
word res;
long sm, em;

int rollNum[5], SRoll;
int rollPos;



// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  mySerial.listen();
  uint8_t p = finger.getImage();
  while(!(p==FINGERPRINT_OK)){
    p = finger.getImage();
  }
  // if (p != FINGERPRINT_OK)  return -1;
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) 
  { return 1; }
  
  else if (p == FINGERPRINT_NOTFOUND) 
  
  {  myGLCD.fillScr(0, 98, 125);
     myGLCD.setColor(255, 255, 255);
     myGLCD.setBackColor(0, 98, 125);
     myGLCD.setFont(BigFont);
     myGLCD.print("No match found", 5, 20);
     myGLCD.print("Please try", 5, 200);
     myGLCD.print("again",5, 220);
     getFingerprintIDez();
     
  }
  
  else {
     myGLCD.fillScr(0, 98, 125);
     myGLCD.setColor(255, 255, 255);
     myGLCD.setBackColor(0, 98, 125);
     myGLCD.setFont(BigFont);
     myGLCD.print("Error in", 20, 20);
     myGLCD.print("Recieving", 20, 40);
     myGLCD.print("Packet", 20, 60);
     myGLCD.print("Please try", 5, 200);
     myGLCD.print("again", 5, 220);
     getFingerprintIDez();
     
  }
     
  Serial.print("Found ID #"); 
  Serial.print(finger.fingerID);
  Serial.print("with Roll Number: ");
  Serial.print(student[finger.fingerID-1].rollNumber);
  Serial.print("whose name is ");
  Serial.print(student[finger.fingerID-1].name);
  Serial.print(" with confidence of "); 
  Serial.println(finger.confidence);
  return finger.fingerID; 
}


void drawDays()
{
  myGLCD.setColor(255, 255, 255);
  myGLCD.fillCircle(120, 160, 30);
  myGLCD.fillCircle(60, 160, 30);
  myGLCD.fillCircle(180, 160, 30);
  myGLCD.fillCircle(150, 212, 30);
  myGLCD.fillCircle(90, 212, 30);
  myGLCD.fillCircle(150, 108, 30);
  myGLCD.fillCircle(90, 108, 30);

  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(255, 255, 255);
  myGLCD.setFont(BigFont);
  myGLCD.print("Sun", 96, 152);
  myGLCD.print("Mon", 36, 152);
  myGLCD.print("Thu", 156, 152);
  myGLCD.print("Wed", 126, 206);
  myGLCD.print("Tue", 66, 206);
  myGLCD.print("Fri", 126, 100);
  myGLCD.print("Sat", 66, 100);

}

void getDay()  {
  myGLCD.setFont(BigFont);
  myGLCD.fillScr(0, 98, 125);

  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 98, 125);
  myGLCD.print("Select Day", 35, 20);

  drawDays();
  myGLCD.setColor( 255, 255, 255);
  myGLCD.fillRoundRect(20, 270, 220, 310);
  myGLCD.setColor(0, 0, 0);
  myGLCD.drawRoundRect(20, 270, 220, 310);
  myGLCD.setBackColor(255, 255, 255);
  myGLCD.setColor(0, 0, 0);
  myGLCD.setFont(BigFont);
  myGLCD.print("Continue", 50, 280);

  while(1) { 
    myGLCD.setColor(0, 200, 255);

    if(myTouch.dataAvailable()) {

      myTouch.read();
      x=myTouch.getX();
      y=myTouch.getY();
      while (myTouch.dataAvailable()) ; //uncomment this later to test

      if ((y>=78)&&(y<=138)) { 
        drawDays(); 
        myGLCD.setColor(9, 200, 255);

        if ((x<=120)&&(x>=60)) { 
          day=7; 
          myGLCD.fillCircle(90, 108, 30); 
          myGLCD.setColor(0, 0, 0);
          myGLCD.setBackColor(0, 200, 255);
          myGLCD.print("Sat", 66, 100);
        }
        if ((x>=120)&&(x<=180)) { 
          day=6; 
          myGLCD.fillCircle(150, 108, 30);
          myGLCD.setColor(0, 0, 0);
          myGLCD.setBackColor(0, 200, 255);
          myGLCD.print("Fri", 126, 100);
        }

      }

      if ((y>=182)&&(y<=242)) { 
        drawDays();  
        myGLCD.setColor(9, 200, 255);

        if ((x<=120)&&(x>=60)) { 
          day=3; 
          myGLCD.fillCircle(90, 212, 30);
          myGLCD.setColor(0, 0, 0);
          myGLCD.setBackColor(0, 200, 255);
          myGLCD.print("Tue", 66, 206);
        }
        if ((x>=120)&&(x<=180)) { 
          day=4; 
          myGLCD.fillCircle(150, 212, 30);
          myGLCD.setColor(0, 0, 0);
          myGLCD.setBackColor(0, 200, 255);
          myGLCD.print("Wed", 126, 206); 
        }

      }

      if ((y>138)&&(y<182)) { 
        drawDays();  
        myGLCD.setColor(9, 200, 255);

        if ((x<=150)&&(x>=90)) { 
          day=1; 
          myGLCD.fillCircle(120, 160, 30);
          myGLCD.setColor(0, 0, 0);
          myGLCD.setBackColor(0, 200, 255);
          myGLCD.print("Sun", 96, 152);
        }

        if ((x>=30)&&(x<=90)) 
        { 
          day=2; 
          myGLCD.fillCircle(60, 160, 30);
          myGLCD.setColor(0, 0, 0);
          myGLCD.setBackColor(0, 200, 255);
          myGLCD.print("Mon", 36, 152); 
        }
        if ((x>=150)&&(x<=210)) 
        { 
          day=5; 
          myGLCD.fillCircle(180, 160, 30);
          myGLCD.setColor(0, 0, 0);
          myGLCD.setBackColor(0, 200, 255); 
          myGLCD.print("Thu", 156, 152); 
        }
      }

      if (((y>=270)&&(y<=310))&&((x>=20)&&(x<=220)))
      {  
        if (day)
          break;
      }

    }
  }
}

void drawMeal()  {

  myGLCD.setColor(255, 255, 255);
  myGLCD.fillCircle(70, 100, 50);
  myGLCD.fillCircle(170, 100, 50);
  myGLCD.fillCircle(120, 187, 50);

  myGLCD.setFont(BigFont);
  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(255, 255, 255);
  myGLCD.print("Lunch", 30, 92);
  myGLCD.print("Dinner", 122, 92);
  myGLCD.print("Break",80, 171);
  myGLCD.print("Fast", 88, 187);

}

void getMeal() {
  myGLCD.fillScr(0, 98, 125);
  drawMeal();
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 98, 125);
  myGLCD.print("Select Meal", 20, 20);

  myGLCD.setColor( 255, 255, 255);
  myGLCD.fillRoundRect(20, 270, 220, 310);
  myGLCD.setColor(0, 0, 0);
  myGLCD.drawRoundRect(20, 270, 220, 310);
  myGLCD.setBackColor(255, 255, 255);
  myGLCD.setColor(0, 0, 0);
  myGLCD.setFont(BigFont);
  myGLCD.print("Continue", 50, 280);

  while (true)  {

    if (myTouch.dataAvailable())    {

      myTouch.read();
      x=myTouch.getX();
      y=myTouch.getY();

      while(myTouch.dataAvailable())  {
        ;
      }

      if ((y>=50)&&(y<=150))  {

        if ((x>=20)&&(x<=120))   {
          meal=2;
          drawMeal();
          myGLCD.setColor(0, 200, 255);
          myGLCD.fillCircle(70, 100, 50);
          myGLCD.setColor(0, 0, 0);
          myGLCD.setBackColor(0, 200, 255);
          myGLCD.print("Lunch", 30, 92);

        }

        if ((x>=120)&&(x<=220))   {
          meal=3;
          drawMeal();
          myGLCD.setColor(0, 200, 255);
          myGLCD.fillCircle(170, 100, 50);
          myGLCD.setColor(0, 0, 0);
          myGLCD.setBackColor(0, 200, 255);
          myGLCD.print("Dinner", 122, 92);

        }

      }

      if (((y>=150)&&(y<=250))&&((x>=70)&&(x<=170)))  {
        meal=1;
        drawMeal();
        myGLCD.setColor(0, 200, 255);
        myGLCD.fillCircle(120, 187, 50);
        myGLCD.setColor(0, 0, 0);
        myGLCD.setBackColor(0, 200, 255);
        myGLCD.print("Break",80, 171);
        myGLCD.print("Fast", 88, 187);

      }

      if (((y>=270)&&(y<=310))&&((x>=20)&&(x<=220)))
      {  
        if (meal)
          break;
      }

    }  


  }

}


void  printmyDigit (int rollPos)
{ 
  myGLCD.setBackColor(0, 98, 125);
  myGLCD.setColor(255, 255, 255);
  myGLCD.printNumI(rollNum[rollPos], 32+rollPos*36, 60);

} 

void getRollNum()

{  
  myGLCD.setFont(SevenSegNumFont);
  picsize_x=myGLCD.getDisplayXSize();
  picsize_y=myGLCD.getDisplayYSize();
  res=myGLCD.loadBitmap(0, 0, 240, 320, "login2.raw");


  rollPos=0;
  while(1)
  {
    if (myTouch.dataAvailable())
    {
      myTouch.read();
      x=myTouch.getX();
      y=myTouch.getY();


      x=myTouch.getX();
      y=myTouch.getY();


      if (((y>=120) && (y<=170))&&(rollPos<5))
      {
        if ((x>=0) && (x<=80))
        { 
          rollNum[rollPos]=1;
          printmyDigit(rollPos);
          rollPos++;
        }

        if ((x>=80) && (x<=160))
        { 
          rollNum[rollPos]=2;
          printmyDigit(rollPos);
          rollPos++;
        }

        if ((x>=160) && (x<=240))
        { 
          rollNum[rollPos]=3;
          printmyDigit(rollPos);
          rollPos++;
        }
      }

      if (((y>=170) && (y<=220))&&(rollPos<5))
      {
        if ((x>=0) && (x<=80))
        { 
          rollNum[rollPos]=4;
          printmyDigit(rollPos);
          rollPos++;
        }

        if ((x>=80) && (x<=160))
        { 
          rollNum[rollPos]=5;
          printmyDigit(rollPos);
          rollPos++;
        }

        if ((x>=160) && (x<=240))
        { 
          rollNum[rollPos]=6;
          printmyDigit(rollPos);
          rollPos++;
        }
      }


      if (((y>=220) && (y<=270))&&(rollPos<5))
      {
        if ((x>=0) && (x<=80))
        { 
          rollNum[rollPos]=7;
          printmyDigit(rollPos);
          rollPos++;
        }

        if ((x>=80) && (x<=160))
        { 
          rollNum[rollPos]=8;
          printmyDigit(rollPos);
          rollPos++;
        }

        if ((x>=160) && (x<=240))
        { 
          rollNum[rollPos]=9;
          printmyDigit(rollPos);
          rollPos++;
        }
      }

      if ((y>=270) && (y<=320))
      {
        if ((x>=160) && (x<=240))
        { 
          if (rollPos>4) {
            break;
          }


        }

        if (((x>=80) && (x<=160))&&(rollPos<5))
        { 
          rollNum[rollPos]=0;
          printmyDigit(rollPos);
          rollPos++;
        }

        if ((x>=0) && (x<=80))
        { 
          if (rollPos>0) {
            myGLCD.setColor(0, 98, 125);
            rollPos--;
            myGLCD.fillRect(32+rollPos*36, 60, 68+rollPos*36, 110);

          }

        }
      }



    }
    delay(100);
  }
}

void drawCoupon(int currCoupon)
{
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(0, 98, 125);
  myGLCD.setColor(255, 255, 255);
  myGLCD.print("Select Coupons", 20, 20);
  myGLCD.setColor(255, 255, 255);
  myGLCD.fillRoundRect(5, 40, 235, 160);

  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(255, 255, 255);
  myGLCD.setColor(0, 0, 0);
  myGLCD.print(coupon[currCoupon].name, 10, 50);
  myGLCD.print("Rs.", 10, 70);
  myGLCD.printNumI(coupon[currCoupon].rate, 40, 70);


  if (coupon[currCoupon].veg==1) {
    myGLCD.setColor(59, 159, 5);
  }    //drawing the green veg symbol
  else {
    myGLCD.setColor(212, 0, 0);
  } 
  myGLCD.fillRect(160, 100, 210, 150);
  myGLCD.setColor(255, 255, 255);
  myGLCD.fillRect(165, 105, 205, 145);
  if (coupon[currCoupon].veg==1) {
    myGLCD.setColor(59, 159, 5);
  }    //drawing the green veg symbol
  else {
    myGLCD.setColor(212, 0, 0);
  }
  myGLCD.fillCircle(185, 125, 15);

  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 98, 125);
  myGLCD.print("You will take", 20, 170);
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.printNumI(coupon[currCoupon].quant, 95, 190);

}

void drawButts() 
{ 
  myGLCD.setColor(255, 255, 255);       //draw the +/- buttons
  myGLCD.fillCircle(55, 215, 25);
  myGLCD.fillCircle(185, 215, 25);
  myGLCD.setColor(0, 98, 125);
  myGLCD.fillRect(35, 210, 75, 220);
  myGLCD.fillRect(165, 210, 205, 220);
  myGLCD.fillRect(180,195 , 190, 235);

  myGLCD.setColor(255, 255, 255);
  geo.fillTriangle( 20, 285, 70, 260, 70, 310);
  geo.fillTriangle( 130, 285, 80, 260, 80, 310);

  myGLCD.fillRoundRect(150, 260, 230, 310);
  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(255, 255, 255);
  myGLCD.setFont(BigFont);

  myGLCD.print("Final", 150, 270);
  myGLCD.print("Order", 150, 290);


}



void confirmScr() {

  myGLCD.fillScr(0, 98, 125);
  myGLCD.setFont(BigFont);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 98, 125);
  myGLCD.print("Confirm Order", 20, 20);

  int i;
  for(i=0;i<totCoupon;i++)
  {   
    myGLCD.print(coupon[i].name, 20, 60 + (20*i));
    myGLCD.print("X", 200, 60+20*i);
    myGLCD.printNumI(coupon[i].quant, 220, 60+(20*i));

  }

  myGLCD.print("Total: Rs.", 20, 200);
  myGLCD.printNumI(sum, 180, 200);

  myGLCD.setColor(255, 255, 255);
  myGLCD.fillRoundRect(10, 240, 110, 300);
  myGLCD.fillRoundRect(130, 240, 230, 300);
  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(255, 255, 255);
  myGLCD.print("Back", 20, 260);
  myGLCD.print("Final", 130, 260);

  while(true)  {

    if (myTouch.dataAvailable())  {
      myTouch.read();
      x=myTouch.getX();
      y=myTouch.getY();

      if ((y<=300)&&(y>=240))  {

        if ((x>=10)&&(x<=110))  {
          myGLCD.fillScr(0, 98, 125);
          selectCoupons();
          break;  
        }

        if ((x>=130)&&(x<=230))  {
          myGLCD.fillScr(0, 98, 125);
          myGLCD.setColor(255, 255, 255);
          myGLCD.setBackColor(0, 98, 125);
          myGLCD.print("Thank You", 20, 20);
          delay(1000);
          break;
        }


      }


      delay(100);
    }        
  }
}

void selectCoupons()  {
  drawCoupon(currCoupon);

  drawButts();

  while(1)
  {

    if (myTouch.dataAvailable())
    {
      myTouch.read(); 
      x=myTouch.getX();
      y=myTouch.getY();

      if ((y>=260)&&(y<=310))  {
        if ((x<=70)&&(x>=20)) {
          if (currCoupon>0) currCoupon--;
          else currCoupon=totCoupon-1;
        }

        if ((x>=80)&&(x<=130))  {
          if (currCoupon!=(totCoupon-1))
          {
            currCoupon++;
          }
          else {
            currCoupon=0;
          }
        }

        if ((x>=150)&&(x<=240))  {
          grandTotal();
          confirmScr();
          break;
        }
      }


      if ((y>=190)&&(y<=240))  {
        if((x>=30)&&(x<=80))
        { 
          if (coupon[currCoupon].quant>0)
            coupon[currCoupon].quant--;
        }
        if (( x>=165)&&(x<=210))
        {
          coupon[currCoupon].quant++;
        }
      }
      drawCoupon(currCoupon);

    }
    delay(100);
  }
}

void SDHeaders()
{

  File logFile = SD.open("orders.csv", FILE_WRITE);
  if (logFile)
  {
    logFile.println("-,-,-,-,-,-,-,-,-"); //Just a leading blank line, incase there was previous data
    String header = "Roll No.,Student Name,";
    logFile.print(header);
    kev=0;
    while(kev<noItems)
    {
      logFile.print(coupon[kev].name);
      logFile.print(",");
      kev++;
    }
    logFile.println("Grand Total");
    logFile.close();
  }
  else
  {
    Serial.println("Couldn't open log file");
  }

}

void savetoSDnprint()
{
  GenSBill();
  myFile= SD.open("orders.csv", FILE_WRITE);
  if(myFile){
    myFile.print(SRoll);
    myFile.print(",");
    myFile.print(SName);
    myFile.print(",");
    Serial.println("OKKK!!");
    kev=0;
    while(kev<noItems){

      Serial.println("OKKK!!");

      myFile.print(coupon[kev].quant);
      myFile.print(",");
      kev=kev+1;
    }
    myFile.println(sum);
  }

  myFile.close();
  kev=0;

  printer.listen();

  printer.setDefault(); // Restore printer to defaults
  printer.wake();
  printer.justify('C');
  printer.println("IITK, Cashless Campus");

  printer.justify('C');
  printer.println(SRoll);

//  printer.setSize('M');

  for(i=0;i<noItems;i++)
  { 
    if(!(coupon[i].quant==0)){
      printer.print(coupon[i].name);
      printer.print(" X ");
      printer.print(coupon[i].quant);
      printer.print("  =  ");
      printer.print("Rs. ");
      printer.println(coupon[i].quant*coupon[i].rate);
    }
  }
  printer.setSize('M');

  printer.justify('C');
  printer.print("TOTAL = Rs. ");
  printer.println(sum);
  printer.println("____________________");
  printer.println(" ");

  printer.flush();
  printer.sleep();      // Tell printer to sleep
  printer.setDefault(); // Restore printer to defaults
  delay(1000);

  for (k=0;k<6;k++)
  {
    coupon[k].quant=0;
  }
  sum=0;
}


void getmenu()
{
  menu[0]=day+48;
  menu[1]=meal+48;

  myFile=SD.open(menu);
  Serial.println(menu);  
  noItems=myFile.parseInt();   // parse doesnt read the next char (only peeks)
  myFile.read();
  while(inc<noItems){
    coupon[inc].name=myFile.readStringUntil(',');  // readuntil reads " , " also
    coupon[inc].rate=myFile.parseInt();
    myFile.read();
    coupon[inc].veg=myFile.parseInt();
    myFile.read();
    inc=inc+1;
  }
  totCoupon=noItems;
}

void grandTotal()
{  
  sum=0;
  int k;
  for (k=0;k<totCoupon;k++)
  {
    sum = sum + coupon[k].rate*coupon[k].quant ;
  }
}

void reset()
{
  k=0;
  for (k=0;k<6;k++)
  {
    coupon[k].quant=0;
  }

  k=0;
}



void GenSBill(){

  File SB=SD.open(SBill);
  if(SB){
    SName=SB.readStringUntil(',');
    Gsum=SB.parseInt();
  }
  SB.close();
  Gsum=Gsum+sum;
  SD.remove(SBill);
  SB=SD.open(SBill, FILE_WRITE);
  if(SB){
    SB.print(SName);
    SB.print(",");
    SB.print(Gsum);
    SB.print(",");
  }
  SB.close();
}

void welcomeScreen()  {

  myGLCD.fillScr(0, 98, 125);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 98, 125);
  myGLCD.print("Welcome", 20, 20);
  myGLCD.print(student[finger.fingerID-1].name, 20, 40);
  myGLCD.print("RollNumber is", 20, 60);
  myGLCD.printNumI(student[finger.fingerID-1].rollNumber, 20, 80);
  delay(1000);
  myGLCD.print("Touch to", 20, 280);
  myGLCD.print("Continue", 20, 300);
  while (!(myTouch.dataAvailable())) ;

}


void setVars(){
  int ruf;
  SRoll=student[finger.fingerID-1].rollNumber;
  ruf=SRoll;
  SBill[0]=((ruf-(ruf%10000))/10000)+48;
  ruf=ruf%10000;
  SBill[1]=((ruf-(ruf%1000))/1000)+48;
  ruf=ruf%1000;
  SBill[2]=((ruf-(ruf%100))/100)+48;
  ruf=ruf%100;
  SBill[3]=((ruf-(ruf%10))/10)+48;
  ruf=ruf%10;
  SBill[4]=ruf+48;
  Serial.print(SBill[0]);
  Serial.print(SBill[1]);
  Serial.print(SBill[2]);
  Serial.print(SBill[3]);
  Serial.print(SBill[4]);
  Serial.print(SBill[5]);
  Serial.print(SBill[6]);
  Serial.print(SBill[7]);
  Serial.println(SBill[8]);
  SName=student[finger.fingerID-1].name;
}



void setup()
{
  Serial.begin(9600);
  finger.begin(57600);
  printer.begin();
  myGLCD.InitLCD(PORTRAIT);
  myGLCD.fillScr(0, 98, 125);
  myGLCD.setFont(SevenSegNumFont);
  if (!SD.begin(chipSelect)) {
    return;
  }
  res=file.initFAT();
  myGLCD.setColor(255,255,255);
  myTouch.InitTouch(0);
  myTouch.setPrecision(PREC_MEDIUM);
  currCoupon=0;
  totCoupon=6;

  //Assigning Names, IDs and roll Numbers
  student[0].name="Avi";
  student[1].name="Kevin";
  student[2].name="Vineet";
  student[3].name="Prawal";

  for(i=0;i<totalStudents;i++)
  {
    student[i].ID=i+1;
  }

  student[0].rollNumber=12177;
  student[1].rollNumber=12352;
  student[2].rollNumber=12810;
  student[3].rollNumber=12510;

  // Assigning ends here


  getDay();
  getMeal();

  getmenu();

  SDHeaders();
}


void loop()
{ 
  //getRollNum();
  //SRoll=rollNum[0]*10000+rollNum[1]*1000+rollNum[2]*100+rollNum[3]*10+rollNum[4];
  // verifyFinger();
  res=myGLCD.loadBitmap(0, 0, 240, 320, "ready.raw");
  getFingerprintIDez();
  welcomeScreen();
  setVars();
  myGLCD.fillScr(0, 98,125);
  selectCoupons();
  mySerial.flush();
  savetoSDnprint();
  reset();
}








