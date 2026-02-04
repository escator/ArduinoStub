#include "littlefs_stub.h"
#include <iostream>

#include "littlefs_stub.h"
#include <iostream>

int diagnostic_test() {
    std::cout << "=== DEBUG LittleFS Test ===\n\n";
    
    // 1. Показываем состояние до
    std::cout << "1. Before LittleFS.begin():\n";
    system("ls -la /tmp/littlefs_test 2>/dev/null || echo 'Directory does not exist'");
    std::cout << std::endl;
    
    // 2. Инициализация
    std::cout << "2. Calling LittleFS.begin()...\n";
    if (!LittleFS.begin()) {
        std::cout << "FAILED to mount LittleFS!\n";
        return 1;
    }
    std::cout << "SUCCESS: LittleFS mounted\n";
    std::cout << "Base path: " << LittleFS.getBasePath() << "\n\n";
    
    // 3. Показываем состояние после begin
    std::cout << "3. After LittleFS.begin():\n";
    system("ls -la /tmp/littlefs_test 2>/dev/null || echo 'Directory does not exist'");
    std::cout << std::endl;
    
    // 4. Пробуем создать файл
    std::cout << "4. Attempting to open file for writing...\n";
    File writeFile = LittleFS.open("/test/data.txt", "w");
    
    std::cout << "\n5. File object created:\n";
    writeFile.debugInfo("   ");
    
    // 6. Проверяем состояние директории
    std::cout << "\n6. Directory state after open attempt:\n";
    std::string cmd = "find /tmp/littlefs_test -type d 2>/dev/null | sort";
    std::cout << "Running: " << cmd << "\n";
    system(cmd.c_str());
    
    cmd = "find /tmp/littlefs_test -type f 2>/dev/null";
    std::cout << "\nRunning: " << cmd << "\n";
    system(cmd.c_str());
    
    // 7. Пробуем записать если файл открыт
    if (writeFile) {
        std::cout << "\n7. File opened successfully, attempting to write...\n";
        size_t written = writeFile.write("Test", 4);
        std::cout << "write() returned: " << written << "\n";
        
        if (written > 0) {
            writeFile.close();
            std::cout << "File written and closed\n";
        }
    } else {
        std::cout << "\n7. File NOT opened\n";
        
        // Пробуем альтернативный путь
        std::cout << "\n8. Trying alternative...\n";
        
        // Прямое создание директории
        std::cout << "Direct mkdir: mkdir -p /tmp/littlefs_test/test\n";
        system("mkdir -p /tmp/littlefs_test/test && echo 'mkdir succeeded'");
        
        // Прямое создание файла
        std::cout << "Direct file creation:\n";
        std::ofstream direct_file("/tmp/littlefs_test/test/data.txt");
        std::cout << "Direct file open: " << (direct_file.is_open() ? "SUCCESS" : "FAILED") << "\n";
        if (direct_file) {
            direct_file << "Direct test\n";
            direct_file.close();
            std::cout << "Direct write succeeded\n";
        }
    }
    
    LittleFS.end();
    
    // 9. Финальное состояние
    std::cout << "\n9. Final directory state:\n";
    system("tree /tmp/littlefs_test 2>/dev/null || find /tmp/littlefs_test -type f 2>/dev/null");
    
    return 0;
}

void test_littlefs() {
    std::cout << "=== Testing LittleFS Stub ===\n\n";
    
    // Инициализация
    if (!LittleFS.begin()) {
        std::cout << "Failed to mount LittleFS!\n";
        return;
    }
    
    std::cout << "LittleFS mounted at: " << LittleFS.getBasePath() << "\n";
    std::cout << "Free bytes: " << LittleFS.freeBytes() << "\n\n";
    
    // Запись файла
    std::cout << "1. Writing file...\n";
    File writeFile = LittleFS.open("/test/data.txt", "w");
    writeFile.debugShort();
    if (writeFile) {
        writeFile.write("Hello LittleFS!\n");
        writeFile.write("This is a test.\n");
        writeFile.print("Number: ");
        writeFile.println(42);
        writeFile.printf("Float: %.2f\n", 3.14159);
        writeFile.close();
        std::cout << "   File written successfully.\n";
    } else {
        std::cout << "   Failed to open file for writing.\n";
    }
    
    // Чтение файла
    std::cout << "\n2. Reading file...\n";
    File readFile = LittleFS.open("/test/data.txt", "r");
    if (readFile) {
        std::cout << "   File size: " << readFile.size() << " bytes\n";
        std::cout << "   Content:\n";
        
        while (readFile.available()) {
            char buffer[128];
            size_t bytesRead = readFile.read((uint8_t*)buffer, sizeof(buffer)-1);
            buffer[bytesRead] = '\0';
            std::cout << buffer;
        }
        readFile.close();
    }
    
    // Проверка существования
    std::cout << "\n3. File operations...\n";
    if (LittleFS.exists("/test/data.txt")) {
        std::cout << "   File exists.\n";
    }
    
    // Переименование
    LittleFS.rename("/test/data.txt", "/test/renamed.txt");
    std::cout << "   File renamed.\n";
    
    // Список файлов
    std::cout << "\n4. Listing files...\n";
    auto files = LittleFS.listFiles();
    for (const auto& file : files) {
        std::cout << "   - " << file << "\n";
    }
    
    // Удаление
    LittleFS.remove("/test/renamed.txt");
    std::cout << "   File deleted.\n";
    
    // Проверка директории
    std::cout << "\n5. Directory operations...\n";
    File dir = LittleFS.open("/test");
    if (dir && dir.isDirectory()) {
        std::cout << "   Is directory: true\n";
        
        File entry = dir.openNextFile();
        while (entry) {
            std::cout << "   Entry: " << entry.name();
            if (entry.isDirectory()) {
                std::cout << " [DIR]";
            } else {
                std::cout << " (" << entry.size() << " bytes)";
            }
            std::cout << "\n";
            entry = dir.openNextFile();
        }
    }
    
    // Завершение
    LittleFS.end();
    std::cout << "\n=== Test completed ===\n";
}

int main() {
    // test1();
    
    // diagnostic_test();

     test_littlefs();
    
    // Очистка после теста
    // LittleFS.clearAll();
    
    return 0;
}