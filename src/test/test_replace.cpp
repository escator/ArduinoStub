#include <iostream>
#include <cassert>
#include "../src/hardware/arduino_compat.h"

void test_replace_no_recursion() {
    std::cout << "Testing replace() without recursion...\n";
    
    // Тест различных комбинаций
    String s1 = "Hello World";
    s1.replace("World", "Arduino");
    assert(s1 == "Hello Arduino");
    std::cout << "✓ replace(const char*, const char*)\n";
    
    String s2 = "test test test";
    String find = "test";
    String repl = "pass";
    s2.replace(find, repl);
    assert(s2 == "pass pass pass");
    std::cout << "✓ replace(const String&, const String&)\n";
    
    String s3 = "foo bar baz";
    s3.replace("bar", String("qux"));
    assert(s3 == "foo qux baz");
    std::cout << "✓ replace(const char*, const String&)\n";
    
    String s4 = "abc def ghi";
    String find2 = "def";
    s4.replace(find2, "jkl");
    assert(s4 == "abc jkl ghi");
    std::cout << "✓ replace(const String&, const char*)\n";
    
    // Цепочка вызовов
    String s5 = "a b c";
    s5.replace(' ', '_').replace("a", "A").replace("c", "C");
    assert(s5 == "A_b_C");
    std::cout << "✓ chained replace calls\n";
    
    std::cout << "✓ All replace tests passed without recursion!\n";
}

int main() {
    std::cout << "=== Testing Fixed replace() Implementation ===\n\n";
    
    try {
        test_replace_no_recursion();
        std::cout << "\n=== Success! No infinite recursion ===\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Error: " << e.what() << std::endl;
        return 1;
    }
}