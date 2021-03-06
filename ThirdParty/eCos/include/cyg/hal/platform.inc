#ifndef CYGONCE_HAL_PLATFORM_INC
#define CYGONCE_HAL_PLATFORM_INC
##=============================================================================
##
##  platform.inc
##
##  NT96650 DSC platform assembler header file
##
##=============================================================================
#####ECOSGPLCOPYRIGHTBEGIN####
## -------------------------------------------
## This file is part of eCos, the Embedded Configurable Operating System.
## Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
##
## eCos is free software; you can redistribute it and/or modify it under
## the terms of the GNU General Public License as published by the Free
## Software Foundation; either version 2 or (at your option) any later version.
##
## eCos is distributed in the hope that it will be useful, but WITHOUT ANY
## WARRANTY; without even the implied warranty of MERCHANTABILITY or
## FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
## for more details.
##
## You should have received a copy of the GNU General Public License along
## with eCos; if not, write to the Free Software Foundation, Inc.,
## 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
##
## As a special exception, if other files instantiate templates or use macros
## or inline functions from this file, or you compile this file and link it
## with other works to produce a work based on this file, this file does not
## by itself cause the resulting work to be covered by the GNU General Public
## License. However the source code for this file must still be made available
## in accordance with section (3) of the GNU General Public License.
##
## This exception does not invalidate any other reasons why a work based on
## this file might be covered by the GNU General Public License.
##
## Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
## at http://sources.redhat.com/ecos/ecos-license/
## -------------------------------------------
#####ECOSGPLCOPYRIGHTEND####
##=============================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s):
## Contributors:
## Date:
## Purpose:      NT96650 DSC platform definitions.
## Description:  This file contains various definitions and macros that are
##               useful for writing assembly code for the NT96650 DSC board.
## Usage:
##      #include <cyg/hal/platform.inc>
##      ...
##
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <cyg/hal/mips.inc>

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/plf_io.h>

#include <pkgconf/hal_mips.h>


#------------------------------------------------------------------------------
# Additional bits for status register.
# We set the IM[0] bit to accept all interrupts.

#define INITIAL_SR_PLF 0x00000400

#define EBase_CPUNUM_MASK       0x03FF      /* specifies the number of the CPU in a */
                                            /* multi-processor system               */
#define EBase_CPUNUM_CPU0       0x0f
#define EBase_CPUNUM_CPU1       0x78

#------------------------------------------------------------------------------

#define CYGPKG_HAL_RESET_VECTOR_FIRST_CODE
    .macro hal_reset_vector_first_code

    # Branch forward past the CPU number register.
    # Only one MIPS inside 96650 as main processor,
    # So, No need to detect CPU ID
#if 0
    mfc0 k0,$15,1
    andi k0,k0,EBase_CPUNUM_MASK
    li   k1,EBase_CPUNUM_CPU0
    beq  k0,k1,1f
    nop

    di
    wait    # halt the CPU1

    nop
    nop
    nop
    nop
    nop
1:
#endif
    .endm

#define CYGPKG_HAL_EARLY_INIT
    .macro  hal_early_init
    # Do these initializations early (rather than in hal_memc_init) so
    # we have access to the LEDs on the board for debugging purposes.

    .endm

#------------------------------------------------------------------------------
# Initialize CPU

#ifndef CYGPKG_HAL_MIPS_CPU_INIT_DEFINED
        .macro  hal_cpu_init

    # Read CCRes to check Counter divisor
    # li      $10, 0x00000008
    # mtc0    $10, $7, 0
    # rdhwr   a0, $3
    # nop

    # Clear Count register
    mtc0    zero, $9

    # Set compare to -1 to delay 1st count=compare
    # Also, clears timer interrupt
    li      $10, -1
    mtc0    $10, $11

    # Initialize/clear watchpoint registers
    mtc0    zero, $18               # C0_WATCHLO
    mtc0    zero, $19               # C0_WATCHHI

    la      t1, cyg_mem_size
    lw      a1, 0(t1)
    bnez    a1,1f
    nop
    li      t2, 0xb0000000          # DRAM controller configuration register
    lw      t3, 0(t2)
    andi    t2, t3, 7               # t2:0=128Mb,1=256Mb,2=512Mb,3=1Gb,4=2Gb
    li      t3, 128                 # 128Mb
    sll     a1, t3, t2              # Store memSize into cyg_mem_size
    sw      a1, 0(t1)
1:

        .endm
