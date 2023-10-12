

const int analogPin = 6;
int value;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  value = analogRead(analogPin);
  Serial.print("Value:");
  Serial.println(value);


  Serial.print("Voltage:");
  Serial.print((3.3 * value)/1030);
  Serial.println(" V");
  Serial.println("");
   
  delay(1000);
 
}
