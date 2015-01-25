# Compiler
CXX =g++

# Compile flags
CFLAGS += -O3 -isystem extern/gtest-1.7.0/include

#Libraries
LIBS += -L/usr/local/lib/ -L/usr/lib -ljson_linux-gcc-4.6_libmt extern/gtest-1.7.0/make/gtest.a

#Includes
INCS += -Iinclude -Iextern/gtest-1.7.0 -Iextern/jsoncpp-src-0.5.0/include/

#Suffixes
.SUFFIXES: .o .h .c .hpp .cpp

# Directories
SRCDIR = src
BINDIR = bin
INCDIR = include

# Object files
OBJS = $(BINDIR)/optparser.o $(BINDIR)/effectsmodel.o $(BINDIR)/eventinfo.o $(BINDIR)/profile.o $(BINDIR)/hwstate.o $(BINDIR)/effect.o $(BINDIR)/effectupdatemessage.o

all: $(BINDIR)/effects_model_ui $(BINDIR)/unit_test

$(BINDIR)/effects_model_ui: $(BINDIR)/effects_model_ui.o $(OBJS)
	$(CXX) $(CFLAGS) -o $(BINDIR)/effects_model_ui $(BINDIR)/effects_model_ui.o $(OBJS) $(LIBS) ${INCS}

$(BINDIR)/unit_test: 	$(OBJS) $(BINDIR)/unittesting.o
	$(CXX) $(CFLAGS) -o $(BINDIR)/unit_test $(BINDIR)/unittesting.o $(OBJS) $(LIBS) ${INCS}
	./$(BINDIR)/unit_test 

$(BINDIR)/effects_model_ui.o: $(SRCDIR)/effects_model_ui.cpp 
	@echo $<
	$(CXX) $(CFLAGS) -c -o $(BINDIR)/effects_model_ui.o $(SRCDIR)/effects_model_ui.cpp ${INCS}

$(BINDIR)/optparser.o: $(SRCDIR)/optparser.cpp $(INCDIR)/optparser.h
	@echo $<
	$(CXX) $(CFLAGS) -c -o $(BINDIR)/optparser.o $(SRCDIR)/optparser.cpp ${INCS}

$(BINDIR)/effectsmodel.o: $(SRCDIR)/effectsmodel.cpp $(INCDIR)/effectsmodel.h
	@echo $<
	$(CXX) $(CFLAGS) -c -o $(BINDIR)/effectsmodel.o $(SRCDIR)/effectsmodel.cpp ${INCS}

$(BINDIR)/eventinfo.o: $(SRCDIR)/eventinfo.cpp $(INCDIR)/eventinfo.h
	@echo $<
	$(CXX) $(CFLAGS) -c -o $(BINDIR)/eventinfo.o $(SRCDIR)/eventinfo.cpp ${INCS}

$(BINDIR)/profile.o: $(SRCDIR)/profile.cpp $(INCDIR)/profile.h
	@echo $<
	$(CXX) $(CFLAGS) -c -o $(BINDIR)/profile.o $(SRCDIR)/profile.cpp ${INCS}

$(BINDIR)/hwstate.o: $(SRCDIR)/hwstate.cpp $(INCDIR)/hwstate.h
	@echo $<
	$(CXX) $(CFLAGS) -c -o $(BINDIR)/hwstate.o $(SRCDIR)/hwstate.cpp ${INCS}

$(BINDIR)/effect.o: $(SRCDIR)/effect.cpp $(INCDIR)/effect.h
	@echo $<
	$(CXX) $(CFLAGS) -c -o $(BINDIR)/effect.o $(SRCDIR)/effect.cpp ${INCS}

$(BINDIR)/effectupdatemessage.o: $(SRCDIR)/effectupdatemessage.cpp $(INCDIR)/effectupdatemessage.h
	@echo $<
	$(CXX) $(CFLAGS) -c -o $(BINDIR)/effectupdatemessage.o $(SRCDIR)/effectupdatemessage.cpp ${INCS}

$(BINDIR)/unittesting.o: $(SRCDIR)/unittesting.cpp $(INCDIR)/effect.h $(INCDIR)/effectupdatemessage.h
	@echo $<
	$(CXX) $(CFLAGS) -c -o $(BINDIR)/unittesting.o $(SRCDIR)/unittesting.cpp ${INCS}

clean:
	rm -rf ${BINDIR}/*
