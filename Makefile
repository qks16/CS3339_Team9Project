im_test: IM_tester.cpp processor.o
	g++ -Wall IM_tester.cpp processor.o -o im_test

MIPSSimulator: newsource.cpp JMprocessor.o
	g++ newsource.cpp JMprocessor.o -o MIPSSimulator

newprocessor.o: newprocessor.cpp
	g++ -c newprocessor.cpp

JMprocessor.o: JMprocessor.cpp
	g++ -c JMprocessor.cpp

processor.o: processor.cpp
	g++ -Wall -c processor.cpp

clean:
	rm -f im_test processor.o  newprocessor.o MIPSSimulator JMprocessor.o