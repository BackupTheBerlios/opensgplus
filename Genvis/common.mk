
SUB_DIR     := Genvis
PACKAGENAME := Genvis

INCL_DIR    := $($(PROJ)POOL)/$(SUB_DIR)

HAS_INCL    := 1
HAS_LIB     :=

REQUIRED_SYSTEM_PACKAGES :=

include $($(PROJ)POOL)/$($(PROJ)COMMON)/commonPackage.mk
