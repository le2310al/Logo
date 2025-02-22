CC=gcc
COMMON= -Wall -Wextra -Wfloat-equal -Wpedantic -Wvla -std=c99 -Werror
DEBUG= -g3
SANITIZE= $(COMMON) -fsanitize=undefined -fsanitize=address $(DEBUG)
VALGRIND= $(COMMON) $(DEBUG)
PRODUCTION= $(COMMON) -O3
SDL= $(COMMON)  $(shell pkg-config --cflags --libs sdl2)  -lSDL2_image $(SANTIZE)

all: parse_s interpret_s visualise_s

parse_s: Parse/parse.h Parse/parse.c
	$(CC) Parse/parse.c -o parse_s -I./Parse $(SANITIZE) -lm

interpret_s: Interpret/interpret.h Interpret/interpret.c
	$(CC)  Interpret/interpret.c -o interpret_s -I./Interpret $(SANITIZE) -lm

test_s: Testing/test_interpret.c Testing/test_interpret.h 
	$(CC) Testing/test_interpret.c -o test_s -I./Testing $(SANITIZE) -lm

test: test_s
	./test_s

visualise_s: Visualise/visualise.h Visualise/visualise.c 
	$(CC)  Visualise/visualise.c -o visualise_s -I./Visualise $(SDL) -lm

render: visualise_s
	./visualise_s Instructions/shapes.ttl render

run: all
	./parse_s Instructions/empty.ttl
	./parse_s Instructions/forward.ttl
	./parse_s Instructions/ok_parse_fail_interpret.ttl
	./parse_s Instructions/set1.ttl
	./parse_s Instructions/donothing.ttl
	./parse_s Instructions/set2.ttl
	./parse_s Instructions/turn.ttl
	./parse_s Instructions/spiral.ttl
	./parse_s Instructions/octagon1.ttl
	./parse_s Instructions/octagon2.ttl
	./parse_s Instructions/tunnel.ttl
	./parse_s Instructions/labyrinth.ttl
	./parse_s Instructions/hypno.ttl
	./parse_s Instructions/5x5.ttl
	./parse_s Instructions/downarrow.ttl
	./interpret_s Instructions/empty.ttl out_empty.txt
	./interpret_s Instructions/forward.ttl out_forward.txt
	./interpret_s Instructions/set1.ttl out_set1.txt
	./interpret_s Instructions/donothing.ttl out_donothing.txt
	./interpret_s Instructions/fail_parse_ok_interpret.ttl out_fail_parse_ok_interpret.txt
	./interpret_s Instructions/set2.ttl out_set2.txt
	./interpret_s Instructions/turn.ttl out_turn.txt
	./interpret_s Instructions/spiral.ttl out_spiral.txt
	./interpret_s Instructions/octagon1.ttl out_octagon1.txt
	./interpret_s Instructions/octagon2.ttl out_octagon2.txt
	./interpret_s Instructions/tunnel.ttl out_tunnel.txt
	./interpret_s Instructions/labyrinth.ttl out_labyrinth.txt
	./interpret_s Instructions/hypno.ttl out_hypno.txt
	./interpret_s Instructions/5x5.ttl out_5x5.txt
	./interpret_s Instructions/downarrow.ttl out_downarrow.txt
	./interpret_s Instructions/garbage.ttl out_garbage.txt
	./interpret_s Instructions/empty.ttl out_empty.ps
	./interpret_s Instructions/forward.ttl out_forward.ps
	./interpret_s Instructions/set1.ttl out_set1.ps
	./interpret_s Instructions/donothing.ttl out_donothing.ps
	./interpret_s Instructions/fail_parse_ok_interpret.ttl out_fail_parse_ok_interpret.ps
	./interpret_s Instructions/set2.ttl out_set2.ps
	./interpret_s Instructions/turn.ttl out_turn.ps
	./interpret_s Instructions/spiral.ttl out_spiral.ps
	./interpret_s Instructions/octagon1.ttl out_octagon1.ps
	./interpret_s Instructions/octagon2.ttl out_octagon2.ps
	./interpret_s Instructions/tunnel.ttl out_tunnel.ps
	./interpret_s Instructions/labyrinth.ttl out_labyrinth.ps
	./interpret_s Instructions/hypno.ttl out_hypno.ps
	./interpret_s Instructions/5x5.ttl out_5x5.ps
	./interpret_s Instructions/downarrow.ttl out_downarrow.ps
	./visualise_s Instructions/garbage.ttl out_garbage.txt
	./visualise_s Instructions/shapes.ttl out_shapes.txt
	./visualise_s Instructions/racing.ttl out_racing.txt
	./visualise_s Instructions/garbage.ttl out_garbage.ps
	./visualise_s Instructions/shapes.ttl out_shapes.ps
	./visualise_s Instructions/racing.ttl out_racing.ps

clean:
	rm -f parse_s interpret_s test_s visualise_s out_*.txt out_*.ps out_*.pdf

zip: 
	zip -r Turtle_Graphics.zip *