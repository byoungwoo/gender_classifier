LINK		= g++

INCPATH 	= -I/opt/local/include/
LIBS		= -L/opt/local/lib/ -lcxcore -lcv -lhighgui -lml

SOURCES		= main.cpp

OBJECTS		= $(SOURCES:.cpp=.o)

TARGET		= model

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

$(TARGET) : $(OBJECTS)
	$(LINK) -o $@ $(OBJECTS) $(OBJCOMP) $(LIBS)

all: $(TARGET)
