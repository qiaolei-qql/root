TIME:=$(shell date +'%Y%m%d')
OS:=$(shell uname -s)
ARCH:=$(shell uname -m)
BIT32:=i686
BIT64:=x86_64

# source directions
COMMON_DIR=common/
SRC_DIR=src/
TEST_DIR=test/

OBJ_DIR=./lib/

LIB_DIR = /usr/lib
LOCAL_LIB_DIR = /usr/local/lib

ifeq ($(ARCH),$(BIT64))
    LIB_DIR = /usr/lib64
    LOCAL_LIB_DIR = /usr/local/lib64
endif


CC=gcc
LOG=-DLOGGER -D_DEBUG

INCLUDE:=-I/usr/include -I/usr/local/include  -I. -I./include -I./libs/include -I./libs/ -I${BOOST_DIR} -I/usr/local/log4cplus/include -I/usr/local/mysql/include/mysql

# lib directions
BOOST_DIR = /usr/local/include/boost-1_51/
LIBS_PATH:=${PWD}/libs
LIB=-lstdc++ -L${LIB_DIR} -L${LOCAL_LIB_DIR} -L./lib/ -lrt -lcrypto -lpthread -lz -lbz2 -lm -lc -L${LIBS_PATH}  /usr/local/lib/liblog4cplus.a  ./libs/libhiredis.a /usr/local/mysql/lib/mysql/libmysqlclient_r.a   /usr/local/lib/libboost_system.a  /usr/local/lib/libboost_thread.a  /usr/local/lib/libboost_program_options.a /usr/local/lib/libboost_serialization.a


CFLAGS:=-Wall -g -O2  -DOS_LINUX  $(INCLUDE) $(LOG) #-DBOOST_DISABLE_ASSERTS
CPPFLAGS:=$(CFLAGS) 

COMMON_SRC:=$(wildcard $(COMMON_DIR)*.cpp)
SRC_SRC:=$(wildcard $(SRC_DIR)*.cpp)
TEST_SRC:=$(wildcard $(TEST_DIR)*.cpp)

COMMON_OBJ:=$(patsubst %.cpp,%.o,$(COMMON_SRC))
SRC_OBJ:=$(patsubst %.cpp,%.o,$(SRC_SRC))
TEST_OBJ:=$(patsubst %.cpp,%.o,$(TEST_SRC))

COMMON_DEP:=$(patsubst %.cpp,%.d,$(COMMON_SRC))
SRC_DEP:=$(patsubst %.cpp,%.d,$(SRC_SRC))
TEST_DEP:=$(patsubst %.cpp,%.d,$(TEST_SRC))

CPPFLAGS += -MD

#LIB_NAME=$(OBJ_DIR)libdeny_crawler_${ARCH}_${OS}.a
LIB_NAME=bin/deny_crawler_${ARCH}_${OS}
all: $(LIB_NAME)

$(LIB_NAME): $(COMMON_OBJ) $(SRC_OBJ)
	#ar -r $@ $^
	#ranlib $(LIB_NAME)
	$(CC) $^ $(CFLAGS) -o $@ $(LIB)
clean:
	rm $(COMMON_DIR)*.o $(SRC_DIR)*.o   $(DISCONF_DIR)*.o $(DISGROUP_DIR)*.o $(TEST_DIR)*.o -fv
	rm $(COMMON_DIR)*.d $(SRC_DIR)*.d   $(DISCONF_DIR)*.d $(DISGROUP_DIR)*.d  $(TEST_DIR)*.d -fv
	rm $(COMMON_DIR)*.gcov $(SRC_DIR)*.gcov   $(DISCONF_DIR)*.gcov  $(DISGROUP_DIR)*.gcov  $(TEST_DIR)*.gcov -fv
	rm $(COMMON_DIR)*.gcda $(SRC_DIR)*.gcda   $(DISCONF_DIR)*.gcda  $(DISGROUP_DIR)*.gcda  $(TEST_DIR)*.gcda -fv
	rm $(COMMON_DIR)*.gcno $(SRC_DIR)*.gcno   $(DISCONF_DIR)*.gcno  $(DISGROUP_DIR)*.gcno  $(TEST_DIR)*.gcno -fv

-include $(COMMON_DEP) $(SRC_DEP) $(TEST_DEP)


rebuild: clean all
love: clean all

