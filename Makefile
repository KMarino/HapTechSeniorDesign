# Compiler
CXX =g++

PLATFORM =linux-gcc

# Directories
SRCDIR = src
BINDIR = bin
INCDIR = include
EXTDIR = extern

# Compile flags
CFLAGS += -O3 -isystem $(EXTDIR)/gtest-1.7.0/include -std=c++11

#Libraries
LIBS += -L/usr/local/lib/ -L/usr/lib -lglut -lGL -lGLU \
	$(EXTDIR)/jsoncpp-src-0.5.0/libs/linux-gcc-4.6/libjson_linux-gcc-4.6_libmt.so \
	$(EXTDIR)/gtest-1.7.0/make/gtest.a \
	$(EXTDIR)/aquila/libAquila.a \
	$(EXTDIR)/aquila/lib/libOoura_fft.a \
	-lsfml-audio \
	-lsfml-system \
	-lpthread

#Includes
INCS += -Iinclude -I$(EXTDIR)/gtest-1.7.0 -I$(EXTDIR)/jsoncpp-src-0.5.0/include/ -I$(EXTDIR)/aquila-src/aquila

#Suffixes
.SUFFIXES: .o .h .c .hpp .cpp

# Object files
OBJS = $(BINDIR)/optparser.o $(BINDIR)/effectsmodel.o $(BINDIR)/eventinfo.o $(BINDIR)/profile.o $(BINDIR)/hwstate.o $(BINDIR)/effect.o $(BINDIR)/effectupdatemessage.o

all:   test ui dsp
ui:    $(BINDIR)/effects_model_ui
dsp:   $(BINDIR)/dsp 
test:  $(BINDIR)/unit_test 

$(BINDIR)/effects_model_ui: $(BINDIR)/effects_model_ui.o $(OBJS)
	$(CXX) $(CFLAGS) -o $(BINDIR)/effects_model_ui $(BINDIR)/effects_model_ui.o $(OBJS) $(LIBS) ${INCS}

$(BINDIR)/dsp: $(BINDIR)/dsp.o $(OBJS)
	$(CXX) $(CFLAGS) -o $(BINDIR)/dsp $(BINDIR)/dsp.o $(OBJS) $(LIBS) ${INCS}

$(BINDIR)/unit_test: 	$(OBJS) $(BINDIR)/unittesting.o
	$(CXX) $(CFLAGS) -o $(BINDIR)/unit_test $(BINDIR)/unittesting.o $(OBJS) $(LIBS) ${INCS}
	./$(BINDIR)/unit_test $(SRCDIR)/../config/unitTest/

$(BINDIR)/effects_model_ui.o: $(SRCDIR)/effects_model_ui.cpp 
	@echo $<
	$(CXX) $(CFLAGS) -c -o $(BINDIR)/effects_model_ui.o $(SRCDIR)/effects_model_ui.cpp ${INCS}

$(BINDIR)/dsp.o: $(SRCDIR)/dsp.cpp 
	@echo $<
	$(CXX) $(CFLAGS) -c -o $(BINDIR)/dsp.o $(SRCDIR)/dsp.cpp ${INCS}

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


deps:
	# gtest
	$(MAKE) -C $(EXTDIR)/gtest-1.7.0/make gtest.a
	# jsoncpp
	scons -C $(EXTDIR)/jsoncpp-src-0.5.0 platform=$(PLATFORM) check
	#aquila
	$(MAKE) -C $(EXTDIR)/aquila-src all install
