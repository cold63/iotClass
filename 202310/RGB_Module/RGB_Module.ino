
#define LED_GREEN 3
#define LED_BLUE 7
#define LED_RED 12
uint8_t x;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_RED,OUTPUT);
  pinMode(LED_GREEN,OUTPUT);
  pinMode(LED_BLUE,OUTPUT);

  //yellow
  analogWrite(LED_RED,128);
  analogWrite(LED_GREEN,128);
  analogWrite(LED_BLUE,0);
  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  //Display RED LED --------------/
  analogWrite(LED_GREEN,0);
  analogWrite(LED_BLUE,0); 

  for(x = 0; x < 128; x++){     
    analogWrite(LED_RED,x);   
    delay(200);
  }
  //Display GREEN LED --------------/
  analogWrite(LED_RED,0); 
  analogWrite(LED_BLUE,0);

  for(x = 0; x < 128; x++){
    analogWrite(LED_GREEN,x);
    delay(200);
  }
  //Display BLUE LED --------------/
  analogWrite(LED_GREEN,0);
  analogWrite(LED_RED,0);
  
  for(x = 0; x < 128; x++){
    analogWrite(LED_BLUE,x);
    delay(200);
  }

}
