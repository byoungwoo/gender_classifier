LINK		= g++

INCPATH 	= -I/opt/local/include/ -I/usr/include/boost_1_44_0/
LIBS		= -L/opt/local/lib/ -lcxcore -lcv -lhighgui -L/usr/include/boost_1_44_0/stage/lib -lboost_thread 

SOURCES		= face_detector.cpp \
		face_normalizer.cpp \
		feature_detector.cpp \
		feature_detector_task.cpp \
		exceptions.cpp \
		mac_camera.cpp \
		main.cpp

OBJECTS		= $(SOURCES:.cpp=.o)

TARGET		= gender_classifier

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

$(TARGET) : $(OBJECTS)
	$(LINK) -o $@ $(OBJECTS) $(OBJCOMP) $(LIBS)


all: $(TARGET)

.PHONY: clean
clean:
	rm *.o
