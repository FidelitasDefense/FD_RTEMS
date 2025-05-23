/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * 
 * @addtogroup RTEMSScoreCPUnios2
 *
 * @brief Altera Nios II CPU Department Source
 */

/*
 *  Copyright (c) 2011 embedded brains GmbH & Co. KG
 *
 *  Copyright (c) 2006 Kolja Waschk (rtemsdev/ixo.de)
 *
 *  COPYRIGHT (c) 1989-2004.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RTEMS_SCORE_CPU_H
#define _RTEMS_SCORE_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/basedefs.h>
#include <rtems/score/nios2.h>

#define CPU_SIMPLE_VECTORED_INTERRUPTS TRUE

#define CPU_INTERRUPT_NUMBER_OF_VECTORS 32

#define CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER (CPU_INTERRUPT_NUMBER_OF_VECTORS - 1)

#define CPU_PROVIDES_ISR_IS_IN_PROGRESS TRUE

#define CPU_ISR_PASSES_FRAME_POINTER FALSE

#define CPU_HARDWARE_FP FALSE

#define CPU_SOFTWARE_FP FALSE

#define CPU_ALL_TASKS_ARE_FP FALSE

#define CPU_IDLE_TASK_IS_FP FALSE

#define CPU_USE_DEFERRED_FP_SWITCH FALSE

#define CPU_ENABLE_ROBUST_THREAD_DISPATCH FALSE

#define CPU_STACK_GROWS_UP FALSE

/* FIXME: Is this the right value? */
#define CPU_CACHE_LINE_BYTES 32

#define CPU_STRUCTURE_ALIGNMENT \
  RTEMS_SECTION( ".sdata" ) RTEMS_ALIGNED( CPU_CACHE_LINE_BYTES )

#define CPU_STACK_MINIMUM_SIZE (4 * 1024)

#define CPU_SIZEOF_POINTER 4

/*
 * Alignment value according to "Nios II Processor Reference" chapter 7
 * "Application Binary Interface" section "Memory Alignment".
 */
#define CPU_ALIGNMENT 4

#define CPU_HEAP_ALIGNMENT CPU_ALIGNMENT

/*
 * Alignment value according to "Nios II Processor Reference" chapter 7
 * "Application Binary Interface" section "Stacks".
 */
#define CPU_STACK_ALIGNMENT 4

#define CPU_INTERRUPT_STACK_ALIGNMENT CPU_CACHE_LINE_BYTES

/*
 * A Nios II configuration with an external interrupt controller (EIC) supports
 * up to 64 interrupt levels.  A Nios II configuration with an internal
 * interrupt controller (IIC) has only two interrupt levels (enabled and
 * disabled).  The _CPU_ISR_Get_level() and _CPU_ISR_Set_level() functions will
 * take care about configuration specific mappings.
 */
#define CPU_MODES_INTERRUPT_MASK 0x3f

#define CPU_USE_GENERIC_BITFIELD_CODE TRUE

#define CPU_USE_LIBC_INIT_FINI_ARRAY FALSE

#define CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK 0

#define CPU_MAXIMUM_PROCESSORS 32

#ifndef ASM

/**
 * @brief Thread register context.
 *
 * The thread register context covers the non-volatile registers, the thread
 * stack pointer, the return address, and the processor status.
 *
 * There is no need to save the global pointer (gp) since it is a system wide
 * constant and set-up with the C runtime environment.
 */
typedef struct {
  uint32_t r16;
  uint32_t r17;
  uint32_t r18;
  uint32_t r19;
  uint32_t r20;
  uint32_t r21;
  uint32_t r22;
  uint32_t r23;
  uint32_t fp;
  uint32_t status;
  uint32_t sp;
  uint32_t ra;

  /**
   * @brief This member is used for the external interrupt controller (EIC) support.
   *
   * It corresponds to Per_CPU_Control::isr_dispatch_disable.
   */
  uint32_t isr_dispatch_disable;

  uint32_t stack_mpubase;
  uint32_t stack_mpuacc;
} Context_Control;

#define _CPU_Context_Get_SP( _context ) \
  (uintptr_t)(_context)->sp

typedef void CPU_Interrupt_frame;

typedef struct {
  uint32_t r1;
  uint32_t r2;
  uint32_t r3;
  uint32_t r4;
  uint32_t r5;
  uint32_t r6;
  uint32_t r7;
  uint32_t r8;
  uint32_t r9;
  uint32_t r10;
  uint32_t r11;
  uint32_t r12;
  uint32_t r13;
  uint32_t r14;
  uint32_t r15;
  uint32_t r16;
  uint32_t r17;
  uint32_t r18;
  uint32_t r19;
  uint32_t r20;
  uint32_t r21;
  uint32_t r22;
  uint32_t r23;
  uint32_t gp;
  uint32_t fp;
  uint32_t sp;
  uint32_t ra;
  uint32_t et;
  uint32_t ea;
  uint32_t status;
  uint32_t ienable;
  uint32_t ipending;
} CPU_Exception_frame;

/**
 * @brief Macro to disable interrupts.
 *
 * The processor status before disabling the interrupts will be stored in
 * @a _isr_cookie.  This value will be used in _CPU_ISR_Flash() and
 * _CPU_ISR_Enable().
 *
 * The global symbol _Nios2_ISR_Status_mask will be used to clear the bits in
 * the status register representing the interrupt level.  The global symbol
 * _Nios2_ISR_Status_bits will be used to set the bits representing an
 * interrupt level that disables interrupts.  Both global symbols must be
 * provided by the board support package.
 *
 * In case the Nios II uses the internal interrupt controller (IIC), then only
 * the PIE status bit is used.
 *
 * In case the Nios II uses the external interrupt controller (EIC), then the
 * RSIE status bit or the IL status field is used depending on the interrupt
 * handling variant and the shadow register usage.
 */
