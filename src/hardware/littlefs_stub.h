#ifndef LITTLEFS_STUB_H
#define LITTLEFS_STUB_H

// #define LITTLEFS_STUB_DEBUG // Включить отладочные сообщения

#include <string>
#include <fstream>
#include <vector>
#include <memory>
#include <chrono>
#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <algorithm>
#include "arduino_compat.h"

namespace fs {
    enum SeekMode {
        SeekSet = 0,
        SeekCur = 1,
        SeekEnd = 2
    };

    class File {
    private:
        std::fstream file_;
        std::string path_;
        std::string name_;
        std::string mode_;
        bool is_directory_;
        size_t position_;
        size_t size_;
        bool exists_;
        DIR* dir_ptr_;
        
        // Вспомогательные функции для работы с файлами

        static bool checkExists(const std::string &path) {
          struct stat st;
          int result = stat(path.c_str(), &st);
#ifdef LITTLEFS_STUB_DEBUG
        std::cout << "[DEBUG] Call checkExists() of path: '" << path << "'"
                  << std::endl;
          std::cout << "[DEBUG] stat('" << path << "') returned " << result;
          if (result == 0) {
            std::cout << ", file exists" << std::endl;
          } else {
            std::cout << ", error: " << strerror(errno) << " (errno=" << errno
                      << ")" << std::endl;
          }
#endif
          return result == 0;
        }

        // static bool checkExists(const std::string& path) {
        //     struct stat st;
        //     return stat(path.c_str(), &st) == 0;
        // }
        
        static bool isDir(const std::string& path) {
            struct stat st;
            if (stat(path.c_str(), &st) == 0) {
                return S_ISDIR(st.st_mode);
            }
            return false;
        }
        
        static size_t getFileSize(const std::string& path) {
            struct stat st;
            if (stat(path.c_str(), &st) == 0) {
                return st.st_size;
            }
            return 0;
        }

        void initFile() {
#ifdef LITTLEFS_STUB_DEBUG
          std::cout << "[DEBUG] initFile called with path: '" << path_
                    << "', mode: '" << mode_ << "'" << std::endl;
#endif
          // Извлекаем имя файла
          size_t pos = path_.find_last_of('/');
          if (pos != std::string::npos) {
            name_ = path_.substr(pos + 1);
          } else {
            name_ = path_;
          }
#ifdef LITTLEFS_STUB_DEBUG
          std::cout << "[DEBUG] File name extracted: '" << name_ << "'"
                    << std::endl;
#endif
          // Проверяем существование
          exists_ = checkExists(path_);
#ifdef LITTLEFS_STUB_DEBUG
          std::cout << "[DEBUG] checkExists('" << path_
                    << "') = " << (exists_ ? "true" : "false") << std::endl;
#endif

          if (exists_) {
            is_directory_ = isDir(path_);
#ifdef LITTLEFS_STUB_DEBUG
            std::cout << "[DEBUG] isDir('" << path_
                      << "') = " << (is_directory_ ? "true" : "false")
                      << std::endl;
#endif

            if (!is_directory_) {
              size_ = getFileSize(path_);
#ifdef LITTLEFS_STUB_DEBUG
              std::cout << "[DEBUG] File size: " << size_ << " bytes"
                        << std::endl;
#endif
            }
          } else {
            is_directory_ = false;
#ifdef LITTLEFS_STUB_DEBUG
            std::cout
                << "[DEBUG] File doesn't exist, setting is_directory_ = false"
                << std::endl;
#endif
          }

          // Для файлов (не директорий) открываем поток
          if (!is_directory_) {
            std::ios_base::openmode openmode = std::ios::binary;
            if (mode_.find('r') != std::string::npos)
              openmode |= std::ios::in;
            if (mode_.find('w') != std::string::npos)
              openmode |= std::ios::out | std::ios::trunc;
            if (mode_.find('a') != std::string::npos)
              openmode |= std::ios::out | std::ios::app;

#ifdef LITTLEFS_STUB_DEBUG
            std::cout << "[DEBUG] Opening file with mode: ";
            if (openmode & std::ios::in)
              std::cout << "in ";
            if (openmode & std::ios::out)
              std::cout << "out ";
            if (openmode & std::ios::app)
              std::cout << "app ";
            if (openmode & std::ios::trunc)
              std::cout << "trunc ";
            if (openmode & std::ios::binary)
              std::cout << "binary";
            std::cout << std::endl;
            std::cout << "[DEBUG] Opening file: '" << path_ << "'" << std::endl;
#endif
            file_.open(path_, openmode);

            if (file_.is_open()) {
#ifdef LITTLEFS_STUB_DEBUG
              std::cout << "[DEBUG] File opened successfully" << std::endl;
#endif
              position_ = 0;
            } else {
#ifdef LITTLEFS_STUB_DEBUG
              std::cout << "[DEBUG] FAILED to open file!" << std::endl;
              std::cout << "[DEBUG] Error state: ";
              std::cout << "good:" << file_.good() << " ";
              std::cout << "eof:" << file_.eof() << " ";
              std::cout << "fail:" << file_.fail() << " ";
              std::cout << "bad:" << file_.bad() << std::endl;

              // Проверяем почему не открылся
              struct stat st;
              if (stat(path_.c_str(), &st) != 0) {
                std::cout << "[DEBUG] stat() failed: " << strerror(errno)
                          << std::endl;
              } else {
                std::cout << "[DEBUG] stat() succeeded, mode: " << std::oct
                          << st.st_mode << std::dec << std::endl;
                std::cout << "[DEBUG] Is regular file: " << S_ISREG(st.st_mode)
                          << std::endl;
                std::cout << "[DEBUG] Is directory: " << S_ISDIR(st.st_mode)
                          << std::endl;
                std::cout << "[DEBUG] Permissions: " << std::oct
                          << (st.st_mode & 0777) << std::dec << std::endl;
              }
#endif
            }
          } else {
#ifdef LITTLEFS_STUB_DEBUG
            std::cout
                << "[DEBUG] This is a directory, not opening as file stream"
                << std::endl;
#endif
          }
        }

