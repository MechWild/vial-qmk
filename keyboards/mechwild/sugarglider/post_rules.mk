ifeq ($(strip $(BOOTLOADER)), tinyuf2)
    ifndef EEPROM_DRIVER
        MCU_LDSCRIPT = STM32F401xE
        EEPROM_DRIVER = vendor
        UF2_BUILD = yes
    endif
endif

ifeq ($(strip $(DYNAMIC_TOUCH_TAP_ENABLE)), yes)
    OPT_DEFS += -DDYNAMIC_TOUCH_TAP_ENABLE
endif

ifeq ($(strip $(OLED_DISPLAY_128X64)), yes)
    OPT_DEFS += -DOLED_DISPLAY_128X64
endif

ifeq ($(strip $(FAST_ENCODER_ENABLE)), yes)
    ifdef ENCODER_ENABLE
        OPT_DEFS += -DFAST_ENCODER_ENABLE
    endif
endif