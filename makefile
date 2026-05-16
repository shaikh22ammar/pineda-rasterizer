### Configuration

# Debug build
isDebug ?= true

# Packages paths
comma := ,
PKGS := SDL3
PKGS_INFO := $(shell pkg-config --cflags --libs $(PKGS))
PKGS_INC := $(filter -I%, $(PKGS_INFO))
PKGS_LIB := $(filter -L%, $(PKGS_INFO)) $(filter -l%, $(PKGS_INFO))
PKGS_RPATH := $(patsubst -L%, -Wl$(comma)-rpath$(comma)%, $(filter -L%, $(PKGS_LIB)))

### Compiler and linker commands

CC := cc

CFLAGS = 
CFLAGS += -Wall -Wextra -std=c23
CFLAGS += -Wdouble-promotion -Wfloat-conversion
CFLAGS += -MMD -MP
CFLAGS += -Isrc
CFLAGS += $(PKGS_INC)

ifeq ($(isDebug),true)
CFLAGS += -O0 -g
else
CFLAGS += -O3 -DNDEBUG
endif

LDFLAGS =
LDFLAGS += -lm
LDFLAGS += $(PKGS_LIB) $(PKGS_RPATH)


### Build directories and files

BUILD_DIR = build
TARGET = $(BUILD_DIR)/renderer

SRC_DIR = src
SRCS = $(wildcard $(SRC_DIR)/*.c)
$(info srcs detected are $(SRCS))

OBJS_DIR = $(BUILD_DIR)/objs
OBJS = $(patsubst $(SRC_DIR)/%, $(OBJS_DIR)/%, $(SRCS:.c=.o))
DEPS = $(OBJS:.o=.d)


### Targets

all: $(TARGET)

$(TARGET): $(OBJS) $(SPV_SHADERS) | $(BUILD_DIR)
	$(CC) -o $@ $(OBJS) $(LDFLAGS) 

$(OBJS_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJS_DIR)
	$(CC) $(CFLAGS) -o $@ -c $<

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(OBJS_DIR): | $(BUILD_DIR)
	mkdir -p $(OBJS_DIR)

-include $(DEPS)

clean:
	rm -v $(TARGET) $(OBJS) $(DEPS) $(SPV_SHADERS)
