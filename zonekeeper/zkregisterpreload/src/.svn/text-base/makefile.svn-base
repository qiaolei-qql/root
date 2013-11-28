INCLUDES=-I/home/qiaolei/zonekeeper/client/include/ -I/usr/include/ -I/usr/local/include/ -I/usr/include/boost

#lib define
LIB_DIR=-L/usr/local/lib    #-L/usr/local/lib64 -L/usr/lib64
#LIB_SSL=-lssl

ALL_LIBS=-llog4cplus -lzookeeper_mt -lz  -lboost_program_options -lboost_date_time -lboost_thread
#ALL_LIBS=/usr/local/lib/liblog4cplus.a /home/qiaolei/zonekeeper/client/lib/libzookeeper_mt.a  /usr/local/lib/libboost_program_options.a  -lpthread

#common define
MAKE=make
CXX:=g++ -Wall
MV=/bin/mv
EXTRAFLAGS=-g -fPIC -DRECORD_LOG -D__LINUX__ -DTHREADED 
#EXTRAFLAGS=-g -D__LINUX__
CXXFLAGS=$(EXTRAFLAGS) $(INCLUDES) 

RM=rm -f
AR=ar rc
OBJ_PATH:=objs/
SRC_PATH:=./

SRC:=$(wildcard *.cpp)
OBJS:=$(patsubst %.cpp, %.o, $(SRC))
OBJS:=$(addprefix $(OBJ_PATH), $(OBJS))
DEPENDS:=$(patsubst %.cpp, %.d, $(SRC))
DEPENDS:=$(addprefix $(OBJ_PATH), $(DEPENDS))

include $(DEPENDS)

# High Performance Framework
#BIN=libs/libhpf.a
BIN=../libs/libzk_register_preload.so
#BIN=main

all: $(BIN) 

$(BIN):$(OBJS) 
	$(CXX) $(CXXFLAGS) -shared -fPIC -o $@ $^ $(LIB_DIR) $(ALL_LIBS) 
	#ar cr $@ $^
	#$(CXX) $(CXXFLAGS) -o $@ $^  $(LIB_DIR) $(ALL_LIBS)
clean:
	$(RM)  $(OBJS) $(BIN) $(BIN).tar.bz2 $(BIN)_bin $(DEPENDS)


$(OBJ_PATH)%.o : $(SRC_PATH)%.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(OBJ_PATH)%.d : $(SRC_PATH)%.cpp
	set -e;\
		$(RM) $@;\
		gcc $(INCLUDES) -MM $< > $@.$$$$;\
		sed 's,\($*\)\.o[: ],$(OBJ_PATH)\1\.o $@: ,g' < $@.$$$$ > $@;\
		$(RM) $@.$$$$;