      public:
        File() : is_directory_(false), position_(0), size_(0), exists_(false), dir_ptr_(nullptr) {}
        
        File(const std::string& path, const std::string& mode = "r") 
            : path_(path), mode_(mode), is_directory_(false), 
              position_(0), size_(0), exists_(false), dir_ptr_(nullptr) {
            initFile();
        }
        
        // Конструктор перемещения
        File(File&& other) noexcept 
            : file_(std::move(other.file_)),
              path_(std::move(other.path_)),
              name_(std::move(other.name_)),
              mode_(std::move(other.mode_)),
              is_directory_(other.is_directory_),
              position_(other.position_),
              size_(other.size_),
              exists_(other.exists_),
              dir_ptr_(other.dir_ptr_) {
            
            other.is_directory_ = false;
            other.position_ = 0;
            other.size_ = 0;
            other.exists_ = false;
            other.dir_ptr_ = nullptr;
        }
        
        // Оператор перемещения
        File& operator=(File&& other) noexcept {
            if (this != &other) {
                close();
                if (dir_ptr_) {
                    closedir(dir_ptr_);
                }
                
                file_ = std::move(other.file_);
                path_ = std::move(other.path_);
                name_ = std::move(other.name_);
                mode_ = std::move(other.mode_);
                is_directory_ = other.is_directory_;
                position_ = other.position_;
                size_ = other.size_;
                exists_ = other.exists_;
                dir_ptr_ = other.dir_ptr_;
                
                other.is_directory_ = false;
                other.position_ = 0;
                other.size_ = 0;
                other.exists_ = false;
                other.dir_ptr_ = nullptr;
            }
            return *this;
        }
        
        // Запретить копирование
        File(const File&) = delete;
        File& operator=(const File&) = delete;
        
        ~File() {
            close();
            if (dir_ptr_) {
                closedir(dir_ptr_);
            }
        }
        
        // Чтение
        size_t read(uint8_t* buf, size_t size) {
            if (!file_.is_open() || !file_.good()) return 0;
            
            file_.read(reinterpret_cast<char*>(buf), size);
            size_t bytes_read = file_.gcount();
            position_ += bytes_read;
            return bytes_read;
        }
        