#define CYGPKG_HAL_MIPS_CPU_INIT_DEFINED
#endif // ifndef CYGPKG_HAL_MIPS_CPU_INIT_DEFINED


    # -------------------------------------------------------------------------#
    # Interrupt decode macros                                                  #
    # Only INTN[0] is connected on the NT96650 DSC board. We need to access the#
    # interrupt controller to get the actual vector number.                    #
    # -------------------------------------------------------------------------#
#ifndef CYGPKG_HAL_MIPS_INTC_INIT_DEFINED

    .macro  hal_intc_init

    # Reset interrupt controller, disable all interrupts
    li      t0, HAL_INTR_BASE
    sw      zero, 0(t0)

    # .extern __ebase

    # Check if vectored interrupts & external interrupt controller is implemented.
    # Config3(VInt), Config3(VEIC), r
    # mfc0  v0,$16,3
    # nop
    # andi    v0,v0,0x60
    # ori       v1,zero,0x60
    # bne       v0,v1,1f        # for MIPS old version, compatible mode only
    # nop

    # Specify the base address of the exception vectors
    # EBase(Exception Base)
    # mtc0  zero,$15,1
    # la        v0,__ebase      # 0x80000200
    # mtc0  v0,$15,1
    # where __ebase = 0x80000200, but bit[9..0] are read only. So, not necessary

/*
 ************************************************************************
 *          I N T C T L   R E G I S T E R   ( 1 2, SELECT 1 )           *
 ************************************************************************
 *
 *  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
 *  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |  I  |  I  |                               |         |         |
 * |  P  |  P  |         0                     |   VS    |   0     |  IntCtl
 * |  T  |  P  |                               |         |         |
 * |  I  |  C  |                               |         |         |
 * |     |  I  |                               |         |         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
    li      v0, 0x00000020          # Set VS as 32 bytes
    mtc0    v0,$12,1


    # -------------------------------------------------------------------------#
    # Set SSV2 as 1 ( vector 2 use shadow register1)                           #
    # -------------------------------------------------------------------------#
    # mfc0    t2, $12, 3
    # li      t3, 0x00000100    # Set SSV2 as 1 ( vector 2 use shadow register1)
    li      t3, 0x00000000    # Set SSV2 as 0 ( vector 2 use shadow register1)
    mtc0    t3, $12, 3
    nop
/*
 ************************************************************************
 *                C A U S E   R E G I S T E R   ( 1 3 )                 *
 ************************************************************************
 *
 *  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
 *  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | | |   | | |   |I|W| | | | | | |I|I|I|I|I|I|I|I| |         |   |
 * |B|0| C |0|0| 0 |V|P| | | | | | |P|P|P|P|P|P|P|P|0| ExcCode | 0 | Cause
 * |D| | E | | |   | | | | | | | | |7|6|5|4|3|2|1|0| |         |   |
 * | | |   | | |   | | | | | | | | | | | | | | | | | |         |   |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    T     D P                    \---RIPL----/
 *    I     C C                      Release 2
 *            I
 */
    # Clear WP bit to avoid watch exception upon user code entry
    # Clear IV bit - Interrupts go to general exception vector
    # Clear software interrupts
    # mtc0    $0, $13
    # Indicates an interrupt exception uses the general exception vector.
    # Cause(IV) = 0, r/w
    //#NT#2013/07/26#Steven Wang -begin
    //#NT#Change to CAUSEiv = 1(bit23), and enable counter
#if 0
    mfc0    v0,cause
    lui     v1,0x0070           # Use the general interrupt vector(180h/380h)
                                # bit[31..28] are read only register, not necessary configured
    and     v0,v0,v1
    mtc0    v0,cause
#else
#   lui     v1,0x0880           # Use special interrupt vector (200h where BEV=0) and Disable Count register as well
    lui     v1,0x0080           # Use special interrupt vector (200h where BEV=0) and Enable Count register as well
    mtc0    v1,cause
#endif
    //#NT#2013/07/26#Steven Wang -end

1:

