###############################################################################
# SPDX-License-Identifier: GPL-3.0-or-later
###############################################################################

COMPONENT_DEPS = cjson \
                 freertos \
                 log \
                 rfm69 \
                 w25q

DEFS += -DDEBUG \
        -DFREERTOS \
        -DSTM32F103xB \
        -DUSE_HAL_DRIVER \
        -D__FILE_NAME__=\"$(notdir $<)\"

CFLAGS += -Wno-unused-parameter

###############################################################################