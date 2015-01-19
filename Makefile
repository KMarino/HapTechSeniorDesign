# Compiler
CXX =g++

# Compile flags
CFLAGS += -O3 -isystem extern/gtest-1.7.0/include

#Libraries
LIBS += -L/usr/local/lib/ -L/usr/lib -ljson_linux-gcc-4.6_libmt extern/gtest-1.7.0/make/gtest.a

#Includes
INCS += -Iinclude -Iextern/gtest-1.7.0 -I/extern/jsoncpp-src-0.5.0/include/

#Suffixes
.SUFFIXES: .o .h .c .hpp .cpp

# Directories
SRCDIR = src
BINDIR = bin
INCDIR = include

# Object files
# PUT object files here. These are examples from another project
OBJS = $(BINDIR)/optparser.o $(BINDIR)/effectsmodel.o $(BINDIR)/eventinfo.o $(BINDIR)/profile.o $(BINDIR)/hwstate.o $(BINDIR)/effect.o $(BINDIR)/effectupdatemessage.o
# OBJS = $(BINDIR)/util.o $(BINDIR)/hdf_util.o $(BINDIR)/optparser.o $(BINDIR)/features.o $(BINDIR)/piotrfeatures.o  $(BINDIR)/random_forest.o $(BINDIR)/forest_helpers.o $(BINDIR)/boosted_svm.o $(BINDIR)/learning.o $(BINDIR)/inference.o $(BINDIR)/posemachine.o $(BINDIR)/classifier.o $(BINDIR)/logger.o $(BINDIR)/random_exemplar.o $(BINDIR)/feature_space_quantizer.o $(BINDIR)/kmeans.o

all: $(BINDIR)/effects_model_ui $(BINDIR)/serial_test

$(BINDIR)/effects_model_ui: $(BINDIR)/effects_model_ui.o $(OBJS)
	$(CXX) $(CFLAGS) -o $(BINDIR)/effects_model_ui $(BINDIR)/effects_model_ui.o $(OBJS) $(LIBS) ${INCS}

$(BINDIR)/serial_test: $(BINDIR)/serial_test.o $(OBJS)
	$(CXX) $(CFLAGS) -o $(BINDIR)/serial_test $(BINDIR)/serial_test.o $(OBJS) $(LIBS) ${INCS}

$(BINDIR)/effects_model_ui.o: $(SRCDIR)/effects_model_ui.cpp 
	@echo $<
	$(CXX) $(CFLAGS) -c -o $(BINDIR)/effects_model_ui.o $(SRCDIR)/effects_model_ui.cpp ${INCS}

$(BINDIR)/serial_test.o: $(SRCDIR)/serial_test.cpp
	@echo $<
	$(CXX) $(CFLAGS) -c -o $(BINDIR)/serial_test.o $(SRCDIR)/serial_test.cpp ${INCS}

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

#all: $(BINDIR)/train_pose_machine $(BINDIR)/run_pose_machine $(BINDIR)/run_pim_webcam $(BINDIR)/hdf_test $(BINDIR)/train_time_test $(BINDIR)/unit_test

#$(BINDIR)/unit_test: 	$(OBJS) $(BINDIR)/unittesting.o
#	$(CXX) $(CFLAGS) -o $(BINDIR)/unit_test $(BINDIR)/unittesting.o $(OBJS) $(LIBS) ${INCS}
#	./$(BINDIR)/unit_test $(SRCDIR)/../config/unitTest/

#$(BINDIR)/train_pose_machine: $(BINDIR)/train_pose_machine.o $(OBJS)
#	$(CXX) $(CFLAGS) -o $(BINDIR)/train_pose_machine $(BINDIR)/train_pose_machine.o $(OBJS)  $(LIBS) ${INCS}

#$(BINDIR)/fsq_test: $(BINDIR)/fsq_test.o $(OBJS)
#	$(CXX) $(CFLAGS) -o $(BINDIR)/fsq_test $(BINDIR)/fsq_test.o $(OBJS) $(LIBS) ${INCS}

