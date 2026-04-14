im_test: IM_tester.cpp processor.o
	g++ -Wall IM_tester.cpp processor.o -o im_test

processor.o: processor.cpp
	g++ -Wall -c processor.cpp

clean:
	rm -f im_test processor.o