        int read() {
            if (!file_.is_open() || !file_.good()) return -1;
            
            char c;
            file_.get(c);
            if (file_.good()) {
                position_++;
                return static_cast<uint8_t>(c);
            }
            return -1;
        }
        
        // Запись
        size_t write(const uint8_t* buf, size_t size) {
            if (!file_.is_open() || !file_.good()) return 0;
            
            file_.write(reinterpret_cast<const char*>(buf), size);
            position_ += size;
            
            // Обновляем размер файла если нужно
            if (position_ > size_) {
                size_ = position_;
            }
            
            return size;
        }
        
        size_t write(uint8_t c) {
            return write(&c, 1);
        }
        
        size_t write(const char* str) {
            if (!str) return 0;
            return write(reinterpret_cast<const uint8_t*>(str), strlen(str));
        }
        
        size_t write(const char* str, size_t len) {
            if (strlen(str) <= len) {
                return write(str);
            }

            char buffer[len + 1];
            memcpy(buffer, str, len);
            buffer[len] = '\0';
            return write(buffer);
        }

        size_t write(const String& str) {
            return write(str.c_str());
        }
        
        // Печать
        size_t print(const char* str) {
            return write(str);
        }
        
        size_t print(char c) {
            return write(static_cast<uint8_t>(c));
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
                char buffer[20];
                snprintf(buffer, sizeof(buffer), "%lx", n);
                str = buffer;
            } else if (base == OCT) {
                char buffer[20];
                snprintf(buffer, sizeof(buffer), "%lo", n);
                str = buffer;
            } else if (base == BIN) {
                unsigned long un = (n < 0) ? -n : n;
                for (int i = sizeof(n) * 8 - 1; i >= 0; i--) {
                    str += (un & (1UL << i)) ? '1' : '0';
                }
                // Убираем ведущие нули
                size_t pos = str.find_first_not_of('0');
                if (pos != std::string::npos) {
                    str = str.substr(pos);
                } else {
                    str = "0";
                }
            }
            return print(str.c_str());
        }
        
        size_t print(unsigned long n, int base = DEC) {
            std::string str;
            if (base == DEC) {
                str = std::to_string(n);
            } else if (base == HEX) {
                char buffer[20];
                snprintf(buffer, sizeof(buffer), "%lx", n);
                str = buffer;
            } else if (base == OCT) {
                char buffer[20];
                snprintf(buffer, sizeof(buffer), "%lo", n);
                str = buffer;
            }
            return print(str.c_str());
        }
        
        size_t print(double n, int digits = 2) {
            char format[20];
            snprintf(format, sizeof(format), "%%.%df", digits);
            
            char buffer[50];
            snprintf(buffer, sizeof(buffer), format, n);
            return print(buffer);
        }
        
        size_t print(const String& str) {
            return print(str.c_str());
        }
        
        // Печать с новой строкой
        size_t println(const char* str) {
            size_t n = print(str);
            n += println();
            return n;
        }
        
