im_test: IM_tester.cpp processor.o
	g++ -Wall IM_tester.cpp processor.o -o im_test

mips_sim: newsource.cpp newprocessor.o
	g++ -std=c++17 -wall newsource.cpp newprocessor.o -o mips_sim.exe

newprocessor.o: newprocessor.cpp
	g++ -std=c++17 -c newprocessor.cpp

clean:
	rm -f im_test processor.o