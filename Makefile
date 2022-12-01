CC = mpicc
PROGS = piV2 piV2a piV2b

all: $(PROGS)

clean:
	rm -f $(PROGS)