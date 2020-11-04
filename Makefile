files = $(CURRENT_DIRECTORY)\*.cpp
output = main.exe
RAYLIB_PATH=C:\raylb\raylib
COMPILER_PATH=C:\raylib\mingw\bin
CC=$(COMPILER_PATH)\g++
CFLAGS=C:\raylib\raylib\src\raylib.rc.data -s -static -Os -std=c++11 -Wall -Iexternal -DPLATFORM_DESKTOP -Wl,-subsystem,windows
# comment out -Wl,-subsystem,windows to enable console
LDFLAGS=-lraylib -lopengl32 -lgdi32 -lwinmm -lws2_32

Release:
	$(CC) -o $(output) $(files) $(CFLAGS) $(LDFLAGS)
	$(output)
