#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM)
endif
ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPro)
endif
include $(DEVKITARM)/gba_rules

BUILD		:=	build
SOURCES		:=	source source/gba
INCLUDES	:=	include build
DATA		:=	data

ARCH	:=	-mthumb -mthumb-interwork

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
CFLAGS	:=	-g -O3 -Wall -Wno-switch -Wno-multichar \
		-ffunction-sections -fdata-sections -Wl,--gc-sections \
		$(ARCH) $(INCLUDE) -D__GBA__
ASFLAGS	:=	-g -Wa,--warn $(ARCH) -D__GBA__

#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))

export TARGET	:=	$(CURDIR)/lib/libgbaline.a

export VPATH	:=	$(foreach dir,$(DATA),$(CURDIR)/$(dir)) $(foreach dir,$(SOURCES),$(CURDIR)/$(dir))

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES))

export OFILES_SRC	:=	$(CFILES:.c=.o) $(SFILES:.s=.o)

export OFILES	:=	$(OFILES_BIN) $(OFILES_SRC)

export HFILES	:=	$(addsuffix .h,$(subst .,_,$(BINFILES)))

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir))
export DEPSDIR	:=	$(CURDIR)/build

.PHONY: $(BUILD) clean

$(BUILD):
	@[ -d lib ] || mkdir -p lib
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

clean:
	@echo clean ...
	@rm -fr $(BUILD) lib

#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
$(TARGET): $(OFILES)

$(OFILES_SRC)	: $(HFILES)

#---------------------------------------------------------------------------------
%.a: $(OFILES)
	@echo $@
	@rm -f $@
	@$(AR) rcs $@ $(OFILES)

%_fnt.h %.fnt.o	:	%.fnt
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

endif
#---------------------------------------------------------------------------------