#$(BINDIR)/hdf_test: $(BINDIR)/hdf_test.o $(OBJS)
#	$(CXX) $(CFLAGS) -o $(BINDIR)/hdf_test $(BINDIR)/hdf_test.o $(OBJS) $(LIBS) ${INCS}
	
#$(BINDIR)/train_time_test: $(BINDIR)/TrainTimeTest.o $(OBJS)
#	$(CXX) $(CFLAGS) -o $(BINDIR)/train_time_test $(BINDIR)/TrainTimeTest.o $(OBJS) $(LIBS) ${INCS}
	
#$(BINDIR)/TrainTimeTest.o: $(SRCDIR)/TrainTimeTest.cpp $(INCDIR)/ml/random_forest.h $(INCDIR)/ml/forest_helpers.h $(INCDIR)/globalDefs.h
#	@echo $<
#	$(CXX) $(CFLAGS) -c -o $(BINDIR)/TrainTimeTest.o $(SRCDIR)/TrainTimeTest.cpp ${INCS}

#$(BINDIR)/run_pose_machine: $(BINDIR)/run_pose_machine.o $(OBJS)
#	$(CXX) $(CFLAGS) -o $(BINDIR)/run_pose_machine $(BINDIR)/run_pose_machine.o $(OBJS) $(LIBS) ${INCS}

#$(BINDIR)/run_pim_webcam: $(BINDIR)/run_pim_webcam.o $(OBJS)
#	$(CXX) $(CFLAGS) -o $(BINDIR)/run_pim_webcam $(BINDIR)/run_pim_webcam.o $(OBJS) $(LIBS) ${INCS}

#$(BINDIR)/unittesting.o: $(SRCDIR)/unittesting.cpp $(INCDIR)/util.h $(INCDIR)/inference.h $(INCDIR)/features/features.h $(INCDIR)/optparser.h $(INCDIR)/globalDefs.h $(INCDIR)/HDFUtil.h $(INCDIR)/ml/random_forest.h $(INCDIR)/ml/boosted_svm.h $(INCDIR)/cuda_inference.h $(INCDIR)/posemachine.h $(INCDIR)/piotr/piotrfeatures.h
#	@echo $<
#	$(CXX) $(CFLAGS) -c -o $(BINDIR)/unittesting.o $(SRCDIR)/unittesting.cpp ${INCS}

#$(BINDIR)/train_pose_machine.o: $(SRCDIR)/train_pose_machine.cpp $(INCDIR)/optparser.h $(INCDIR)/ml/classifier.h $(INCDIR)/util.h $(INCDIR)/piotr/piotrfeatures.h $(INCDIR)/features/features.h $(INCDIR)/posemachine.h $(INCDIR)/learning.h $(INCDIR)/inference.h $(INCDIR)/globalDefs.h
#	@echo $<
#	$(CXX) $(CFLAGS) -c -o $(BINDIR)/train_pose_machine.o $(SRCDIR)/train_pose_machine.cpp ${INCS}

#$(BINDIR)/run_pose_machine.o: $(SRCDIR)/run_pose_machine.cpp $(INCDIR)/optparser.h $(INCDIR)/ml/classifier.h $(INCDIR)/util.h $(INCDIR)/features/features.h $(INCDIR)/piotr/piotrfeatures.h $(INCDIR)/posemachine.h $(INCDIR)/learning.h $(INCDIR)/inference.h $(INCDIR)/cuda_inference.h $(INCDIR)/globalDefs.h
#	@echo $<
#	$(CXX) $(CFLAGS) -c -o $(BINDIR)/run_pose_machine.o $(SRCDIR)/run_pose_machine.cpp ${INCS}

#$(BINDIR)/run_pim_webcam.o: $(SRCDIR)/run_pim_webcam.cpp $(INCDIR)/optparser.h $(INCDIR)/ml/classifier.h $(INCDIR)/util.h $(INCDIR)/features/features.h $(INCDIR)/piotr/piotrfeatures.h $(INCDIR)/posemachine.h $(INCDIR)/learning.h $(INCDIR)/inference.h $(INCDIR)/globalDefs.h
#	@echo $<
#	$(CXX) $(CFLAGS) -c -o $(BINDIR)/run_pim_webcam.o $(SRCDIR)/run_pim_webcam.cpp ${INCS}

