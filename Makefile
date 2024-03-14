all: make-test-data smallest-free-number


make-test-data:
	gcc -o make-test-data make-test-data.c


smallest-free-number:
	gcc -o smallest-free-number smallest-free-number.c


.PHONY: clean

clean:
	-rm make-test-data
	-rm smallest-free-number
	-clean
