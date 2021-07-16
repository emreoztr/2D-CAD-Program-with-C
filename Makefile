test: test.c CADlib.c CADlib.h
	gcc test.c CADlib.c -lm -o test
	./test
