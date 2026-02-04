CXX = g++
CXXFLAGS = -std=c++11 -I./src -I./src/hardware -I./src/test
CXXVARIABLE = -DARDUINO_TEST_MODE
TARGET = arduino_test
PATH_TARGET = target/

# Исходные файлы
SRCS = src/logic/math_operations.cpp \
       src/logic/string_example.cpp \
       src/hardware/fake_serial.cpp \
       main.cpp

TEST_SRCS = test_string_compatibility.cpp


test:
	${CXX} ${CXXFLAGS} src/test/test_string_compatibility.cpp -o ${PATH_TARGET}out
	${PATH_TARGET}out

test_serial:
	${CXX} ${CXXFLAGS} $(CXXVARIABLE) src/test/fake_serial_test.cpp -o ${PATH_TARGET}fake_serial_test
	${PATH_TARGET}fake_serial_test

test_replace:
	${CXX} ${CXXFLAGS} $(CXXVARIABLE) src/test/test_replace.cpp -o ${PATH_TARGET}test_replace
	${PATH_TARGET}test_replace

test_littlefs: src/test/test_littlefs.cpp src/hardware/littlefs_stub.cpp
	${CXX} ${CXXFLAGS} $(CXXVARIABLE) -o ${PATH_TARGET}$@ $^
	${PATH_TARGET}$@

clean:
	rm -f ${PATH_TARGET}*

.PHONY: all run test clean test_serial


#g++ -std=c++11 -I./src -I./src/hardware -DARDUINO_TEST_MODE -o serial_test fake_serial.cpp serial_example.cpp && target/serial_test