#define _CPU_ISR_Disable( _isr_cookie ) \
  do { \
    int _tmp; \
    __asm__ volatile ( \
      "rdctl %0, status\n" \
      "movhi %1, %%hiadj(_Nios2_ISR_Status_mask)\n" \
      "addi %1, %1, %%lo(_Nios2_ISR_Status_mask)\n" \
      "and %1, %0, %1\n" \
      "ori %1, %1, %%lo(_Nios2_ISR_Status_bits)\n" \
      "wrctl status, %1" \
      : "=&r" (_isr_cookie), "=&r" (_tmp) \
    ); \
  } while ( 0 )

/**
 * @brief Macro to restore the processor status.
 *
 * The @a _isr_cookie must contain the processor status returned by
 * _CPU_ISR_Disable().  The value is not modified.
 */
#define _CPU_ISR_Enable( _isr_cookie ) \
  __builtin_wrctl( 0, (int) _isr_cookie )

/**
 * @brief Macro to restore the processor status and disable the interrupts
 * again.
 *
 * The @a _isr_cookie must contain the processor status returned by
 * _CPU_ISR_Disable().  The value is not modified.
 *
 * This flash code is optimal for all Nios II configurations.  The rdctl does
 * not flush the pipeline and has only a late result penalty.  The wrctl on
 * the other hand leads to a pipeline flush.
 */
#define _CPU_ISR_Flash( _isr_cookie ) \
  do { \
    int _status = __builtin_rdctl( 0 ); \
    __builtin_wrctl( 0, (int) _isr_cookie ); \
    __builtin_wrctl( 0, _status ); \
  } while ( 0 )

bool _CPU_ISR_Is_enabled( uint32_t level );

/**
 * @brief Sets the interrupt level for the executing thread.
 *
 * The valid values of @a new_level depend on the Nios II configuration.  A
 * value of zero represents enabled interrupts in all configurations.
 *
 * @see _CPU_ISR_Get_level()
 */
void _CPU_ISR_Set_level( uint32_t new_level );

/**
 * @brief Returns the interrupt level of the executing thread.
 *
 * @retval 0 Interrupts are enabled.
 * @retval otherwise The value depends on the Nios II configuration.  In case
 * of an internal interrupt controller (IIC) the only valid value is one which
 * indicates disabled interrupts.  In case of an external interrupt controller
 * (EIC) there are two possibilities.  Firstly if the RSIE status bit is used
 * to disable interrupts, then one is the only valid value indicating disabled
 * interrupts.  Secondly if the IL status field is used to disable interrupts,
 * then this value will be returned.  Interrupts are disabled at the maximum
 * level specified by the _Nios2_ISR_Status_bits.
 */
uint32_t _CPU_ISR_Get_level( void );

/**
 * @brief Initializes the CPU context.
 *
 * The following steps are performed:
 *  - setting a starting address
 *  - preparing the stack
 *  - preparing the stack and frame pointers
 *  - setting the proper interrupt level in the context
 *
 * @param[in] context points to the context area
 * @param[in] stack_area_begin is the low address of the allocated stack area
 * @param[in] stack_area_size is the size of the stack area in bytes
 * @param[in] new_level is the interrupt level for the task
 * @param[in] entry_point is the task's entry point
 * @param[in] is_fp is set to @c true if the task is a floating point task
 * @param[in] tls_area is the thread-local storage (TLS) area
 */
void _CPU_Context_Initialize(
  Context_Control *context,
  void *stack_area_begin,
  size_t stack_area_size,
  uint32_t new_level,
  void (*entry_point)( void ),
  bool is_fp,
  void *tls_area
);

#define _CPU_Context_Restart_self( _the_context ) \
  _CPU_Context_restore( (_the_context) );

/**
 * @brief CPU initialization.
 */
void _CPU_Initialize( void );

typedef void ( *CPU_ISR_handler )( uint32_t );

void _CPU_ISR_install_vector(
  uint32_t         vector,
  CPU_ISR_handler  new_handler,
  CPU_ISR_handler *old_handler
);

RTEMS_NO_RETURN void *_CPU_Thread_Idle_body( uintptr_t ignored );

void _CPU_Context_switch( Context_Control *run, Context_Control *heir );

RTEMS_NO_RETURN void _CPU_Context_restore( Context_Control *new_context );

void _CPU_Exception_frame_print( const CPU_Exception_frame *frame );

static inline uint32_t CPU_swap_u32( uint32_t value )
{
  uint32_t byte1, byte2, byte3, byte4, swapped;

  byte4 = (value >> 24) & 0xff;
  byte3 = (value >> 16) & 0xff;
  byte2 = (value >> 8)  & 0xff;
  byte1 =  value        & 0xff;

  swapped = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;

  return swapped;
}

#define CPU_swap_u16( value ) \
  (((value&0xff) << 8) | ((value >> 8)&0xff))

typedef uint32_t CPU_Counter_ticks;

uint32_t _CPU_Counter_frequency( void );

CPU_Counter_ticks _CPU_Counter_read( void );

/** Type that can store a 32-bit integer or a pointer. */
typedef uintptr_t CPU_Uint32ptr;

#endif /* ASM */

#ifdef __cplusplus
}
#endif

#endif
