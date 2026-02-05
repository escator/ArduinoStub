#ifndef ARDUINO_STRING_STUB_H
#define ARDUINO_STRING_STUB_H

#include <algorithm> // Для tolower/toupper
#include <cstdlib>
#include <cstring> // Добавляем для strncpy
#include <sstream>
#include <string>

#ifdef __AVR__
// На Arduino используем родной String
#include <WString.h>
#else
// На ПК создаем заглушку
class String {
private:
  std::string str_;

public:
  // Конструкторы
  String() : str_() {}
  String(const char *str) : str_(str ? str : "") {}
  String(const std::string &str) : str_(str) {}
  String(const String &other) : str_(other.str_) {}
  String(char c) : str_(1, c) {}

  // Конструкторы для чисел
  String(unsigned char value, unsigned char base = 10) {
    initFromNumber((unsigned long)value, base);
  }

  String(int value, unsigned char base = 10) {
    initFromNumber((long)value, base);
  }

  String(unsigned int value, unsigned char base = 10) {
    initFromNumber((unsigned long)value, base);
  }

  String(long value, unsigned char base = 10) { initFromNumber(value, base); }

  String(unsigned long value, unsigned char base = 10) {
    initFromNumber(value, base);
  }

  String(float value, unsigned char decimalPlaces = 2) {
    std::stringstream ss;
    ss.precision(decimalPlaces);
    ss << std::fixed << value;
    str_ = ss.str();
  }

  String(double value, unsigned char decimalPlaces = 2) {
    std::stringstream ss;
    ss.precision(decimalPlaces);
    ss << std::fixed << value;
    str_ = ss.str();
  }

private:
  void initFromNumber(long value, unsigned char base) {
    if (base == 10) {
      str_ = std::to_string(value);
    } else if (base == 16) {
      std::stringstream ss;
      ss << std::hex << value;
      str_ = ss.str();
    } else {
      str_ = std::to_string(value);
    }
  }

  void initFromNumber(unsigned long value, unsigned char base) {
    if (base == 10) {
      str_ = std::to_string(value);
    } else if (base == 16) {
      std::stringstream ss;
      ss << std::hex << value;
      str_ = ss.str();
    } else {
      str_ = std::to_string(value);
    }
  }

public:
  // Методы совместимости с Arduino
  unsigned int length() const {
    return static_cast<unsigned int>(str_.length());
  }

  unsigned int capacity() const {
    return static_cast<unsigned int>(str_.capacity());
  }

  void reserve(unsigned int size) { str_.reserve(size); }

  char charAt(unsigned int index) const {
    if (index < str_.length()) {
      return str_[index];
    }
    return '\0';
  }

  void setCharAt(unsigned int index, char c) {
    if (index < str_.length()) {
      str_[index] = c;
    }
  }

  int indexOf(char ch) const {
    size_t pos = str_.find(ch);
    return (pos == std::string::npos) ? -1 : static_cast<int>(pos);
  }

  int indexOf(const String &str) const {
    size_t pos = str_.find(str.str_);
    return (pos == std::string::npos) ? -1 : static_cast<int>(pos);
  }

  int indexOf(char ch, unsigned int fromIndex) const {
    size_t pos = str_.find(ch, fromIndex);
    return (pos == std::string::npos) ? -1 : static_cast<int>(pos);
  }

  int lastIndexOf(char ch) const {
    size_t pos = str_.rfind(ch);
    return (pos == std::string::npos) ? -1 : static_cast<int>(pos);
  }

  int lastIndexOf(char ch, unsigned int fromIndex) const {
    size_t pos = str_.rfind(ch, fromIndex);
    return (pos == std::string::npos) ? -1 : static_cast<int>(pos);
  }

  String substring(unsigned int beginIndex) const {
    if (beginIndex >= str_.length())
      return String("");
    return String(str_.substr(beginIndex));
  }

  String substring(unsigned int beginIndex, unsigned int endIndex) const {
    if (beginIndex >= str_.length())
      return String("");
    if (endIndex > str_.length())
      endIndex = static_cast<unsigned int>(str_.length());
    if (beginIndex >= endIndex)
      return String("");
    return String(str_.substr(beginIndex, endIndex - beginIndex));
  }

  void toCharArray(char *buf, unsigned int bufsize,
                   unsigned int index = 0) const {
    if (!buf || bufsize == 0)
      return;

    unsigned int len = length();
    if (index >= len) {
      buf[0] = '\0';
      return;
    }

    unsigned int copyLen = len - index;
    if (copyLen >= bufsize) {
      copyLen = bufsize - 1;
    }

    // Используем безопасное копирование
    const char *src = str_.c_str() + index;
    for (unsigned int i = 0; i < copyLen; ++i) {
      buf[i] = src[i];
    }
    buf[copyLen] = '\0';
  }

  const char *c_str() const { return str_.c_str(); }

  void toLowerCase() {
    std::transform(str_.begin(), str_.end(), str_.begin(),
                   [](unsigned char c) { return std::tolower(c); });
  }

  void toUpperCase() {
    std::transform(str_.begin(), str_.end(), str_.begin(),
                   [](unsigned char c) { return std::toupper(c); });
  }