        size_t println(char c) {
            size_t n = print(c);
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
        
        size_t println(const String& str) {
            size_t cnt = print(str);
            cnt += println();
            return cnt;
        }
        
        size_t println() {
            return print("\n");  // Исправлено: используем print вместо write
        }
        
        // Форматированный вывод
        size_t printf(const char* format, ...) {
            va_list args;
            va_start(args, format);
            
            // Определяем размер буфера
            va_list args_copy;
            va_copy(args_copy, args);
            int size = vsnprintf(nullptr, 0, format, args_copy);
            va_end(args_copy);
            
            if (size < 0) {
                va_end(args);
                return 0;
            }
            
            // Выделяем буфер
            std::vector<char> buffer(size + 1);
            vsnprintf(buffer.data(), buffer.size(), format, args);
            va_end(args);
            
            // Записываем в файл
            return print(buffer.data());
        }
        
        // Позиционирование
        bool seek(uint32_t pos, SeekMode mode = SeekSet) {
            if (!file_.is_open()) return false;
            
            std::ios_base::seekdir dir;
            switch (mode) {
                case SeekSet: dir = std::ios::beg; break;
                case SeekCur: dir = std::ios::cur; break;
                case SeekEnd: dir = std::ios::end; break;
                default: return false;
            }
            
            file_.seekg(pos, dir);
            file_.seekp(pos, dir);
            
            if (file_.good()) {
                position_ = file_.tellg();
                return true;
            }
            return false;
        }
        
        size_t position() const {
            return position_;
        }
        
        size_t size() const {
            return size_;
        }
        
        // Закрытие
        void close() {
            if (file_.is_open()) {
                file_.close();
            }
        }
        
        // Проверки
        operator bool() const {
            return file_.is_open() && file_.good();
        }
        
        bool isDirectory() const {
            return is_directory_;
        }
        
        const char* name() const {
            return name_.c_str();
        }
        
        const char* fullName() const {
            return path_.c_str();
        }
        
        bool available() const {
            return file_.is_open() && file_.good() && (position_ < size_);
        }
        
        // Для директорий
        File openNextFile(const char* mode = "r") {
            if (!is_directory_) return File();
            
            if (dir_ptr_ == nullptr) {
                dir_ptr_ = opendir(path_.c_str());
                if (!dir_ptr_) return File();
            }
            
            struct dirent* entry;
            while ((entry = readdir(dir_ptr_)) != nullptr) {
                // Пропускаем . и ..
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                    continue;
                }
                
                std::string full_path = path_ + "/" + entry->d_name;
                return File(full_path, mode);
            }
            
            // Конец директории
            if (dir_ptr_) {
                closedir(dir_ptr_);
                dir_ptr_ = nullptr;
            }
            
            return File();
        }
        
        void rewindDirectory() {
            if (dir_ptr_) {
                closedir(dir_ptr_);
                dir_ptr_ = nullptr;
            }
        }

    // DEBUG: METHODS
    public:
    // В публичной секции класса File добавьте:

        // Метод для отладки - выводит состояние всех переменных
        void debugInfo(const char* prefix = "") const {
            std::cout << prefix << "=== File Debug Info ===" << std::endl;
            std::cout << prefix << "Path: '" << path_ << "'" << std::endl;
            std::cout << prefix << "Name: '" << name_ << "'" << std::endl;
            std::cout << prefix << "Mode: '" << mode_ << "'" << std::endl;
            std::cout << prefix << "Is directory: " << (is_directory_ ? "YES" : "NO") << std::endl;
            std::cout << prefix << "Exists: " << (exists_ ? "YES" : "NO") << std::endl;
            std::cout << prefix << "Size: " << size_ << " bytes" << std::endl;
            std::cout << prefix << "Position: " << position_ << std::endl;
            
            // Информация о файловом потоке
            std::cout << prefix << "File stream: ";
            if (file_.is_open()) {
                std::cout << "OPEN";
                std::cout << " (good: " << file_.good();
                std::cout << ", eof: " << file_.eof();
                std::cout << ", fail: " << file_.fail();
                std::cout << ", bad: " << file_.bad() << ")";
                
                // Текущая позиция в потоке
                // std::streampos pos = file_.tellg();
                // std::cout << "\n" << prefix << "Stream position: " << pos;
            } else {
                std::cout << "CLOSED";
            }
            std::cout << std::endl;
            
            // Для директорий
            if (is_directory_) {
                std::cout << prefix << "Directory pointer: ";
                if (dir_ptr_) {
                    std::cout << "VALID";
                } else {
                    std::cout << "NULL";
                }
                std::cout << std::endl;
            }
            
            // Проверка доступности файла на диске (еще раз для уверенности)
            struct stat st;
            if (stat(path_.c_str(), &st) == 0) {
                std::cout << prefix << "Disk info - ";
                if (S_ISDIR(st.st_mode)) {
                    std::cout << "Is a directory";
                } else if (S_ISREG(st.st_mode)) {
                    std::cout << "Is a regular file, size: " << st.st_size << " bytes";
                    std::cout << ", permissions: " << std::oct << (st.st_mode & 0777) << std::dec;
                } else {
                    std::cout << "Is a special file";
                }
                std::cout << std::endl;
            } else {
                std::cout << prefix << "Disk info - File not found on disk (errno: " << errno << ")" << std::endl;
            }
            
            std::cout << prefix << "Operator bool(): " << (operator bool() ? "TRUE" : "FALSE") << std::endl;
            std::cout << prefix << "Available(): " << (available() ? "TRUE" : "FALSE") << std::endl;
            std::cout << prefix << "=========================" << std::endl;
        }

