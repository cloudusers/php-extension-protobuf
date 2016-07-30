PLATFORM := $(shell uname)

# set LIBDIR for Linux or FreeBSD
ARCH := $(shell uname -m)
LIBDIR := $(strip $(if $(shell echo $(ARCH)|grep 64), \
	$(if $(shell echo $(PLATFORM)|grep Linux),lib64,lib), \
	lib))

# set CPUS for Linux or FreeBSD
CPUS := $(strip $(if $(shell echo $(PLATFORM)|grep Linux),\
	$(shell cat /proc/cpuinfo|grep -c processor),\
	$(shell sysctl -a | egrep -i 'hw.ncpu' | cut -d: -f2)))

CXX := g++
CXXFLAGS := -g3 -O2 -fno-strict-aliasing -Wall -fPIC \
	-I/usr/local/include
LDFLAGS := -pthread

LIBS := -lprotobuf

DIR_SRC := src
DIR_LIB := lib
SRC := $(wildcard $(DIR_SRC)/*.cc)
HDR := $(wildcard $(DIR_SRC)/*.h)
OBJ := $(patsubst %.cc, %.o, $(SRC))
DEP := $(patsubst %.o, %.d, $(OBJ))

LIBNAME = liburl_attribute_table
LIBVER = 0.0.0

SHARED_EXT = so
STATIC_EXT = a
SHARED_LIB = $(LIBNAME).$(SHARED_EXT)
STATIC_LIB = $(LIBNAME).$(STATIC_EXT)

LIBVER_MAJOR = $(shell echo $(LIBVER) | cut -d. -f1)

all: 
	mkdir $(DIR_SRC)
	mkdir $(DIR_LIB)
	$(MAKE) pb
	$(MAKE) libs
	rm -rf $(DIR_SRC)/*.o
	rm -rf cloud_search

libs: shared static
	mv *.so* *.a ./$(DIR_LIB)

shared: $(SHARED_LIB).$(LIBVER) $(SHARED_LIB).$(LIBVER_MAJOR) $(SHARED_LIB)

static: $(STATIC_LIB)

pb:
	protoc --cpp_out=. -Iproto proto/*.proto
	protoc --java_out=. -Iproto proto/*.proto
	mv *.cc *.h ./$(DIR_SRC)

$(SHARED_LIB).$(LIBVER) : $(OBJ)
	$(CXX) $^ -o $@ $(LDFLAGS) $(LIBS) -shared -Wl,-soname,$(SHARED_LIB).$(LIBVER_MAJOR)

$(SHARED_LIB).$(LIBVER_MAJOR) : $(SHARED_LIB).$(LIBVER)
	ln -sf $^ $@

$(SHARED_LIB) : $(SHARED_LIB).$(LIBVER)
	ln -sf $^ $@

$(STATIC_LIB) : $(OBJ)
	ar -rs $@ $^

#-include $(DEP)

%.o : %.cc
	$(CXX) -c $(CXXFLAGS) $< -o $@
%.d : %.cc
	@$(CXX) -MM $< $(CXXFLAGS) | sed 's/$(notdir $*)\.o/$(subst /,\/,$*).o $(subst /,\/,$*).d/g' > $@

clean:
	-rm -rf $(HDR) $(SRC) $(OBJ) $(DEP) $(DIR_SRC) $(DIR_LIB)

.PHONY: all clean shared static pb libs
