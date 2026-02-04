#include "fake_serial.h"
#include "../src/hardware/arduino_compat.h"
#include <iostream>

FakeSerial Serial(true, true);  // Эхо в stdout с таймстампами

void setup() {
    Serial.begin(9600);
    Serial.println("=== Serial Test Started ===");
    
    // Разные типы данных
    Serial.print("Integer: ");
    Serial.println(42);
    
    Serial.print("Float: ");
    Serial.println(3.14159, 4);
    
    Serial.print("Hex: 0x");
    Serial.println(255, HEX);
    
    // Использование String
    String message = "Hello from String!";
    Serial.println(message);
    
    // Бинарный вывод
    Serial.print("Binary: ");
    Serial.println(5, BIN);
}

void loop() {
    // Не используется в тестах
}

int main() {
    std::cout << "=== Testing FakeSerial ===\n\n";
    
    setup();
    
    // Показать что накопилось в буфере
    std::cout << "\n\n=== Captured Serial Output ===\n";
    std::cout << Serial.getOutput();
    
    // Показать построчно
    auto lines = Serial.getLines();
    std::cout << "\n=== Lines (" << lines.size() << ") ===\n";
    for (size_t i = 0; i < lines.size(); i++) {
        std::cout << i << ": " << lines[i] << std::endl;
    }
    
    return 0;
}