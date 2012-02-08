ifeq ($(strip $(BOARD_USE_AML_STANDARD_RIL)),true)
	include $(call all-subdir-makefiles)
endif
