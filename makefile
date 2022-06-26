###############################################################################
# SPDX-License-Identifier: GPL-3.0-or-later
###############################################################################

-include .config

OUTDIR ?= build
CPU ?= cortex-m3
OPTIMIZATION ?= -O0 -g0
LIBDIR ?= libs
PROJNAME ?= $(notdir $(shell pwd))
MAKEFLAGS += --no-print-directory

AR = arm-none-eabi-ar
CC = arm-none-eabi-gcc
CXX = arm-none-eabi-g++
OBJCOPY = arm-none-eabi-objcopy

ifeq ($(MAKECMDGOALS),)
  DIRS := $(shell find components -type d)
  COMPONENTS := $(wildcard $(addsuffix /component.mk, $(DIRS)))
  COMPONENTS += $(wildcard $(addsuffix /component.mk, application))
endif

###############################################################################

CORE = -mcpu=$(CPU) -mfloat-abi=soft -mthumb

ifeq ($(MAKECMDGOALS),component)

  DEFS =
  DEPS = -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"
  DIRS := components $(shell find $(COMPONENT_DIR) -type d)

  LIBNAME := $(addprefix lib., $(subst components/,, $(COMPONENT_DIR)))

  ASSOURCES := $(wildcard $(addsuffix /*.s, $(DIRS)))
  CSOURCES  := $(wildcard $(addsuffix /*.c, $(DIRS)))
  CPPSOURCES := $(wildcard $(addsuffix /*.cpp, $(DIRS)))

  HEADERS := $(addprefix -I", $(addsuffix ", $(DIRS)))
  HEADERS += $(addprefix -I", $(addsuffix ", scripts/include))

  OBJECTS := $(addprefix $(OUTDIR)/, $(ASSOURCES:.s=.o))
  OBJECTS += $(addprefix $(OUTDIR)/, $(CSOURCES:.c=.o))
  OBJECTS += $(addprefix $(OUTDIR)/, $(CPPSOURCES:.cpp=.o))

  FLAGS = $(subst ",,$(CONFIG_COMPILER_OPTIMIZATION_LEVEL)) \
          $(subst ",,$(CONFIG_COMPILER_DEBUG_LEVEL)) \
          $(subst ",,$(CONFIG_COMPILER_FUNCTION_SECTIONS)) \
          $(subst ",,$(CONFIG_COMPILER_DATA_SECTIONS)) \
          $(subst ",,$(CONFIG_COMPILER_STACK_USAGE)) \
          $(subst ",,$(CONFIG_COMPILER_WARNING_ALL)) \
          $(subst ",,$(CONFIG_COMPILER_WARNING_ERROR)) \
          $(subst ",,$(CONFIG_COMPILER_WARNING_EXTRA)) \
          $(subst ",,$(CONFIG_COMPILER_WARNING_NO_UNUSED_PARAMETER)) \
          $(subst ",,$(CONFIG_COMPILER_WARNING_SWITCH_DEFAULT)) \
          $(subst ",,$(CONFIG_COMPILER_WARNING_SWITCH_ENUM))

  ASFLAGS = $(CORE) -x assembler-with-cpp

  CFLAGS = -std=$(subst ",,$(CONFIG_COMPILER_C_LANGUAGE_STANDARD)) \
           $(CORE) $(FLAGS) $(DEFS) $(HEADERS) \
           -specs=nano.specs \
           -Wall -Wextra \
           $(DEPS)

  CXXFLAGS = -std=$(subst ",,$(CONFIG_COMPILER_CPP_LANGUAGE_STANDARD)) \
             $(CORE) $(FLAGS) $(DEFS) $(HEADERS) \
             --specs=nano.specs \
             -fno-exceptions \
             -fno-use-cxa-atexit \
             -fno-threadsafe-statics \
             -fno-rtti \
             -Wall -Wextra \
             $(DEPS)

  #Include header folders of dependency components
  include $(COMPONENT_DIR)/$(COMPONENT_MK)
  ifneq ($(COMPONENT_DEPS),)
    COMDEPS := $(addprefix components/, $(COMPONENT_DEPS))
    DEPDIRS := $(shell find $(COMDEPS) -type d)
    HEADERS += $(addprefix -I", $(addsuffix ", $(DEPDIRS)))
  endif
endif

LINKERS = $(addprefix -T", $(addsuffix ", $(wildcard $(addsuffix /*.ld, application/cubemx))))
LDFLAGS := $(CORE) \
           $(LINKERS) \
           $(subst ",,$(CONFIG_COMPILER_USE_PRINTF_FLOAT)) \
           $(subst ",,$(CONFIG_COMPILER_USE_SCANF_FLOAT)) \
           -Wl,-Map=$(OUTDIR)/$(PROJNAME).map \
           -specs=nano.specs \
           -specs=nosys.specs \
           -Wl,--print-memory-usage \
           -Wl,--gc-sections -static \
           -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group

MAKEFILE = $(firstword $(MAKEFILE_LIST))

###############################################################################

all: menuconfig | build

build: $(OUTDIR)/$(PROJNAME).bin

$(OUTDIR)/$(PROJNAME).bin: $(OUTDIR)/$(PROJNAME).elf
	@$(OBJCOPY) -O binary $^ $@

$(OUTDIR)/$(PROJNAME).elf: $(COMPONENTS)
	$(eval LIBDIRS = $(addprefix -L", $(addsuffix ", $(OUTDIR)/libs)))
	$(eval LIBLIST = $(notdir $(wildcard $(addsuffix /*.a, $(OUTDIR)/libs))))
	$(eval LIBS = $(addprefix -l", $(addsuffix ", $(LIBLIST:lib%.a=%))))
	@echo linker: $(OUTDIR)/$(PROJNAME).elf
	@echo binary: $(OUTDIR)/$(PROJNAME).bin
	@$(CXX) $(LDFLAGS) -o $@ $(LIBDIRS) $(LIBS)

$(COMPONENTS):
	@$(MAKE) component COMPONENT_DIR=$(@D) COMPONENT_MK=$(@F)

component: $(OUTDIR)/$(LIBDIR)/$(LIBNAME).a
	@: #prevent warning "make[2]: Nothing to be done for 'component'."

$(OUTDIR)/$(LIBDIR)/$(LIBNAME).a: $(OBJECTS)
	@echo arc: $(@F)
	@mkdir -p $(@D)
	@$(AR) rc $@ $^

$(OBJECTS): $(MAKEFILE) #Rebuild project if Makefile changed

$(OUTDIR)/%.o: %.s
	@mkdir -p $(@D)
	@$(CC) -c $(ASFLAGS) '$<' -o '$@'

$(OUTDIR)/%.o: %.c
	@mkdir -p $(@D)
	@$(CC) -c $(CFLAGS) '$<' -o '$@'

$(OUTDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	@$(CXX) -c $(CXXFLAGS) '$<' -o '$@'

menuconfig:
	@$(MAKE) -f scripts/Makefile $@

cppcheck:
	@cppcheck --force --quiet -icubemx application

cloc:
	@cloc --quiet --exclude-dir=cubemx application

clean:
	rm -rf $(OUTDIR)

cleancfg:
	@$(MAKE) -f scripts/Makefile clean

-include $(OBJECTS:.o=.d) #Dependencies

.PHONY: all test build $(COMPONENTS) component menuconfig cppcheck cloc clean \
        cleancfg

###############################################################################
