CFLAGS= -O3 -std=c++17
#CFLAGS= -O0 -g -std=c++17 

BUILDDIR = build
EXECUTABLE = tipc 
CC = g++
INC = -Iinclude
LIBS = -lstdc++
DIRS=$(BUILDDIR)

SRCS = $(wildcard *.cpp)
HEADERS := $(wildcard *.h)
OBJS = $(addprefix $(BUILDDIR)/, $(SRCS:.cpp=.o))

$(BUILDDIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -c $(INC) -o $@ $<

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(BUILDDIR)/*.o $(EXECUTABLE)

# Make the build directory
$(shell mkdir -p $(DIRS))

lines:
	wc -l *.h *.cpp | grep total
