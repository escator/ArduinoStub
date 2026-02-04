#ifndef FAKE_SERIAL_H
#define FAKE_SERIAL_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <chrono>
#include "arduino_compat.h"  // Для String

class FakeSerial {
private:
    std::stringstream buffer_;
    bool echo_to_stdout_;
    bool timestamp_enabled_;
    std::chrono::steady_clock::time_point start_time_;
    
public:
    // Конструктор
    FakeSerial(bool echo = true, bool timestamp = false) 
        : echo_to_stdout_(echo), 
          timestamp_enabled_(timestamp),
          start_time_(std::chrono::steady_clock::now()) {}
    
    // Метод begin (имитация Serial.begin())
    void begin(unsigned long baudrate) {
        if (echo_to_stdout_) {
            std::cout << "[Serial] Initialized with baud rate: " << baudrate << std::endl;
        }
    }
    
    void begin(unsigned long baudrate, uint8_t config) {
        begin(baudrate);
        (void)config;
    }
    
    // Проверка доступности данных
    int available() {
        return 0;  // На ПК всегда 0
    }
    
    // Чтение (заглушки)
    int read() {
        return -1;  // EOF
    }
    
    int peek() {
        return -1;
    }
    
    void flush() {
        // В реальном Serial очищает буфер передачи
        buffer_.str("");  // Очищаем буфер
        buffer_.clear();
    }
    
    // Метод print для разных типов
    size_t print(const char str[]) {
        return write(str);
    }
    
    size_t print(char c) {
        return write(c);
    }
    
    size_t print(unsigned char b, int base = DEC) {
        return print(static_cast<unsigned long>(b), base);
    }
    
    size_t print(int n, int base = DEC) {
        return print(static_cast<long>(n), base);
    }
    
    size_t print(unsigned int n, int base = DEC) {
        return print(static_cast<unsigned long>(n), base);
    }
    
    size_t print(long n, int base = DEC) {
        std::string str;
        if (base == DEC) {
            str = std::to_string(n);
        } else if (base == HEX) {
            std::stringstream ss;
            ss << std::hex << n;
            str = ss.str();
        } else if (base == OCT) {
            std::stringstream ss;
            ss << std::oct << n;
            str = ss.str();
        } else if (base == BIN) {
            // Бинарное представление
            unsigned long un = (n < 0) ? -n : n;
            for (int i = sizeof(n) * 8 - 1; i >= 0; i--) {
                str += (un & (1UL << i)) ? '1' : '0';
            }
        }
        return write(str.c_str());
    }
    
    size_t print(unsigned long n, int base = DEC) {
        std::string str;
        if (base == DEC) {
            str = std::to_string(n);
        } else if (base == HEX) {
            std::stringstream ss;
            ss << std::hex << n;
            str = ss.str();
        } else if (base == OCT) {
            std::stringstream ss;
            ss << std::oct << n;
            str = ss.str();
        }
        return write(str.c_str());
    }
    
    size_t print(double n, int digits = 2) {
        std::stringstream ss;
        ss.precision(digits);
        ss << std::fixed << n;
        return write(ss.str().c_str());
    }
    
    // Для String (нашей реализации)
    size_t print(const String &str) {
        return write(str.c_str());
    }
    
    // Метод println (print с новой строкой)
    size_t println(const char str[]) {
        size_t n = print(str);
        n += println();
        return n;
    }
    
    size_t println(char c) {
        size_t n = print(c);
        n += println();
        return n;
    }
    
    size_t println(unsigned char b, int base = DEC) {
        size_t n = print(b, base);
        n += println();
        return n;
    }
    
    size_t println(int n, int base = DEC) {
        size_t cnt = print(n, base);
        cnt += println();
        return cnt;
    }
    
    size_t println(unsigned int n, int base = DEC) {
        size_t cnt = print(n, base);
        cnt += println();
        return cnt;
    }
    
    size_t println(long n, int base = DEC) {
        size_t cnt = print(n, base);
        cnt += println();
        return cnt;
    }
    
    size_t println(unsigned long n, int base = DEC) {
        size_t cnt = print(n, base);
        cnt += println();
        return cnt;
    }
    
    size_t println(double n, int digits = 2) {
        size_t cnt = print(n, digits);
        cnt += println();
        return cnt;
    }
    
    size_t println(const String &str) {
        size_t cnt = print(str);
        cnt += println();
        return cnt;
    }
    
    size_t println() {
        return write("\n");
    }
    
    // Низкоуровневый write
    size_t write(uint8_t c) {
        return write(static_cast<char>(c));
    }
    
    size_t write(const char* str) {
        return write(str, strlen(str));
    }
    
    size_t write(const uint8_t* buffer, size_t size) {
        return write(reinterpret_cast<const char*>(buffer), size);
    }
    
    size_t write(const char* buffer, size_t size) {
        // Добавляем в буфер
        std::string data(buffer, size);
        buffer_ << data;
        
        // Выводим в stdout если включено
        if (echo_to_stdout_) {
            if (timestamp_enabled_) {
                auto now = std::chrono::steady_clock::now();
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - start_time_).count();
                std::cout << "[" << ms << "ms] ";
            }
            std::cout << data << std::flush;
        }
        
        return size;
    }
    
    // Вспомогательные методы для тестирования
    std::string getOutput() const {
        return buffer_.str();
    }
    
    void clearOutput() {
        buffer_.str("");
        buffer_.clear();
    }
    
    std::vector<std::string> getLines() const {
        std::vector<std::string> lines;
        std::string content = buffer_.str();
        std::stringstream ss(content);
        std::string line;
        
        while (std::getline(ss, line)) {
            lines.push_back(line);
        }
        
        return lines;
    }
    
    // Настройки
    void setEcho(bool enable) {
        echo_to_stdout_ = enable;
    }
    
    void setTimestamp(bool enable) {
        timestamp_enabled_ = enable;
    }
};

// Глобальный экземпляр Serial
#ifdef ARDUINO_TEST_MODE
    extern FakeSerial Serial;
#else
    #ifdef __AVR__
        // На Arduino используем настоящий Serial
        #include <Arduino.h>
    #else
        // На ПК используем нашу заглушку
        extern FakeSerial Serial;
    #endif
#endif

#endif // FAKE_SERIAL_H