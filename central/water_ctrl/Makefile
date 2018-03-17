PROJECT_NAME     := valve_ctrl

SRC_FILES += \
  main.cpp \
  CValveCtrl.cpp \
  ../../central/components/dbhandler/CDBHandler.cpp \
  ../../central/components/safeout/safeout.cpp \

INC_FOLDERS += \
  ./ \
  ../../central/components/dbhandler/ \
  ../../central/components/safeout/ \


LIB_FILES += \
  mysqlclient \
  pthread \

ifeq ($(MACHIME),armv6l)
LIB_FILES += wiringPi
endif
ifeq ($(MACHIME),armv7l)
LIB_FILES += wiringPi
endif

CFLAGS += -O0
CFLAGS += -g3
CFLAGS += -Wall
CFLAGS += -c
CFLAGS += -fmessage-length=0

INST_PATH = /usr/local/bin/

OBJ_FILES = $(SRC_FILES:.cpp=.o)

default: all

clean:
	rm -f $(PROJECT_NAME)
	rm -f $(OBJ_FILES)

all: build

install: $(PROJECT_NAME)
	cp $(PROJECT_NAME) $(INST_PATH)

uninstall:
	rm -f $(INST_PATH)$(PROJECT_NAME)

build: $(PROJECT_NAME)
 
$(PROJECT_NAME): $(OBJ_FILES)
	g++ $(OBJ_FILES) -o $@ $(addprefix -l, $(LIB_FILES))


%.o:%.cpp
	g++ $(CFLAGS) $(addprefix -I, $(INC_FOLDERS)) $< -o $@

