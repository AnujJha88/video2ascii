CC=g++

CFLAGS=`pkg-config --cflags opencv4`

LIBS=`pkg-config --libs opencv4`

TARGET=ascii_vid

all:
	$(CC) main.cpp -o $(TARGET) $(CFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)
