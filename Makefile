all : 
	incltouch.x
	make -C src/
	make -C tst/
	make -C obj/

PHONY : clean

clean :
	rm obj/*.?
	rm *.x