/*
 ************************************************************************
 *               S T A T U S   R E G I S T E R   ( 1 2 )                *
 ************************************************************************
 *
 *  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
 *  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |C|C|C|C|R|F|R|M|P|B|T|S|M| | R |I|I|I|I|I|I|I|I|K|S|U|U|R|E|E|I|
 * |U|U|U|U|P|R|E|X|X|E|S|R|M| | s |M|M|M|M|M|M|M|M|X|X|X|M|s|R|X|E| Status
 * |3|2|1|0| | | | | |V| | |I| | v |7|6|5|4|3|2|1|0| | | | |v|L|L| |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
    # -------------------------------------------------------------------------#
    # Config status register                                                   #
    # -------------------------------------------------------------------------#
    # Controls the location of exception vectors normal.
    # Clear Interrupt Priority Level.
    # Status(BEV), Status(IPL), r/w
    # mfc0    v0,status
#if defined(CYG_HAL_STARTUP_RAM)
    # li      v1,0x1000FC00         # BEV = 0 (old value :0x1000FC00)
    //#NT#2013/07/26#Steven Wang -begin
    //#NT#unmask interrupt 2(650 INT) & 6(perf) &7(counter)
    # li      v1,0x10000000
    li      v1, 0x0100C400
#elif defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_ROMRAM)
    # li      v1,0x1040FC00        # BEV = 1
    li      v1,0x10400000
#endif
    //#NT#2013/07/26#Steven Wang -end
    # move    v0,v1
    # or     v0,v1

# #if 0
#   ori     v0,v0,0x0001         # Controls the enabling of the interrupt enable(IE)
# #endif
    mtc0    v1,status

    # Clear Count register
    mtc0    $0, $9

    # Set compare to -1 to delay 1st count=compare
    # Also, clears timer interrupt
    li      t2, -1
    mtc0    t2, $11

    .endm

#define CYGPKG_HAL_MIPS_INTC_INIT_DEFINED
#endif

#ifndef CYGPKG_HAL_MIPS_INTC_DECODE_DEFINED
    .macro  hal_intc_decode vnum
        mfc0    v1,status               # get status register (interrupt mask)
#       nop                             # delay slot
        mfc0    v0,cause                # get cause register
        and     v0,v0,v1                # apply interrupt mask
        srl     v0,v0,8                 # shift interrupt bits down
        andi    v1,v0,0x80              # test HW5
        beqz    v1,1f
        li      \vnum,39                # check if match with CYGNUM_HAL_INTR_CPUCNT value in plf_intr.h
        j       2f
        nop
1:
        li      v1,HAL_INTR_ID          # HAL_INTR_ID = 0xB0080024 interrupt ID
        lw      v0,0(v1)
        srl     \vnum,v0,2              # TODO ID is already in 4 bytes unit
2:
    .endm

#define CYGPKG_HAL_MIPS_INTC_DECODE_DEFINED
#endif

#ifndef CYGPKG_HAL_MIPS_INTC_TRANSLATE_DEFINED
    .macro  hal_intc_translate inum,vnum
    move    \vnum,\inum         # Vector == interrupt number
    .endm
#define CYGPKG_HAL_MIPS_INTC_TRANSLATE_DEFINED
#endif

// We also define our own interrupt tables in platform.S...
#define CYG_HAL_MIPS_ISR_TABLES_DEFINED

#------------------------------------------------------------------------------
# Diagnostic macros

#ifndef CYGPKG_HAL_MIPS_DIAG_DEFINED

    .macro  hal_diag_init
    .endm

    .macro  hal_diag_excpt_start
    .endm

    .macro  hal_diag_intr_start
    .endm

    .macro  hal_diag_restore
    .endm

    .macro  hal_diag_writec char
    .endm

#define CYGPKG_HAL_MIPS_DIAG_DEFINED
#endif // ifndef CYGPKG_HAL_MIPS_DIAG_DEFINED

#------------------------------------------------------------------------------
# MEMC macros.
#

#define CYGARC_START_FUNC_UNCACHED

#------------------------------------------------------------------------------
# Make sure the jump to _start in vectors.S is done uncached
#

    .macro  hal_memc_sdr_init
    .endm

    .macro  hal_memc_ddr_init
    .endm

    .macro  hal_memc_ddr2_init
    .endm

#ifndef CYGPKG_HAL_MIPS_MEMC_DEFINED

    .macro  hal_memc_init
    .endm

#define CYGPKG_HAL_MIPS_MEMC_DEFINED
#endif // ifndef CYGPKG_HAL_MIPS_MEMC_DEFINED

#------------------------------------------------------------------------------
# Timer initialization.

#ifndef CYGPKG_HAL_MIPS_TIMER_DEFINED

    .macro  hal_timer_init
    .endm

#define CYGPKG_HAL_MIPS_TIMER_DEFINED
#endif // ifndef CYGPKG_HAL_MIPS_TIMER_DEFINED


#------------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_PLATFORM_INC
# end of platform.inc