#$(BINDIR)/hdf_test.o: $(SRCDIR)/testhdf5.cpp
#	@echo $<
#	$(CXX) $(CFLAGS) -c -o $(BINDIR)/hdf_test.o $(SRCDIR)/testhdf5.cpp ${INCS}
	
#$(BINDIR)/fsq_test.o: $(SRCDIR)/fsq_test.cpp $(INCDIR)/feature_quantizer/feature_space_quantizer.h $(INCDIR)/cluster/kmeans.h $(INCDIR)/ml/forest_helpers.h
#	@echo $<
#	$(CXX) $(CFLAGS) -c -o $(BINDIR)/fsq_test.o $(SRCDIR)/fsq_test.cpp ${INCS}

#$(BINDIR)/logger.o: $(SRCDIR)/logger.cpp $(INCDIR)/misc_util/logger.h
#	@echo $<
#	$(CXX) $(CFLAGS) -c -o $(BINDIR)/logger.o $(SRCDIR)/logger.cpp ${INCS}
	
#$(BINDIR)/random_exemplar.o: $(SRCDIR)/random_exemplar.cpp $(INCDIR)/cluster/cluster.h $(INCDIR)/misc_util/logger.h $(INCDIR)/misc_util/math_utils.h $(INCDIR)/misc_util/matrix.h $(INCDIR)/misc_util/stl_utils.h
#	@echo $<
#	$(CXX) $(CFLAGS) -c -o $(BINDIR)/random_exemplar.o $(SRCDIR)/random_exemplar.cpp ${INCS}
	
#$(BINDIR)/feature_space_quantizer.o: $(SRCDIR)/feature_space_quantizer.cpp $(INCDIR)/feature_quantizer/feature_space_quantizer.h $(INCDIR)/cluster/cluster.h $(INCDIR)/misc_util/logger.h $(INCDIR)/misc_util/matrix.h $(INCDIR)/misc_util/stl_utils.h
#	@echo $<
#	$(CXX) $(CFLAGS) -c -o $(BINDIR)/feature_space_quantizer.o $(SRCDIR)/feature_space_quantizer.cpp ${INCS}
	
#$(BINDIR)/kmeans.o: $(SRCDIR)/kmeans.cpp $(INCDIR)/cluster/kmeans.h $(INCDIR)/misc_util/logger.h $(INCDIR)/misc_util/matrix.h $(INCDIR)/misc_util/stl_utils.h $(INCDIR)/cluster/cluster.h $(INCDIR)/cluster/random_exemplar.h $(INCDIR)/misc_util/math_utils.h
#	@echo $<
#	$(CXX) $(CFLAGS) -c -o $(BINDIR)/kmeans.o $(SRCDIR)/kmeans.cpp ${INCS}

#$(BINDIR)/features.o: $(SRCDIR)/features.cpp $(INCDIR)/features/features.h $(INCDIR)/util.h $(INCDIR)/inference.h $(INCDIR)/posemachine.h $(INCDIR)/globalDefs.h $(INCDIR)/cuda_inference.h
#	@echo $<
#	$(CXX) $(CFLAGS) -c -o $(BINDIR)/features.o $(SRCDIR)/features.cpp ${INCS}

#$(BINDIR)/piotrfeatures.o: $(SRCDIR)/piotrfeatures.cpp $(INCDIR)/features/features.h $(INCDIR)/util.h $(INCDIR)/inference.h $(INCDIR)/posemachine.h $(INCDIR)/globalDefs.h $(INCDIR)/piotr/wrappers.hpp $(INCDIR)/piotr/sse.hpp $(INCDIR)/piotr/piotrfeatures.h
#	@echo $<
#	$(CXX) $(CFLAGS) -c -o $(BINDIR)/piotrfeatures.o $(SRCDIR)/piotrfeatures.cpp ${INCS}

