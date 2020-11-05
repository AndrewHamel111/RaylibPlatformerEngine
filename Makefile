#files_game_PRE = dool.cpp environment.cpp main.cpp network.cpp jute.cpp operators.cpp player.cpp
#files_editor_PRE = environment.cpp editor.cpp operators.cpp player.cpp
files_game = $(CURRENT_DIRECTORY)\*.cpp
#files_game = $(CURRENT_DIRECTORY)\$(files_game_PRE)
#files_editor = $$(CURRENT_DIRECTORY)\$(files_editor_PRE)
#output = main.exe
RAYLIB_PATH=C:\raylb\raylib
COMPILER_PATH=C:\raylib\mingw\bin
CC=$(COMPILER_PATH)\g++
CFLAGS=C:\raylib\raylib\src\raylib.rc.data -s -static -Os -std=c++11 -Wall -Iexternal -DPLATFORM_DESKTOP -Wl,-subsystem,windows
# comment out -Wl,-subsystem,windows to enable console
LDFLAGS=-lraylib -lopengl32 -lgdi32 -lwinmm -lws2_32

Release:
	$(CC) -o main.exe $(files_game) $(CFLAGS) $(LDFLAGS)
	main.exe

Editor:
	$(CC) -o editor.exe $(files_editor) $(CFLAGS) $(LDFLAGS)
	editor.exe
