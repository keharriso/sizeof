CFLAGS = -O3 -march=native -fomit-frame-pointer -pipe -s

all: sizeof

clean:
	rm -f sizeof
