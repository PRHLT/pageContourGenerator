
SOURCES_PAGE_CONTOUR = page_format_generate_contour.cc
OUTPUTFILE_PAGE_CONTOUR = page_format_generate_contour

#--------------------------------------------------------------------------------------
COMMONDIR = ./
INSTALLDIR= ./

AUTO_INCLUDE = `pkg-config --cflags-only-I opencv liblog4cxx eigen3`
MANUAL_INCLUDE = -I$(COMMONDIR) -I/opt/local/include/ 
INCLUDE = $(AUTO_INCLUDE) $(MANUAL_INCLUDE)

AUTO_LIB_INCLUDE = `pkg-config --cflags-only-I opencv liblog4cxx eigen3`
LIB_INCLUDE = $(AUTO_LIB_INCLUDE)


AUTO_LIBS = `pkg-config liblog4cxx opencv eigen3 --libs`
#MANUAL_LIBS = -lboost_system-mt -lboost_program_options-mt -lboost_filesystem-mt	-lboost_thread-mt
MANUAL_LIBS = -lboost_system -lboost_program_options -lboost_filesystem	-lboost_thread
EXTERNAL_LIBS = $(AUTO_LIBS) $(MANUAL_LIBS)

#--------------------------------------------------------------------------------------
INTERNAL_PAGE_CONTOURS_LIBS= $(COMMONDIR)/algorithm_sauvola.o $(COMMONDIR)/page_file.o $(COMMONDIR)/pugixml.o $(COMMONDIR)/line_region_list.o
#--------------------------------------------------------------------------------------
COPTIONS = -c -O3 -o
#COPTIONS = -c -O0 -g -o 
LOPTIONS = -O3 -o
#LOPTIONS = -O0 -g -o
LIB_OPTIONS = -c -O3 -pthread -o
#--------------------------------------------------------------------------------------
all: $(OUTPUTFILE_PAGE_CONTOUR)

page_format_generate_contour.o: $(SOURCES_PAGE_CONTOUR)
	$(CXX) $(COPTIONS)  $@ $< $(INCLUDE)

algorithm_sauvola.o: algorithm_sauvola.cc algorithm_sauvola.hpp
	$(CXX) $(LIB_OPTIONS) $@ $< $(LIB_INCLUDE)

line_region_list.o: line_region_list.cc line_region_list.hpp
	$(CXX) $(LIB_OPTIONS) $@ $< $(LIB_INCLUDE)

pugixml.o: pugixml.cpp pugixml.hpp pugiconfig.hpp
	$(CXX) $(LIB_OPTIONS) $@ $< $(LIB_INCLUDE)

page_file.o: page_file.cc page_file.hpp pugixml.hpp line_region_list.hpp
	$(CXX) $(LIB_OPTIONS) $@ $< $(LIB_INCLUDE)

$(OUTPUTFILE_PAGE_CONTOUR): page_format_generate_contour.o $(INTERNAL_PAGE_CONTOURS_LIBS)
	$(CXX) $(LDFLAGS) $(LOPTIONS) $@ $^  $(INCLUDE) $(EXTERNAL_LIBS)

clean:
	rm -rf *.o
	rm -f $(OUTPUTFILE_PAGE_CONTOUR)

