#include <Arduino.h>

String type = "";
int n;

bool is_prime(int number){
    if(number < 2) return false;
    if(number == 2) return true;
    if(number % 2 == 0) return false;
    for(int i=3; (i*i)<=number; i+=2){
        if(number % i == 0 ) return false;
    }
    return true;
}

void even_func(void *pvParameters) {
    int i = (int) pvParameters; 
    type += "Foo";
    Serial.printf("\nFoo %d\n", i);
    vTaskDelete(NULL);
}

void odd_func(void *pvParameters) {
    int i = (int) pvParameters;
    type += "Bar";
    Serial.printf("\nBar %d\n", i);
    vTaskDelete(NULL);
}

void prime_func(void *pvParameters) {
    int i = (int) pvParameters;
    type += "Prime";
    Serial.printf("\nPrime %d\n", i);
    vTaskDelete(NULL);
}



void setup() {
  Serial.begin(115200);
  Serial.println("enter");
  while (Serial.available() == 0) {
  }
  String input = Serial.readStringUntil('\n');
  int n = input.toInt();
  Serial.print("Received ");
  Serial.println(n);

  for (int i=n; i>=0; i--){
    if ((i%2)==0) {
      xTaskCreatePinnedToCore(even_func, "even_task", 10000, (void*) i, 1, NULL, 0);}
    if ((i%2)==1){
      xTaskCreatePinnedToCore(odd_func, "odd_task", 10000, (void*) i, 1, NULL, 1);}
    if (is_prime(i)){
      xTaskCreatePinnedToCore(prime_func, "prime_task", 10000, (void*) i, 1, NULL, 0);}
  }
}

void loop() {
}
