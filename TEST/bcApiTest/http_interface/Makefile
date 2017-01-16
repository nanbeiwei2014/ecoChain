###############################################################
# Makefile for (Redhat Linux) ver 1.0
###############################################################

BINPATH = ../bin/
OBJPATH = ./
SRCPATH = ./

COMPILER= gcc -g 
LINKER  = gcc -g
DEBUG   = -Wall  -g
DEFINES = -D_DEBUG  
OSLIB   =  -lpthread -lssl -ldl -ljson-c  -L../lib/lib_X64

LIBS    = $(OSLIB) 
CFLAGS  =$(DEFINES) -Winline 
# -Wno-unused-parameter   

SOURCES = $(wildcard $(SRCPATH)*.c)

DEPS := 
ADEPS := 
DDEPS := \
		$(OBJPATH)common.o \
		$(OBJPATH)http.o  \
		$(OBJPATH)main.o  \
		$(OBJPATH)GetBlockByDateTest.o \
		$(OBJPATH)GetBlockDetailTest.o \
		$(OBJPATH)GetBlockHashByHeTest.o \
		$(OBJPATH)GetBlockHashByTxTest.o \
		$(OBJPATH)GetBlockHeightTest.o	 \
		$(OBJPATH)GetDataLastNewTest.o \
		$(OBJPATH)GetDataListTest.o	\
		$(OBJPATH)GetNewKeyTest.o \
		$(OBJPATH)GetDataFromSys.o \
		$(OBJPATH)GetNoteStatusTest.o	 \
		$(OBJPATH)SendDataForSignTest.o \
		$(OBJPATH)SendDataToSysTest.o 
	
	
OBJ1 := $(filter-out $(DEPS), $(addprefix $(OBJPATH),$(patsubst $(SRCPATH)%.c,%.o,$(SOURCES))))

OBJS := $(OBJ1)

AOBJS := $(OBJ1) $(filter-out $(ADEPS), $(addprefix $(OBJPATH),$(patsubst $(SRCPATH)%.c,%.o,$(SOURCES))))
DOBJS := $(filter-out $(DDEPS), $(addprefix $(OBJPATH),$(patsubst $(SRCPATH)%.c,%.o,$(SOURCES))))

.PHONY : all clean veryclean rebuild

EXES   = $(BINPATH)btcApiTest

all: $(EXES)
testsql: $(BINPATH)btcApiTest


$(BINPATH)btcApiTest: $(OBJS)
	$(LINKER) $(OBJS) \
	$(LIBS) -o $@

$(OBJPATH)%.o : $(SRCPATH)%.c $(SRCPATH)*.h 
	$(COMPILER) $(CFLAGS) $(DEBUG) \
	-c $< -o $@  -I./  -I ../include/json 

clean:
	rm -f $(OBJPATH)*.o $(BINPATH)btcApiTest
  
veryclean: clean 
	rm -f $(EXES)

rebuild: veryclean all


