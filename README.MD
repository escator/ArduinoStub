# ArduinoStub

Набор заглушек для Arduino модулей. Позволяет писать код и тесты без наличия физического контроллера.

## Требования
- с++11

### Настройка для VS CODE
Настройки необходимы, чтобы VSС мог компилировать код и корректно его отображал.

c_cpp_properties.json
```json
{
    "configurations": [
        {
            "name": "Linux",
            "includePath": [
                "${workspaceFolder}/**"
            ],
            "defines": [
                "ARDUINO_TEST_MODE"
            ],
            "compilerPath": "/usr/bin/g++",
            "cppStandard": "c++11",
            "intelliSenseMode": "gcc-x64"
        }
    ],
    "version": 4
}
```

settings.json
```json
{
    "C_Cpp.default.cppStandard": "c++11",
    "files.associations": {
        "*.ino": "cpp",
        "*.h": "c"
    },
    "C_Cpp.errorSquiggles": "disabled"
}
```

## Serial
Заглушка для serial-монитора с эмуляцией всех команд и вывода в stdout.
- arduino_compat.h
- fake_serial.h

## Эмуляция работы файловой системы LittleFS
Заглушка для работы как с обычной библиотекой LittleFS. Структура файловой системы по-умолчанию разворачиватся в папке littlefs в текущей папке, или по пути указанному в `begin(path)`

## Эмуляция работы String.
Заглушка позволяет работать с Arduino строкой и писать переносимый на контроллер код и тесты.
- arduino_string_stub.h


### Установка
Скопируйте файлы в папку проекта src/hardware или в любую папку достпную компилятору. Все файлы или только нужные. В папке test примеры с демонстрацией работы. Примеры компиляции в Makefile. Создайте в проекте папку target куда будут компилироваться исходники
