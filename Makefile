all:
	cc -std=c99 lispy.c mpc.c -o lispy

clean:
	rm -rf *~ *.o lispy
