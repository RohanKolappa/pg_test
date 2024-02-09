# File        : options.mk
# Author      : Eldridge M. Mount IV (eldridge.mount@labxtechnologies.com)
# Description : Make options for the Biamp Labrinth SuperIO card applications

# Define and export toolchain environment settings
# CROSS  - The prefix for the cross-compiler, e.g. microblaze-uclinux
# ARCH   - The processor architecture, e.g. microblaze
# CFLAGS - Flags passed to the compiler
# LABXFC - Path the to the Lab X Foundation Classes library
#
# NOTE - The CFLAGS must be consistent with the hardware configuration of
#        the MicroBlaze processor!  (e.g. don't specify -mhard-float if you
#        disabled hardware floating point for the MicroBlaze in XPS...)
#
# NOTE - This project expects the Lab X Technologies MicroBlaze Linux git
#        repository to be checked out at the same level of directory
#        hierarchy as itself.
export ARCH       = arm
export CROSS      = arm-unknown-linux-uclibcgnueabi-
#export CPPFLAGS   = -mhard-float -mxl-barrel-shift -mxl-pattern-compare \
#                    -mno-xl-soft-div -mno-xl-soft-mul -ffunction-sections \
#                    -fdata-sections -O2 \
#                    -DHAVE_EXTENDED_ETHTOOL_TESTS \
#                    -DINCLUDE_DEBUG_INFO
export CPPFLAGS -O2

export LABXFC     = ../libLabXFc
export LABXAVB    = ../libLabXAvb
export LABXSRP    = ../libLabXSrp
export LINUX_PATH = $(PROJ_TOP)/../mb-linux-labx/uClinux-dist
export LINUX_INC  = $(LINUX_PATH)/linux-2.6.x/include \
                    $(LINUX_PATH)/linux-2.6.x/arch/microblaze/platform/Biamp/Labrinth-AVB-SuperIO \
                    $(LINUX_PATH)/uClibc/include \
                    $(LINUX_PATH)/uClibc/include/c++
#export CCFRONT    = $(LINUX_PATH)/tools/ucfront-gcc
#export CXXFRONT   = $(LINUX_PATH)/tools/ucfront-g++
export UBOOT_PATH = ../mb-u-boot-labx
export VENDOR     = biamp
export BOARD      = labrinth-avb
