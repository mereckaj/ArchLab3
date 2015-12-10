FLAGS = -Wall -march=native -mtune=native -std=c++11
CMP = g++ 

FILES = *.cpp
OUT_NAME = ArchLab3
LINKS = -lpthread

all:
	$(CMP) $(FLAGS) $(FILES) $(LINKS) -o $(OUT_NAME)

