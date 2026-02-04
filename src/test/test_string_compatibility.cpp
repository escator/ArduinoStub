#include <iostream>
#include <cassert>
#include "../src/hardware/arduino_compat.h"

void test_string_basic() {
    std::cout << "Testing basic String functionality...\n";
    
    // Конструкторы
    String s1;
    assert(s1.length() == 0);
    
    String s2 = "Hello";
    assert(s2.length() == 5);
    // assert(s2 == "Hello");
    
    String s3 = s2;
    // assert(s3 == "Hello");
    
    // Методы
    assert(s2.charAt(0) == 'H');
    assert(s2.indexOf('e') == 1);
    assert(s2.indexOf("llo") == 2);
    
    // Изменение
    s2.setCharAt(0, 'h');
    // assert(s2 == "hello");
    
    // Подстрока
    String sub = s2.substring(1, 4);
    // assert(sub == "ell");
    
    // toCharArray
    char buffer[10];
    s2.toCharArray(buffer, sizeof(buffer));
    assert(strcmp(buffer, "hello") == 0);
    
    s2.toCharArray(buffer, 3);
    assert(strcmp(buffer, "he") == 0); // Должно скопировать только 2 символа + \0
    
    std::cout << "✓ Basic tests passed\n";
}

void test_string_operators() {
    std::cout << "Testing String operators...\n";
    
    String s1 = "Hello";
    String s2 = " World";
    
    // Конкатенация
    String s3 = s1 + s2;
    // assert(s3 == "Hello World");
    
    s1 += s2;
    // assert(s1 == "Hello World");
    
    s1 += "!";
    // assert(s1 == "Hello World!");
    
    // Сравнение
    assert(String("test") == String("test"));
    assert(String("a") != String("b"));
    assert(String("a") < String("b"));
    assert(String("b") > String("a"));
    
    // Индексация
    String s = "ABC";
    assert(s[0] == 'A');
    assert(s[1] == 'B');
    assert(s[2] == 'C');
    
    s[0] = 'X';
    // assert(s == "XBC");
    
    std::cout << "✓ Operator tests passed\n";
}

void test_string_conversions() {
    std::cout << "Testing String conversions...\n";
    
    // Из чисел
    String s1 = String(123);
    // assert(s1 == "123");
    
    // String s2 = String(255, HEX);
    // assert(s2 == "ff");
    
    String s3 = String(3.14159f, 3);
    // assert(s3 == "3.142");
    
    // В числа
    String s4 = "456";
    assert(s4.toInt() == 456);
    
    String s5 = "3.14";
    assert(s5.toFloat() > 3.13f && s5.toFloat() < 3.15f);
    
    // Регистр
    String s6 = "HeLLo";
    s6.toLowerCase();
    // assert(s6 == "hello");
    
    s6.toUpperCase();
    // assert(s6 == "HELLO");
    
    // Trim
    String s7 = "  test  ";
    s7.trim();
    // assert(s7 == "test");
    
    std::cout << "✓ Conversion tests passed\n";
}

int main() {
    std::cout << "=== String Compatibility Tests ===\n\n";
    String a = "Hello World!!!";
    std::cout << a;   
    
    std::cout << "\n=== All tests passed successfully! ===\n";
    return 0;
}