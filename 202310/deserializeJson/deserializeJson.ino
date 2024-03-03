#include <ArduinoJson.h>

void setup() {
  // put your setup code here, to run once:
   Serial.begin(115200);
  DynamicJsonDocument  doc;

  char input[] = "{\"name\":\"Alice\",\"age\":25,\"isStudent\":1,\"hobbies\":[\"reading\",\"painting\",\"hiking\"],\"address\":{\"street\":\"456 Elm St\",\"city\":\"Los Angeles\",\"country\":\"USA\"},\"favoriteFoods\":null}";
  DeserializationError error = deserializeJson(doc, input);

if (error) {
  Serial.print("deserializeJson() failed: ");
  Serial.println(error.c_str());
  return;
}

  const char* name = doc["name"]; // "Alice"
  int age = doc["age"]; // 25
  bool isStudent = doc["isStudent"]; // true

  JsonArray hobbies = doc["hobbies"];
 // const char* hobbies_0 = hobbies[0]; // "reading"
 // const char* hobbies_1 = hobbies[1]; // "painting"
 // const char* hobbies_2 = hobbies[2]; // "hiking"

  JsonObject address = doc["address"];
  const char* address_street = address["street"]; // "456 Elm St"
  const char* address_city = address["city"]; // "Los Angeles"
  const char* address_country = address["country"]; // "USA"

  Serial.print("name: ");
  Serial.println(name);

  Serial.print("age: ");
  Serial.println(age);

  Serial.print("isStudent: ");
  Serial.println(isStudent);

  uint8_t x;
  for(x = 0;x < hobbies.size(); x++){
    Serial.print("hobbies ");
    Serial.print(x);
    Serial.print(" ");
    Serial.println((const char*)hobbies[x]);
  }

  Serial.print("street: ");
  Serial.println(address_street);

  Serial.print("city: ");
  Serial.println(address_city);
  
  Serial.print("country: ");
  Serial.println(address_country);
  
// doc["favoriteFoods"] is null

}

void loop() {
  // put your main code here, to run repeatedly:

}
