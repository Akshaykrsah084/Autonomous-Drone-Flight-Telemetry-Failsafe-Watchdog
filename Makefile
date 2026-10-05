CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2 -pthread
CPPFLAGS ?= -Iinclude

APP := bin/drone_monitor
TEST := bin/test_logic
SRC := src/main.cpp src/telemetry.cpp src/safety_monitor.cpp src/failsafe.cpp src/watchdog_interface.cpp
OBJ := $(SRC:src/%.cpp=build/%.o)
TEST_OBJ := build/test_logic.o build/telemetry.o build/safety_monitor.o build/failsafe.o

KDIR ?= /lib/modules/$(shell uname -r)/build
DRV_DIR := driver

.PHONY: all app driver test clean help

all: app driver

app: $(APP)

$(APP): $(OBJ) | bin
	$(CXX) $(CXXFLAGS) $(OBJ) -o $@

build/%.o: src/%.cpp | build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

build/test_logic.o: tests/test_logic.cpp | build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(TEST): $(TEST_OBJ) | bin
	$(CXX) $(CXXFLAGS) $(TEST_OBJ) -o $@

test: $(TEST)
	./$(TEST)

driver:
	$(MAKE) -C $(KDIR) M=$(CURDIR)/$(DRV_DIR) modules

build:
	mkdir -p $@

bin:
	mkdir -p $@

clean:
	rm -rf build bin
	$(MAKE) -C $(KDIR) M=$(CURDIR)/$(DRV_DIR) clean 2>/dev/null || true

help:
	@echo "make app    - build C++ application"
	@echo "make test   - run safety-logic tests"
	@echo "make driver - build Linux watchdog kernel module"
	@echo "make all    - build application and driver"
	@echo "make clean  - remove generated files"
