all: make-test-data smallest-free-number


make-test-data:	make-test-data.c
	gcc -g -Wall -Wextra -o make-test-data make-test-data.c


smallest-free-number:	smallest-free-number.c
	gcc -g -Wall -Wextra -o smallest-free-number smallest-free-number.c


.PHONY: clean

clean:
	-rm make-test-data
	-rm smallest-free-number
	-clean
