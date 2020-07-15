INST_DIR = $(HOME)/bin
EXECUTABLE  = Seedprep

cflags = -std=c++0x -O2 -fopenmp -I${HOME}/anaconda3/include#-Wall -I${HOME}/usr/include

LDLIBS = -L${HOME}/anaconda2/lib -fopenmp -lfftw3 

CFLAGS = $(DBG) $(cflags)

CC = g++

#DBG = -g 

all : $(EXECUTABLE)

FOBJS = FTNorm.o CCDatabase.o SeedRec.o SacRec.o SysTools.o $(EXECUTABLE).o

$(EXECUTABLE) : $(FOBJS)
	$(CC) -o $@ $^ $(LDLIBS) $(CFLAGS)

%.o : %.cpp
	$(CC) $(cflags) -c $<

install : $(EXECUTABLE)
	install -s $(EXECUTABLE) $(INST_DIR)

install_ariadne : $(EXECUTABLE)
	cp $(EXECUTABLE) $(INST_DIR)/$(EXECUTABLE)_ariadne

clean :
	rm -f $(EXECUTABLE) core $(FOBJS)
