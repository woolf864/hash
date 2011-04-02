all : asd

asd : asd.cc test.cc
	clang++ -O2 asd.cc test.cc timer.cc -o asd 
	
del :
	rm asd
	rm asd3
debug :
	g++ -g -D _SUNOS asd.cc test.cc /home/common/dyd/aisdi/hash/timer.cc -o asd_debug 
	gdb asd_debug

view:
	lynx /home/common/dyd/aisdi/hash/info/index.html
