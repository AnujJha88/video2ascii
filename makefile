CC=g++

CFLAGS=`pkg-config --cflags opencv4 sdl2 libavcodec libavformat libswresample libavutil`

LIBS=`pkg-config --libs opencv4 sdl2 libavcodec libavformat libswresample libavutil` -pthread -latomic

TARGET=ascii_vid

all:
	$(CC) main.cpp video.cpp audio.cpp -o $(TARGET) $(CFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)
