# Define dependency packages
DEPENDENCIES = gstreamer-rtsp-server-1.0

# Define compile flags
CFLAGS = -g -Wall $(shell pkg-config --cflags $(DEPENDENCIES))

# Define linking flags
LDFLAGS = $(shell pkg-config --libs $(DEPENDENCIES))

# Define a list of source codes
SOURCES = my_gst.c helper.c camera.c param.c main.c

# Define a list of object files based on SOURCES variables
OBJECTS = $(SOURCES:.c=.o)

# Define application's name
EXECUTABLE = outdoor

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@echo "[LD] $@"
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.c
	@echo "[CC] $@"
	@$(CC) $(CFLAGS) -c -o $@ $<


clean:
	rm -f *.o $(EXECUTABLE)
