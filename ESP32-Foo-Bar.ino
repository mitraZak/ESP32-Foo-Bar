#include <Arduino.h>


#define MAX_BUFFER 8

QueueHandle_t queueEven, queueOdd;
SemaphoreHandle_t timingSemaphore;

bool isPrime(int num) {
    if (num < 2) return false;
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) return false;
    }
    return true;
}

String createLabel(int num) {
    return (num % 2 ? "Bar" : "Foo") + String(" ") + String(num) + (isPrime(num) ? " Prime" : "");
}

// if true, core 1 is selected, else core 0
bool selectCore(int num) {
  bool core = (num % 2) ? 1 : 0;
  return (isPrime(num)) ? 1 : core;
}

void outputMessage(const char* label) {
    xSemaphoreTake(timingSemaphore, portMAX_DELAY);
    Serial.println(label);    
    xSemaphoreGive(timingSemaphore);
}

void taskEven(void *parameter) {
    int num;
    while (true) {
        if (xQueueReceive(queueEven, &num, portMAX_DELAY) == pdPASS) {
            outputMessage(createLabel(num).c_str());
        }
    }
}

void taskOdd(void *parameter) {
    int num;
    while (true) {
        if (xQueueReceive(queueOdd, &num, portMAX_DELAY) == pdPASS) {
            outputMessage(createLabel(num).c_str());
        }
    }
}

int readInput() {
    while (Serial.available()) {}
    String input = Serial.readStringUntil('\n');
    int n = input.toInt();
    Serial.printf("Received %d\n", n);
    return n;
}

void setup() {
    Serial.begin(9600);
    while (!Serial);

    queueEven = xQueueCreate(MAX_BUFFER, sizeof(int));
    queueOdd = xQueueCreate(MAX_BUFFER, sizeof(int));
    timingSemaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(timingSemaphore);

    xTaskCreatePinnedToCore(taskEven, "TaskEven", 1000, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(taskOdd, "TaskOdd", 1000, NULL, 1, NULL, 1);

    Serial.println("Enter a positive integer:");
    while (Serial.available()) {}
    String input = Serial.readStringUntil('\n');
    int n = input.toInt();
    Serial.printf("Received %d\n", n);

    for (int i = n; i >= 0; i--) {
        delay(1000);
        if (selectCore(i) == 0) {
            xQueueSend(queueEven, &i, portMAX_DELAY); // even on core 0
        } else {
            xQueueSend(queueOdd, &i, portMAX_DELAY); // odd on core 1
        }
    }
}

void loop() {
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        int n = input.toInt();
        Serial.printf("Received %d\n", n);

        if (n == 0) {
            Serial.println("Restarting ESP32...");
            ESP.restart();
        } 
        else if (uxQueueMessagesWaiting(queueEven) + uxQueueMessagesWaiting(queueOdd) < MAX_BUFFER) {
            for (int i = n; i >= 0; i--) {
                delay(1000);
                if (selectCore(i) == 0) {
                    xQueueSend(queueEven, &i, portMAX_DELAY);
                } else {
                    xQueueSend(queueOdd, &i, portMAX_DELAY);
                }
            }
        } 
        else {
            Serial.println("Buffer is full");
        }
    }
}
