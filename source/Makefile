CC  = cc
CXX = g++

#CCFLAGS     = -O0 -ggdb -Wall -std=c11
#CXXFLAGS    = -O0 -ggdb -Wall -std=c++11

CCFLAGS     = -O3 -Wall -std=c11
CXXFLAGS    = -O3 -Wall -std=c++11

LIBRARIES   = -lportaudio -lsndfile
OBJECTS     = $(BUILD_DIR)/*.o

INCLUDE_DIR = ../source
BUILD_DIR   = ../build
TESTS_DIR   = ../tests

all: pre-build libawe tests

pre-build:
	mkdir -p $(BUILD_DIR)

libawe:
	$(CXX) $(CXXFLAGS) -c aweTools.cpp      -o $(BUILD_DIR)/aweTools.o
	$(CXX) $(CXXFLAGS) -c aweLimiter.cpp    -o $(BUILD_DIR)/aweLimiter.o
	$(CXX) $(CXXFLAGS) -c aweSound.cpp      -o $(BUILD_DIR)/aweSound.o
	$(CXX) $(CXXFLAGS) -c aweTrack.cpp      -o $(BUILD_DIR)/aweTrack.o
	$(CXX) $(CXXFLAGS) -c aweMixTrack.cpp   -o $(BUILD_DIR)/aweMixTrack.o
	$(CXX) $(CXXFLAGS) -lsndfile   -c awesndfile.cpp    -o $(BUILD_DIR)/awesndfile.o
	$(CXX) $(CXXFLAGS) -lportaudio -c awePortAudio.cpp  -o $(BUILD_DIR)/awePortAudio.o
	$(CXX) $(CXXFLAGS) -c aweEngine.cpp     -o $(BUILD_DIR)/aweEngine.o

tests:
	$(CXX) $(CXXFLAGS) $(LIBRARIES) $(OBJECTS) -I$(INCLUDE_DIR) $(TESTS_DIR)/play_file.cpp -o $(TESTS_DIR)/play_file

clean:
	rm -frv $(OBJECTS)