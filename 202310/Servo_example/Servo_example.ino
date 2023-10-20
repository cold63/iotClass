#include <AmebaServo.h>

AmebaServo myservo1;

void setup() {
  // put your setup code here, to run once:
   myservo1.attach(8);
   
}

void loop() {
  // put your main code here, to run repeatedly:
 /*
  myservo1.write(0);
  delay(1000);

  myservo1.write(90);
  delay(1000);

  myservo1.write(180);
  delay(1000);

  myservo1.write(90);
  delay(1000);
  */
  /*
    T= 1/f or F=1/t
  */
  
  for(int x = 500; x <=2500;x+= 100){
    myservo1.writeMicroseconds(x);
    delay(500);
  }
  

}
