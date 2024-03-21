# Makefile

all : serverC.cpp serverC.h serverCS.cpp serverCS.h serverEE.cpp serverEE.h serverM.cpp serverM.h client.cpp client.h
	g++ -std=c++11 -o serverC serverC.cpp
	g++ -std=c++11 -o serverCS serverCS.cpp
	g++ -std=c++11 -o serverEE serverEE.cpp
	g++ -std=c++11 -o serverM serverM.cpp
	g++ -std=c++11 -o client client.cpp


#all : serverC.cpp serverC.h serverCS.cpp serverCS.h serverEE.cpp serverEE.h serverM.cpp serverM.h client.cpp client.h
#	g++ -o serverC serverC.cpp
#	g++ -o serverCS serverCS.cpp
#	g++ -o serverEE serverEE.cpp
#	g++ -o serverM serverM.cpp
#	g++ -o client client.cpp