#$(BINDIR)/util.o: $(SRCDIR)/util.cpp $(INCDIR)/util.h $(INCDIR)/ranker.h $(INCDIR)/globalDefs.h $(INCDIR)/ml/classifier.h
#	@echo $<
#	$(CXX) $(CFLAGS) -c -o $(BINDIR)/util.o $(SRCDIR)/util.cpp  ${INCS}
	
#$(BINDIR)/hdf_util.o: $(SRCDIR)/HDFUtil.cpp $(INCDIR)/HDFUtil.h
#	@echo $<
#	$(CXX) $(CFLAGS) -c -o $(BINDIR)/hdf_util.o $(SRCDIR)/HDFUtil.cpp ${INCS}

#$(BINDIR)/random_forest.o: $(SRCDIR)/random_forest.cpp $(INCDIR)/ml/random_forest.h $(INCDIR)/globalDefs.h $(INCDIR)/ml/forest_helpers.h $(INCDIR)/ml/classifier.h
#	@echo $<
#	$(CXX) $(CFLAGS) -c -o $(BINDIR)/random_forest.o $(SRCDIR)/random_forest.cpp ${INCS}

#$(BINDIR)/forest_helpers.o: $(SRCDIR)/forest_helpers.cpp $(INCDIR)/ml/forest_helpers.h $(INCDIR)/globalDefs.h
#	@echo $<
#	$(CXX) $(CFLAGS) -c -o $(BINDIR)/forest_helpers.o $(SRCDIR)/forest_helpers.cpp ${INCS}

#$(BINDIR)/boosted_svm.o: $(SRCDIR)/boosted_svm.cpp $(INCDIR)/ml/boosted_svm.h $(INCDIR)/globalDefs.h $(INCDIR)/ml/random_forest.h
#	@echo $<
#	$(CXX) $(CFLAGS) -c -o $(BINDIR)/boosted_svm.o $(SRCDIR)/boosted_svm.cpp ${INCS}

#$(BINDIR)/classifier.o: $(SRCDIR)/classifier.cpp $(INCDIR)/ml/classifier.h $(INCDIR)/globalDefs.h
#	@echo $<
#	$(CXX) $(CFLAGS) -c -o $(BINDIR)/classifier.o $(SRCDIR)/classifier.cpp ${INCS}

#$(BINDIR)/inference.o: $(SRCDIR)/inference.cpp $(INCDIR)/inference.h $(INCDIR)/ml/classifier.h $(INCDIR)/features/features.h $(INCDIR)/piotr/piotrfeatures.h $(INCDIR)/posemachine.h $(INCDIR)/learning.h $(INCDIR)/util.h $(INCDIR)/globalDefs.h $(INCDIR)/cuda_inference.h
#	@echo $<
#	$(CXX) $(CFLAGS) -c -o $(BINDIR)/inference.o $(SRCDIR)/inference.cpp ${INCS}

#$(BINDIR)/learning.o: $(SRCDIR)/learning.cpp $(INCDIR)/learning.h $(INCDIR)/inference.h $(INCDIR)/features/features.h $(INCDIR)/piotr/piotrfeatures.h $(INCDIR)/posemachine.h $(INCDIR)/globalDefs.h
#	@echo $<
#	$(CXX) $(CFLAGS) -c -o $(BINDIR)/learning.o $(SRCDIR)/learning.cpp ${INCS}

#$(BINDIR)/posemachine.o: $(SRCDIR)/posemachine.cpp $(INCDIR)/posemachine.h $(INCDIR)/ml/classifier.h $(INCDIR)/ml/random_forest.h $(INCDIR)/ml/boosted_svm.h $(INCDIR)/globalDefs.h $(INCDIR)/util.h
#	@echo $<
#	$(CXX) $(CFLAGS) -c -o $(BINDIR)/posemachine.o $(SRCDIR)/posemachine.cpp ${INCS}

clean:
	rm -rf ${BINDIR}/*
