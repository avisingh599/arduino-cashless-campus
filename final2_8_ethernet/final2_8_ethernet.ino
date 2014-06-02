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


#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {  
  0x90, 0xA2, 0xDA, 0x0E, 0x99, 0x4D };
byte ip[] ={ 
  172, 24, 9, 206 };
byte dns1[] ={ 
  172, 31, 1, 1 };
byte gateway[] ={ 
  172, 24, 15, 254 };
byte subnet[] ={ 
  255, 255, 248, 0 };
IPAddress server(172,24,9,207);
EthernetClient client;


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
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
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


void recieptPrint()
{
  printer.listen();
  printer.setDefault(); // Restore printer to defaults
  printer.wake();
  printer.justify('C');
  printer.println("IITK, Cashless Campus");

  printer.justify('C');
  printer.println(student[finger.fingerID-1].rollNumber);

  for(i=0;i<totCoupon;i++)
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

void grandTotal()
{  
  sum=0;
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


void getMenuWeb()  {
  
  if (client.connect(server, 80)) {
    Serial.println("connected");
   
client.println("GET /sitealpha/forarduino.php HTTP/1.0");
client.println();
while(1)  {
  if (client.available())
  { 
    char c = client.read();
    if (c == '^')

    { 
      c=client.read();
      while(c != '^')

      {
        coupon[inc].name= client.readStringUntil('#') ;
        coupon[inc].rate= client.parseInt();
        coupon[inc].veg=client.parseInt();
        inc=inc+1;
        c=client.read();
      }
      totCoupon=inc-1;
      break;
    }
  }
  

    }
  }
  
   else {
    Serial.println("connection failed");
  }
  
  delay(1000);
  
      
      if (!client.connected()) {
       client.stop();}
       
}



void updateBillEthernet(int roll, int bill)  {
  
 
  if (client.connect(server, 80)) 
  {
    Serial.println("connected");
    
    Serial.println("Sending to Server: ");                    // all the "Serial.print" is for debugging and to show other people what arduino does
      client.print("GET /sitealpha/update_bill.php?");            // send this to apache on server to call the script "writetemplocalserv.php"
      Serial.print("GET /sitealpha/update_bill.php?");            //
      client.print("Roll");                                          // give it the temp on sensor "i" to the script
      Serial.print("Roll");
      client.print("=");
      Serial.print("=");
      client.print(roll);
      Serial.print(roll);
      client.print("&");
      Serial.print("&");
      client.print("Bill");                                          // give it the temp on sensor "i" to the script
      Serial.print("Bill");
      client.print("=");
      Serial.print("=");
      client.print(bill);
      Serial.print(bill);


     client.println(" HTTP/1.1");                  //
      Serial.println(" HTTP/1.1");
      client.println("Host: 172.24.9.207");         //
      Serial.println("Host: 172.24.9.207");         //
      client.println("User-Agent: Arduino");        // ethernet related stuff
      Serial.println("User-Agent: Arduino");        //
      client.println("Accept: text/html");          //
      Serial.println("Accept: text/html");          //
      client.println("Connection: close");        //
      Serial.println("Connection: close");        //
      client.println();                             //
      Serial.println();
      
    
  }

  else {
    // kf you didn't get a connection to the server:
    Serial.println("connection failed");
  }   
  
  delay(1000);
  
      if(client.available())  {
        char c= client.read();
        Serial.print(c);
      }
      
      if (client.connected()) {
        Serial.println("flag");
       client.stop();}
}



void setup()
{
  Serial.begin(9600);
  finger.begin(57600);
  printer.begin();
  myGLCD.InitLCD(PORTRAIT);
  myGLCD.fillScr(0, 98, 125);
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
  
  getMenuWeb();
  
  
}

void loop()
{ 
  
  res=myGLCD.loadBitmap(0, 0, 240, 320, "ready.raw");
  getFingerprintIDez();
  welcomeScreen();

  myGLCD.fillScr(0, 98,125);
  selectCoupons();
  mySerial.flush();
 
  updateBillEthernet(student[finger.fingerID-1].rollNumber, sum);
  recieptPrint();
  reset();
}
