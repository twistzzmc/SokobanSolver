CC = g++ -Wall

map_in = maps_in/z2.in
map_out = maps_out/z2.out

all: compile run check clean

compile:
	$(CC) -o sokoban.exe sokoban.cpp

run:
	@echo -e "\nRunning sokoban!"
	./sokoban.exe $(map_in) $(map_out)

check:
	@echo -e "\nRunning sokoban CHECK!"
	python judge.py $(map_in) $(map_out)

clean:
	rm sokoban.exe
