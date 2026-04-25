im_test: IM_tester.cpp processor.o
	g++ -Wall IM_tester.cpp processor.o -o im_test

MIPSSimulator: newsource.cpp newprocessor.o
	g++ newsource.cpp newprocessor.o -o MIPSSimulator

newprocessor.o: newprocessor.cpp
	g++ -c newprocessor.cpp

processor.o: processor.cpp
	g++ -Wall -c processor.cpp

clean:
	rm -f im_test processor.o MIPSSimulator