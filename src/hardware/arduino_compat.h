#ifndef ARDUINO_COMPAT_H
#define ARDUINO_COMPAT_H

// Определяем на какой платформе компилируем
#ifdef __AVR__
// На Arduino - используем родные заголовки
#include <Arduino.h>
#include <WString.h>
#include <avr/pgmspace.h>
#else
// На ПК - используем заглушки

// Базовые типы
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

// Заглушки для Arduino типов
typedef uint8_t byte;
typedef bool boolean;

#define HIGH 0x1
#define LOW 0x0
#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2
#define PI 3.14159265358979323846
#define TWO_PI 6.28318530717958647693
#define DEG_TO_RAD 0.01745329251994329577
#define RAD_TO_DEG 57.2957795130823208768
#define SERIAL 0x0
#define DISPLAY 0x1

// Определяем константы как в Arduino
#ifndef DEC
#define DEC 10
#endif

#ifndef HEX
#define HEX 16
#endif

#ifndef OCT
#define OCT 8
#endif

#ifndef BIN
#define BIN 2
#endif

// Наша реализация String
#include "arduino_string_stub.h"

// Заглушки для функций времени
inline void delay(unsigned long ms) {
  // В тестах обычно не нужно реальное ожидание
  // Можно использовать sleep для реалистичности:
  // #include <chrono>
  // #include <thread>
  // std::this_thread::sleep_for(std::chrono::milliseconds(ms));
  (void)ms;
}

inline void delayMicroseconds(unsigned int us) { (void)us; }

inline unsigned long millis() {
  static unsigned long counter = 0;
  return counter += 100; // Имитируем прошедшее время
}

inline unsigned long micros() { return millis() * 1000; }

// Заглушка для random
inline long random(long max) {
  if (max <= 0)
    return 0;
  return rand() % max;
}

inline long random(long min, long max) {
  if (min >= max)
    return min;
  return min + (rand() % (max - min));
}

inline void randomSeed(unsigned long seed) { srand(seed); }

// Математические функции
inline long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Заглушки для макросов
#define constrain(amt, low, high)                                              \
  ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
// #define min(a,b) ((a)<(b)?(a):(b))
// #define max(a,b) ((a)>(b)?(a):(b))
template <typename T> inline T max(T a, T b) { return a > b ? a : b; }

template <typename T> inline T min(T a, T b) { return a < b ? a : b; }
#define abs(x) ((x) > 0 ? (x) : -(x))
#define round(x) ((x) >= 0 ? (long)((x) + 0.5) : (long)((x) - 0.5))
#define radians(deg) ((deg) * DEG_TO_RAD)
#define degrees(rad) ((rad) * RAD_TO_DEG)
#define sq(x) ((x) * (x))

// Макрос F() для совместимости (на ПК ничего не делает)
#ifndef F
#define F(str) (str)
#endif

// Для совместимости с PROGMEM (на ПК игнорируем)
#define PROGMEM
#define PGM_P const char *
#define PGM_VOID_P const void *
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#define pgm_read_float(addr) (*(const float *)(addr))
#define pgm_read_ptr(addr) (*(const void **)(addr))
#define PSTR(str) (str)
#define strcpy_P(dest, src) strcpy(dest, src)
#define strcat_P(dest, src) strcat(dest, src)
#define strcmp_P(a, b) strcmp(a, b)
#define strlen_P(str) strlen(str)

// Заглушки для пинов (пустые функции)
inline void pinMode(uint8_t pin, uint8_t mode) {
  (void)pin;
  (void)mode;
}

inline void digitalWrite(uint8_t pin, uint8_t val) {
  (void)pin;
  (void)val;
}

inline int digitalRead(uint8_t pin) {
  (void)pin;
  return LOW;
}

inline int analogRead(uint8_t pin) {
  (void)pin;
  return 0;
}

inline void analogWrite(uint8_t pin, int val) {
  (void)pin;
  (void)val;
}

inline void analogReference(uint8_t mode) { (void)mode; }

// Заглушки для EEPROM (если нужно)
template <typename T> T EEPROM_read(int address) {
  (void)address;
  return T();
}

template <typename T> void EEPROM_write(int address, T value) {
  (void)address;
  (void)value;
}

template <typename T> void EEPROM_update(int address, T value) {
  (void)address;
  (void)value;
}

#endif

#endif // ARDUINO_COMPAT_H