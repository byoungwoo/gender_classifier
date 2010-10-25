LINK		= g++

INCPATH 	= -I/opt/local/include/
LIBS		= -L/opt/local/lib/ -lcxcore -lcv -lhighgui

SOURCES		= ../face_normalizer.cpp \
		main.cpp

OBJECTS		= $(SOURCES:.cpp=.o)

TARGET		= normalizer

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

$(TARGET) : $(OBJECTS)
	$(LINK) -o $@ $(OBJECTS) $(OBJCOMP) $(LIBS)

all: $(TARGET)