  void trim() {
    // Удаляем пробелы в начале
    size_t start = str_.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos) {
      str_.clear();
      return;
    }

    // Удаляем пробелы в конце
    size_t end = str_.find_last_not_of(" \t\n\r\f\v");
    str_ = str_.substr(start, end - start + 1);
  }

  long toInt() const {
    if (str_.empty())
      return 0;
    char *end;
    return std::strtol(str_.c_str(), &end, 10);
  }

  float toFloat() const {
    if (str_.empty())
      return 0.0f;
    char *end;
    return std::strtof(str_.c_str(), &end);
  }

  double toDouble() const {
    if (str_.empty())
      return 0.0;
    char *end;
    return std::strtod(str_.c_str(), &end);
  }

  bool equals(const String &rhs) const { return str_ == rhs.str_; }

  bool equalsIgnoreCase(const String &rhs) const {
    if (str_.length() != rhs.str_.length())
      return false;

    std::string s1 = str_;
    std::string s2 = rhs.str_;
    std::transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
    std::transform(s2.begin(), s2.end(), s2.begin(), ::tolower);

    return s1 == s2;
  }

  bool startsWith(const String &prefix) const {
    if (prefix.length() > length())
      return false;
    return str_.substr(0, prefix.length()) == prefix.str_;
  }

  bool endsWith(const String &suffix) const {
    if (suffix.length() > length())
      return false;
    return str_.substr(length() - suffix.length()) == suffix.str_;
  }

  int compareTo(const String &rhs) const { return str_.compare(rhs.str_); }

  // В публичной секции класса String добавьте:

// В классе String в arduino_string_stub.h:

// Замена подстроки
String& replace(const String& find, const String& repl) {
    if (find.length() == 0) return *this;
    
    size_t pos = 0;
    while ((pos = str_.find(find.str_, pos)) != std::string::npos) {
        str_.replace(pos, find.length(), repl.str_);
        pos += repl.length();
    }
    return *this;
}

String& replace(const char* find, const char* repl) {
    return this->replace(String(find), String(repl));
}

String& replace(const char* find, const String& repl) {
    return this->replace(String(find), repl);
}

String& replace(const String& find, const char* repl) {
    return this->replace(find, String(repl));
}

// Замена одного символа
String& replace(char find, char repl) {
    for (size_t i = 0; i < str_.length(); ++i) {
        if (str_[i] == find) {
            str_[i] = repl;
        }
    }
    return *this;
}

// Замена по индексу (Arduino-совместимая версия)
String& replace(unsigned int index, unsigned int count, const String& repl) {
    if (index >= str_.length()) return *this;
    
    // Если count выходит за пределы строки, обрезаем
    if (index + count > str_.length()) {
        count = static_cast<unsigned int>(str_.length()) - index;
    }
    
    str_.replace(index, count, repl.str_);
    return *this;
}

String& replace(unsigned int index, unsigned int count, const char* repl) {
    return this->replace(index, count, String(repl));
}

  // Операторы присваивания
  String &operator=(const String &rhs) {
    if (this != &rhs) {
      str_ = rhs.str_;
    }
    return *this;
  }

  String &operator=(const char *rhs) {
    str_ = rhs ? rhs : "";
    return *this;
  }

  String &operator=(char c) {
    str_ = std::string(1, c);
    return *this;
  }

  // Операторы конкатенации
  String &operator+=(const String &rhs) {
    str_ += rhs.str_;
    return *this;
  }

  String &operator+=(const char *rhs) {
    str_ += (rhs ? rhs : "");
    return *this;
  }

  String &operator+=(char c) {
    str_ += c;
    return *this;
  }

  // Оператор сложения
  String operator+(const String &rhs) const {
    String result = *this;
    result += rhs;
    return result;
  }

  // Операторы сравнения
  bool operator==(const String &rhs) const { return str_ == rhs.str_; }

  bool operator!=(const String &rhs) const { return str_ != rhs.str_; }

  bool operator<(const String &rhs) const { return str_ < rhs.str_; }

  bool operator>(const String &rhs) const { return str_ > rhs.str_; }

  bool operator<=(const String &rhs) const { return str_ <= rhs.str_; }

  bool operator>=(const String &rhs) const { return str_ >= rhs.str_; }

  // Оператор индексации
  char operator[](unsigned int index) const { return charAt(index); }

  char &operator[](unsigned int index) {
    if (index >= str_.length()) {
      static char dummy = '\0';
      return dummy;
    }
    return str_[index];
  }


  // Для отладки
  const std::string &getStdString() const { return str_; }
};

// Внешние операторы
inline String operator+(const char *lhs, const String &rhs) {
  return String(lhs) + rhs;
}

inline String operator+(char lhs, const String &rhs) {
  return String(lhs) + rhs;
}

// Вспомогательные функции
inline String StringFromCharArray(const char *str) { return String(str); }

// Макрос F() - на ПК просто возвращает строку
#define F(str) (str)

// Поддержка вывода в std::ostream для удобства тестов (std::cout)
inline std::ostream &operator<<(std::ostream &os, const String &s) {
  return os << s.c_str();
}


#endif

#endif // ARDUINO_STRING_STUB_H