        // Упрощенная версия для быстрой отладки
        void debugShort() const {
            std::cout << "File['" << name_ << "']";
            std::cout << " path:'" << path_ << "'";
            std::cout << " mode:'" << mode_ << "'";
            std::cout << " open:" << (file_.is_open() ? "Y" : "N");
            std::cout << " dir:" << (is_directory_ ? "Y" : "N");
            std::cout << " exists:" << (exists_ ? "Y" : "N");
            std::cout << " size:" << size_;
            std::cout << " pos:" << position_;
            std::cout << std::endl;
        }
    };

    class LittleFSClass {
    private:
      std::string base_path_;
      bool mounted_;

      // Статическая функция mkdir из sys/stat.h (не путать с методом класса)
      static bool sys_mkdir(const char *path, mode_t mode) {
        return ::mkdir(path, mode) == 0 || errno == EEXIST;
      }

      static bool sys_rmdir(const char *path) { return ::rmdir(path) == 0; }

      static bool createDirRecursive(const std::string &path) {
#ifdef LITTLEFS_STUB_DEBUG
        std::cout << "[DEBUG] createDirRecursive('" << path << "') called"
                  << std::endl;
#endif

        if (path.empty() || path == "/") {
#ifdef LITTLEFS_STUB_DEBUG
          std::cout << "[DEBUG] Path is empty or root, returning true"
                    << std::endl;
#endif
          return true;
        }

#ifdef LITTLEFS_STUB_DEBUG
        std::cout << "[DEBUG] Creating directories for path: '" << path << "'"
                  << std::endl;
#endif

        std::string current;

        for (size_t i = 0; i <= path.length(); i++) {
          // Если нашли слеш или конец строки
          if (i == path.length() || path[i] == '/') {
            if (!current.empty()) {
#ifdef LITTLEFS_STUB_DEBUG
              std::cout << "[DEBUG]   Checking/creating: '" << current << "'"
                        << std::endl;
#endif

              struct stat st;
              if (stat(current.c_str(), &st) != 0) {
    // Директории нет, создаем
#ifdef LITTLEFS_STUB_DEBUG
                std::cout << "[DEBUG]     Directory doesn't exist, creating..."
                          << std::endl;
                std::cout << "[DEBUG]     Calling mkdir('" << current
                          << "', 0755)" << std::endl;
#endif

                int result = ::mkdir(current.c_str(), 0755);
#ifdef LITTLEFS_STUB_DEBUG
                std::cout << "[DEBUG]     mkdir returned: " << result
                          << std::endl;
#endif

                if (result != 0 && errno != EEXIST) {
                  std::cerr << "[ERROR]     FAILED to create '" << current
                            << "': " << strerror(errno) << " (errno=" << errno
                            << ")" << std::endl;
                  return false;
                }

                if (result == 0) {
#ifdef LITTLEFS_STUB_DEBUG
                  std::cout << "[DEBUG]     Created successfully" << std::endl;
#endif
                } else if (errno == EEXIST) {
#ifdef LITTLEFS_STUB_DEBUG
                  std::cout << "[DEBUG]     Already exists (EEXIST)"
                            << std::endl;
#endif
                }
              } else if (!S_ISDIR(st.st_mode)) {
                std::cerr << "[ERROR]     EXISTS BUT NOT A DIRECTORY"
                          << std::endl;
                std::cerr << "[ERROR]     st_mode: " << std::oct << st.st_mode
                          << std::dec << std::endl;
                return false;
              } else {
#ifdef LITTLEFS_STUB_DEBUG
                std::cout << "[DEBUG]     Already exists" << std::endl;
#endif
              }
            }

            // Добавляем слеш для следующей части
            if (i < path.length() && path[i] == '/') {
              current += "/";
            }
          } else {
            current += path[i];
          }
        }

        // Проверяем финальный результат
        struct stat final_st;
        if (stat(path.c_str(), &final_st) == 0 && S_ISDIR(final_st.st_mode)) {
#ifdef LITTLEFS_STUB_DEBUG
          std::cout << "[DEBUG] SUCCESS: Directory '" << path
                    << "' exists and is a directory" << std::endl;
#endif
          return true;
        } else {
          std::cerr << "[ERROR] FAILED: Directory '" << path
                    << "' not created properly" << std::endl;
          return false;
        }
      }

      static bool removeRecursive(const std::string &path) {
        // Сначала проверяем, это файл или директория
        struct stat st;
        if (stat(path.c_str(), &st) != 0) {
          return false;
        }

        if (!S_ISDIR(st.st_mode)) {
          // Это файл - просто удаляем
          return unlink(path.c_str()) == 0;
        }

        // Это директория - удаляем рекурсивно
        DIR *dir = opendir(path.c_str());
        if (!dir) {
          return false;
        }

        struct dirent *entry;
        while ((entry = readdir(dir)) != nullptr) {
          if (strcmp(entry->d_name, ".") == 0 ||
              strcmp(entry->d_name, "..") == 0) {
            continue;
          }

          std::string full_path = path + "/" + entry->d_name;

          if (stat(full_path.c_str(), &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
              removeRecursive(full_path);
            } else {
              unlink(full_path.c_str());
            }
          }
        }

        closedir(dir);
        return sys_rmdir(path.c_str());
      }

      static bool pathExists(const std::string &path) {
        struct stat st;
        return stat(path.c_str(), &st) == 0;
      }
        
    public:
        LittleFSClass() : mounted_(false) {
            base_path_ = "./littlefs_data";
        }

        /**
         * @brief Инициализация LittleFS(fake) в указанной директориию
         * 
         * @param formatOnFail 
         * @param basePath Путь к директории для монтирования LittleFS (по умолчанию ./littlefs)
         * @param maxOpenFiles 
         * @param partitionLabel 
         * @return true 
         * @return false 
         */
        bool begin(bool formatOnFail = false,
                   const char *basePath = "./littlefs", uint8_t maxOpenFiles = 5,
                   const char *partitionLabel = NULL) {
          (void)maxOpenFiles;
          (void)partitionLabel;
          (void)formatOnFail;

          // Всегда используем /tmp для тестов
          //   base_path_ = "/tmp/littlefs_test_" + std::to_string(getpid());
          //   base_path_ = "/tmp/littlefs_test";
          if (basePath && strlen(basePath) > 0) {
            base_path_ = basePath;
          }

          std::cout << "[DEBUG] Trying to mount LittleFS at: " << base_path_
                    << std::endl;

          // Создаем базовую директорию рекурсивно
          if (!createDirRecursive(base_path_)) {
            std::cerr << "Failed to create base directory: " << base_path_
                      << std::endl;

            if (formatOnFail) {
#ifdef LITTLEFS_STUB_DEBUG
              std::cout << "Trying format..." << std::endl;
#endif
              return format();

            } else {
#ifdef LITTLEFS_STUB_DEBUG
              std::cout << "[DEBUG] Mount failed and formatOnFail=false"
                        << std::endl;
#endif
            }
            return false;
          }

          mounted_ = true;
          std::cout << "LittleFS mounted successfully at: " << base_path_
                    << std::endl;
          return true;
        }

        void end() {
            mounted_ = false;
        }

        bool format() {
#ifdef LITTLEFS_STUB_DEBUG
          std::cout << "[DEBUG] Formatting LittleFS at: " << base_path_
                    << std::endl;
#endif

          // Удаляем все файлы в директории
          if (pathExists(base_path_)) {
            #ifdef LITTLEFS_STUB_DEBUG
            std::cout << "[DEBUG] Removing existing files in: " << base_path_
                      << std::endl;
                #endif
            if (!removeRecursive(base_path_)) {
#ifdef LITTLEFS_STUB_DEBUG
              std::cout << "[DEBUG] Failed to remove existing files in: "
                        << base_path_ << std::endl;
#endif
              return false;
            }
          }

          // Создаем заново
          if (!sys_mkdir(base_path_.c_str(), 0755)) {
#ifdef LITTLEFS_STUB_DEBUG
            std::cout << "[DEBUG] Failed to create base directory: " << base_path_
                      << std::endl;
#endif
            return false;
          }

          mounted_ = true;
#ifdef LITTLEFS_STUB_DEBUG
          std::cout << "[DEBUG] LittleFS formatted at: " << base_path_
                    << std::endl;
#endif
          return true;
        }

        File open(const char *path, const char *mode = "r") {
          if (!mounted_) {
#ifdef LITTLEFS_STUB_DEBUG
            std::cout << "[ERROR] LittleFS not mounted!" << std::endl;
#endif
            return File();
          }

#ifdef LITTLEFS_STUB_DEBUG
          std::cout << "[DEBUG] open() called with path: '" << path
                    << "', mode: '" << mode << "'" << std::endl;
          std::cout << "[DEBUG] base_path_: '" << base_path_ << "'"
                    << std::endl;
#endif

          std::string full_path = base_path_ + "/" + path;

#ifdef LITTLEFS_STUB_DEBUG
          std::cout << "[DEBUG] full_path before normalize: '" << full_path
                    << "'" << std::endl;
#endif

          // Нормализуем путь
          std::string normalized;
          bool last_was_slash = false;
          for (char c : full_path) {
            if (c == '/') {
              if (!last_was_slash) {
                normalized += c;
                last_was_slash = true;
              }
            } else {
              normalized += c;
              last_was_slash = false;
            }
          }

          // Убираем слеш в конце если есть
          if (normalized.length() > 1 && normalized.back() == '/') {
            normalized.pop_back();
          }
#ifdef LITTLEFS_STUB_DEBUG
          std::cout << "[DEBUG] normalized path: '" << normalized << "'"
                    << std::endl;
#endif

          // Создаем директории если нужно (для режима записи)
          if (strchr(mode, 'w') || strchr(mode, 'a')) {
            // Извлекаем путь к директории
            size_t pos = normalized.find_last_of('/');
            if (pos != std::string::npos) {
              std::string dir_path = normalized.substr(0, pos);

#ifdef LITTLEFS_STUB_DEBUG
              std::cout << "[DEBUG] Directory path to create: '" << dir_path
                        << "'" << std::endl;
              std::cout << "[DEBUG] dir_path.empty(): " << dir_path.empty()
                        << std::endl;
#endif

              if (!dir_path.empty()) {
#ifdef LITTLEFS_STUB_DEBUG
                std::cout << "[DEBUG] Calling createDirRecursive('" << dir_path
                          << "')" << std::endl;
#endif
                if (!createDirRecursive(dir_path)) {
                  std::cerr << "[ERROR] Failed to create directory path"
                            << std::endl;
                  return File();
                }
#ifdef LITTLEFS_STUB_DEBUG
                std::cout << "[DEBUG] createDirRecursive succeeded"
                          << std::endl;
#endif

              } else {
#ifdef LITTLEFS_STUB_DEBUG
                std::cout
                    << "[DEBUG] dir_path is empty, skipping directory creation"
                    << std::endl;
#endif
              }
            } else {
#ifdef LITTLEFS_STUB_DEBUG
              std::cout << "[DEBUG] No '/' found in normalized path"
                        << std::endl;
#endif
            }
          }
#ifdef LITTLEFS_STUB_DEBUG
          std::cout << "[DEBUG] Creating File object with path: '" << normalized
                    << "', mode: '" << mode << "'" << std::endl;
#endif
          return File(normalized, mode);
        }

        File open(const String& path, const char* mode = "r") {
            return open(path.c_str(), mode);
        }
        
        bool exists(const char* path) {
            if (!mounted_) return false;
            
            std::string full_path = base_path_ + "/" + path;
            return pathExists(full_path);
        }
        
        bool exists(const String& path) {
            return exists(path.c_str());
        }
        
        bool remove(const char* path) {
            if (!mounted_) return false;
            
            std::string full_path = base_path_ + "/" + path;
            
            if (pathExists(full_path)) {
                return removeRecursive(full_path);
            }
            return false;
        }
        
        bool remove(const String& path) {
            return remove(path.c_str());
        }
        
        bool rename(const char* pathFrom, const char* pathTo) {
            if (!mounted_) return false;
            
            std::string from_path = base_path_ + "/" + pathFrom;
            std::string to_path = base_path_ + "/" + pathTo;
            
            if (pathExists(from_path)) {
                // Создаем директории для пути назначения
                size_t pos = to_path.find_last_of('/');
                if (pos != std::string::npos) {
                    std::string dir_path = to_path.substr(0, pos);
                    if (!dir_path.empty()) {
                        createDirRecursive(dir_path);
                    }
                }
                
                // Переименовываем
                return ::rename(from_path.c_str(), to_path.c_str()) == 0;
            }
            return false;
        }
        
        bool rename(const String& pathFrom, const String& pathTo) {
            return rename(pathFrom.c_str(), pathTo.c_str());
        }
        
        // Методы класса
        bool mkdir(const char* path) {
            if (!mounted_) return false;
            
            std::string full_path = base_path_ + "/" + path;
            return createDirRecursive(full_path);
        }
        
        bool mkdir(const String& path) {
            return mkdir(path.c_str());
        }
        
        bool rmdir(const char* path) {
            return remove(path);
        }
        
        bool rmdir(const String& path) {
            return remove(path.c_str());
        }
        
        // Информация
        size_t totalBytes() {
            if (!mounted_) return 0;
            
            size_t total = 0;
            calculateDirSize(base_path_, total);
            return total;
        }
        
        size_t usedBytes() {
            return totalBytes();
        }
        
        size_t freeBytes() {
            // Эмулируем 1MB свободного места
            size_t used = usedBytes();
            return (1024 * 1024 > used) ? (1024 * 1024 - used) : 0;
        }
        
        // Получить путь к данным
        std::string getBasePath() const {
            return base_path_;
        }
        
        // Очистить все данные
        void clearAll() {
            if (pathExists(base_path_)) {
                removeRecursive(base_path_);
                sys_mkdir(base_path_.c_str(), 0755);
            }
        }
        
        // Список файлов
        std::vector<std::string> listFiles() {
            std::vector<std::string> files;
            if (!mounted_) return files;
            
            listFilesRecursive(base_path_, base_path_, files);
            return files;
        }
        
    private:
        void calculateDirSize(const std::string& path, size_t& total) {
            DIR* dir = opendir(path.c_str());
            if (!dir) return;
            
            struct dirent* entry;
            struct stat st;
            
            while ((entry = readdir(dir)) != nullptr) {
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                    continue;
                }
                
                std::string full_path = path + "/" + entry->d_name;
                
                if (stat(full_path.c_str(), &st) == 0) {
                    if (S_ISDIR(st.st_mode)) {
                        calculateDirSize(full_path, total);
                    } else {
                        total += st.st_size;
                    }
                }
            }
            
            closedir(dir);
        }
        
        void listFilesRecursive(const std::string& base, const std::string& path, std::vector<std::string>& files) {
            DIR* dir = opendir(path.c_str());
            if (!dir) return;
            
            struct dirent* entry;
            
            while ((entry = readdir(dir)) != nullptr) {
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                    continue;
                }
                
                std::string full_path = path + "/" + entry->d_name;
                std::string relative_path = full_path.substr(base.length());
                
                files.push_back(relative_path);
                
                // Проверяем, является ли это директорией
                struct stat st;
                if (stat(full_path.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
                    listFilesRecursive(base, full_path, files);
                }
            }
            
            closedir(dir);
        }
    };

    // Глобальный экземпляр
    extern LittleFSClass LittleFS;
    
} // namespace fs

using fs::LittleFS;
using fs::File;
using fs::SeekSet;
using fs::SeekCur;
using fs::SeekEnd;

#endif // LITTLEFS_STUB_H