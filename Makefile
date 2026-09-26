CXX= g++ #set compiler
CXXFLAGS= -std=c++17 -Iinclude  #include flags for header files

SRC = $(shell find src -name "*.cpp") #path of all cpp files
OUT= main  

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT) #command to compile

run: all
	./$(OUT)

clean:
	rm -f $	(OUT)