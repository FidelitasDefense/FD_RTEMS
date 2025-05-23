/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUARM
 *
 * @brief This header file defines implementation interfaces pertaining to the
 *   port of the executive to the ARM architecture.
 */

/*
 *  This include file contains information pertaining to the ARM
 *  processor.
 *
 *  Copyright (C) 2009, 2017 embedded brains GmbH & Co. KG
 *
 *  Copyright (c) 2007 Ray Xu <Rayx.cn@gmail.com>
 *
 *  Copyright (c) 2006 On-Line Applications Research Corporation (OAR)
 *
 *  Copyright (c) 2002 Advent Networks, Inc.
 *        Jay Monkman <jmonkman@adventnetworks.com>
 *
 *  COPYRIGHT (c) 2000 Canon Research Centre France SA.
 *  Emmanuel Raguet, mailto:raguet@crf.canon.fr
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
 *
 */

#ifndef _RTEMS_SCORE_CPU_H
#define _RTEMS_SCORE_CPU_H

#include <rtems/score/basedefs.h>
#if defined(RTEMS_PARAVIRT)
#include <rtems/score/paravirt.h>
#endif
#include <rtems/score/arm.h>

/**
 * @addtogroup RTEMSScoreCPUARM
 *
 * @{
 */

#if defined(ARM_MULTILIB_ARCH_V4)

#if defined(__thumb__) && !defined(__thumb2__)
  #define ARM_SWITCH_REGISTERS uint32_t arm_switch_reg
  #define ARM_SWITCH_TO_ARM ".align 2\nbx pc\n.arm\n"
  #define ARM_SWITCH_BACK "add %[arm_switch_reg], pc, #1\nbx %[arm_switch_reg]\n.thumb\n"
  #define ARM_SWITCH_OUTPUT [arm_switch_reg] "=&r" (arm_switch_reg)
  #define ARM_SWITCH_ADDITIONAL_OUTPUT , ARM_SWITCH_OUTPUT
#else
  #define ARM_SWITCH_REGISTERS
  #define ARM_SWITCH_TO_ARM
  #define ARM_SWITCH_BACK
  #define ARM_SWITCH_OUTPUT
  #define ARM_SWITCH_ADDITIONAL_OUTPUT
#endif

/**
 * @name Program Status Register
 */
/**@{**/

#define ARM_PSR_N (1 << 31)
#define ARM_PSR_Z (1 << 30)
#define ARM_PSR_C (1 << 29)
#define ARM_PSR_V (1 << 28)
#define ARM_PSR_Q (1 << 27)
#define ARM_PSR_J (1 << 24)
#define ARM_PSR_GE_SHIFT 16
#define ARM_PSR_GE_MASK (0xf << ARM_PSR_GE_SHIFT)
#define ARM_PSR_E (1 << 9)
#define ARM_PSR_A (1 << 8)
#define ARM_PSR_I (1 << 7)
#define ARM_PSR_F (1 << 6)
#define ARM_PSR_T (1 << 5)
#define ARM_PSR_M_SHIFT 0
#define ARM_PSR_M_MASK (0x1f << ARM_PSR_M_SHIFT)
#define ARM_PSR_M_USR 0x10
#define ARM_PSR_M_FIQ 0x11
#define ARM_PSR_M_IRQ 0x12
#define ARM_PSR_M_SVC 0x13
#define ARM_PSR_M_ABT 0x17
#define ARM_PSR_M_HYP 0x1a
#define ARM_PSR_M_UND 0x1b
#define ARM_PSR_M_SYS 0x1f

/** @} */

#endif /* defined(ARM_MULTILIB_ARCH_V4) */

/*
 *  The ARM uses the PIC interrupt model.
 */
#define CPU_SIMPLE_VECTORED_INTERRUPTS FALSE

#define CPU_ISR_PASSES_FRAME_POINTER FALSE

#define CPU_HARDWARE_FP FALSE

#define CPU_SOFTWARE_FP FALSE

#define CPU_ALL_TASKS_ARE_FP FALSE

#define CPU_IDLE_TASK_IS_FP FALSE

#define CPU_USE_DEFERRED_FP_SWITCH FALSE

#define CPU_ENABLE_ROBUST_THREAD_DISPATCH TRUE

#define CPU_STACK_GROWS_UP FALSE

#if defined(ARM_MULTILIB_CACHE_LINE_MAX_64)
  #define CPU_CACHE_LINE_BYTES 64
#else
  #define CPU_CACHE_LINE_BYTES 32
#endif

#define CPU_STRUCTURE_ALIGNMENT RTEMS_ALIGNED( CPU_CACHE_LINE_BYTES )

#define CPU_MODES_INTERRUPT_MASK 0x1

#define CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK 0

#define CPU_PROVIDES_ISR_IS_IN_PROGRESS FALSE

#define CPU_STACK_MINIMUM_SIZE (1024 * 4)

/* AAPCS, section 4.1, Fundamental Data Types */
#define CPU_SIZEOF_POINTER 4

/* AAPCS, section 4.1, Fundamental Data Types */
#define CPU_ALIGNMENT 8

#define CPU_HEAP_ALIGNMENT CPU_ALIGNMENT

/* AAPCS, section 5.2.1.2, Stack constraints at a public interface */
#define CPU_STACK_ALIGNMENT 8

#define CPU_INTERRUPT_STACK_ALIGNMENT CPU_CACHE_LINE_BYTES

/*
 * Bitfield handler macros.
 *
 * If we had a particularly fast function for finding the first
 * bit set in a word, it would go here. Since we don't (*), we'll
 * just use the universal macros.
 *
 * (*) On ARM V5 and later, there's a CLZ function which could be
 *     used to implement much quicker than the default macro.
 */

#define CPU_USE_GENERIC_BITFIELD_CODE TRUE

#define CPU_USE_LIBC_INIT_FINI_ARRAY TRUE

#define CPU_MAXIMUM_PROCESSORS 32

#define ARM_CONTEXT_CONTROL_THREAD_ID_OFFSET 44

#ifdef ARM_MULTILIB_VFP
  #define ARM_CONTEXT_CONTROL_D8_OFFSET 48
#endif

#ifdef ARM_MULTILIB_ARCH_V4
  #define ARM_CONTEXT_CONTROL_ISR_DISPATCH_DISABLE 40
#endif

#ifdef RTEMS_SMP
  #if defined(ARM_MULTILIB_VFP)
    #define ARM_CONTEXT_CONTROL_IS_EXECUTING_OFFSET 112
  #else
    #define ARM_CONTEXT_CONTROL_IS_EXECUTING_OFFSET 48
  #endif
#endif

#define ARM_EXCEPTION_FRAME_SIZE 80

#define ARM_EXCEPTION_FRAME_REGISTER_R8_OFFSET 32

#define ARM_EXCEPTION_FRAME_REGISTER_SP_OFFSET 52

#define ARM_EXCEPTION_FRAME_REGISTER_PC_OFFSET 60

#if defined(ARM_MULTILIB_ARCH_V4)
  #define ARM_EXCEPTION_FRAME_REGISTER_CPSR_OFFSET 64
#elif defined(ARM_MULTILIB_ARCH_V6M) || defined(ARM_MULTILIB_ARCH_V7M)
  #define ARM_EXCEPTION_FRAME_REGISTER_XPSR_OFFSET 64
#endif

#define ARM_EXCEPTION_FRAME_VFP_CONTEXT_OFFSET 72

#define ARM_VFP_CONTEXT_SIZE 264

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
#if defined(ARM_MULTILIB_ARCH_V4)
  uint32_t register_r4;
  uint32_t register_r5;
  uint32_t register_r6;
  uint32_t register_r7;
  uint32_t register_r8;
  uint32_t register_r9;
  uint32_t register_r10;
  uint32_t register_fp;
  uint32_t register_sp;
  uint32_t register_lr;
  uint32_t isr_dispatch_disable;
#elif defined(ARM_MULTILIB_ARCH_V6M) || defined(ARM_MULTILIB_ARCH_V7M)
  uint32_t register_r4;
  uint32_t register_r5;
  uint32_t register_r6;
  uint32_t register_r7;
  uint32_t register_r8;
  uint32_t register_r9;
  uint32_t register_r10;
  uint32_t register_r11;
  void *register_lr;
  void *register_sp;
  uint32_t isr_nest_level;
#else
  void *register_sp;
#endif
  uint32_t thread_id;
#ifdef ARM_MULTILIB_VFP
  uint64_t register_d8;
  uint64_t register_d9;
  uint64_t register_d10;
  uint64_t register_d11;
  uint64_t register_d12;
  uint64_t register_d13;
  uint64_t register_d14;
  uint64_t register_d15;
#endif
#ifdef RTEMS_SMP
  volatile bool is_executing;
#endif
} Context_Control;

static inline void _ARM_Data_memory_barrier( void )
{
#ifdef ARM_MULTILIB_HAS_BARRIER_INSTRUCTIONS
  __asm__ volatile ( "dmb" : : : "memory" );
#else
  RTEMS_COMPILER_MEMORY_BARRIER();
#endif
}

static inline void _ARM_Data_synchronization_barrier( void )
{
#ifdef ARM_MULTILIB_HAS_BARRIER_INSTRUCTIONS
  __asm__ volatile ( "dsb" : : : "memory" );
#else
  RTEMS_COMPILER_MEMORY_BARRIER();
#endif
}

static inline void _ARM_Instruction_synchronization_barrier( void )
{
#ifdef ARM_MULTILIB_HAS_BARRIER_INSTRUCTIONS
  __asm__ volatile ( "isb" : : : "memory" );
#else
  RTEMS_COMPILER_MEMORY_BARRIER();
#endif
}

#if defined(ARM_DISABLE_INLINE_ISR_DISABLE_ENABLE)
uint32_t arm_interrupt_disable( void );
void arm_interrupt_enable( uint32_t level );
void arm_interrupt_flash( uint32_t level );
#else
static inline uint32_t arm_interrupt_disable( void )
{
  uint32_t level;

#if defined(ARM_MULTILIB_ARCH_V4)
  /*
   * Disable only normal interrupts (IRQ).
   *
   * In order to support fast interrupts (FIQ) such that they can do something
   * useful, we have to disable the operating system support for FIQs.  Having
   * operating system support for them would require that FIQs are disabled
   * during critical sections of the operating system and application.  At this
   * level IRQs and FIQs would be equal.  It is true that FIQs could interrupt
   * the non critical sections of IRQs, so here they would have a small
   * advantage.  Without operating system support, the FIQs can execute at any
   * time (of course not during the service of another FIQ). If someone needs
   * operating system support for a FIQ, she can trigger a software interrupt and
   * service the request in a two-step process.
   */
#if __ARM_ARCH >= 7
  __asm__ volatile (
    "mrs %0, cpsr\n"
    "cpsid i\n"
    "isb"
    : "=&r" (level)
  );
#else
  uint32_t arm_switch_reg;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrs %[level], cpsr\n"
    "orr %[arm_switch_reg], %[level], #0x80\n"
    "msr cpsr, %[arm_switch_reg]\n"
    ARM_SWITCH_BACK
    : [arm_switch_reg] "=&r" (arm_switch_reg), [level] "=&r" (level)
  );
#endif
#elif defined(ARM_MULTILIB_ARCH_V7M)
  uint32_t basepri = 0x80;

  __asm__ volatile (
    "mrs %[level], basepri\n"
    "msr basepri_max, %[basepri]\n"
    : [level] "=&r" (level)
    : [basepri] "r" (basepri)
  );
#endif

  return level;
}

static inline void arm_interrupt_enable( uint32_t level )
{
#if defined(ARM_MULTILIB_ARCH_V4)
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "msr cpsr, %[level]\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [level] "r" (level)
  );
#elif defined(ARM_MULTILIB_ARCH_V7M)
  __asm__ volatile (
    "msr basepri, %[level]\n"
    :
    : [level] "r" (level)
  );
#endif
}

static inline void arm_interrupt_flash( uint32_t level )
{
#if defined(ARM_MULTILIB_ARCH_V4)
  uint32_t arm_switch_reg;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrs %[arm_switch_reg], cpsr\n"
    "msr cpsr, %[level]\n"
    "msr cpsr, %[arm_switch_reg]\n"
    ARM_SWITCH_BACK
    : [arm_switch_reg] "=&r" (arm_switch_reg)
    : [level] "r" (level)
  );
#elif defined(ARM_MULTILIB_ARCH_V7M)
  uint32_t basepri;

  __asm__ volatile (
    "mrs %[basepri], basepri\n"
    "msr basepri, %[level]\n"
    "msr basepri, %[basepri]\n"
    : [basepri] "=&r" (basepri)
    : [level] "r" (level)
  );
#endif
}
#endif  /* !ARM_DISABLE_INLINE_ISR_DISABLE_ENABLE */

#define _CPU_ISR_Disable( _isr_cookie ) \
  do { \
    _isr_cookie = arm_interrupt_disable(); \
  } while (0)

#define _CPU_ISR_Enable( _isr_cookie )  \
  arm_interrupt_enable( _isr_cookie )

#define _CPU_ISR_Flash( _isr_cookie ) \
  arm_interrupt_flash( _isr_cookie )

static inline bool _CPU_ISR_Is_enabled( uint32_t level )
{
#if defined(ARM_MULTILIB_ARCH_V4)
  return ( level & 0x80 ) == 0;
#elif defined(ARM_MULTILIB_ARCH_V7M)
  return level == 0;
#endif
}

void _CPU_ISR_Set_level( uint32_t level );

uint32_t _CPU_ISR_Get_level( void );

void _CPU_Context_Initialize(
  Context_Control *the_context,
  void *stack_area_begin,
  size_t stack_area_size,
  uint32_t new_level,
  void (*entry_point)( void ),
  bool is_fp,
  void *tls_area
);

#define _CPU_Context_Get_SP( _context ) \
  (uintptr_t)(_context)->register_sp

#ifdef RTEMS_SMP
  static inline bool _CPU_Context_Get_is_executing(
    const Context_Control *context
  )
  {
    return context->is_executing;
  }

  static inline void _CPU_Context_Set_is_executing(
    Context_Control *context,
    bool is_executing
  )
  {
    context->is_executing = is_executing;
  }

  RTEMS_NO_RETURN void _ARM_Start_multitasking( Context_Control *heir );

  #define _CPU_Start_multitasking( _heir ) _ARM_Start_multitasking( _heir )
#endif

#define _CPU_Context_Restart_self( _the_context ) \
   _CPU_Context_restore( (_the_context) );

#define _CPU_Context_Initialize_fp( _destination ) \
  do { \
    *(*(_destination)) = _CPU_Null_fp_context; \
  } while (0)

/**
 * @brief CPU initialization.
 */
void _CPU_Initialize( void );

typedef void ( *CPU_ISR_handler )( void );

void _CPU_ISR_install_vector(
  uint32_t         vector,
  CPU_ISR_handler  new_handler,
  CPU_ISR_handler *old_handler
);

/**
 * @brief CPU switch context.
 */
void _CPU_Context_switch( Context_Control *run, Context_Control *heir );

RTEMS_NO_RETURN void _CPU_Context_switch_no_return(
  Context_Control *executing,
  Context_Control *heir
);

RTEMS_NO_RETURN void _CPU_Context_restore( Context_Control *new_context );

#if defined(ARM_MULTILIB_ARCH_V7M)
  RTEMS_NO_RETURN void _ARMV7M_Start_multitasking( Context_Control *heir );
  #define _CPU_Start_multitasking _ARMV7M_Start_multitasking
#endif

#ifdef RTEMS_SMP
  uint32_t _CPU_SMP_Initialize( void );

  bool _CPU_SMP_Start_processor( uint32_t cpu_index );

  void _CPU_SMP_Finalize_initialization( uint32_t cpu_count );

  void _CPU_SMP_Prepare_start_multitasking( void );

  static inline uint32_t _CPU_SMP_Get_current_processor( void )
  {
    uint32_t mpidr;

    /* Use ARMv7 Multiprocessor Affinity Register (MPIDR) */
    __asm__ volatile (
      "mrc p15, 0, %[mpidr], c0, c0, 5\n"
      : [mpidr] "=&r" (mpidr)
    );

    return mpidr & 0xffU;
  }

  void _CPU_SMP_Send_interrupt( uint32_t target_processor_index );

  static inline void _ARM_Send_event( void )
  {
    __asm__ volatile ( "sev" : : : "memory" );
  }

  static inline void _ARM_Wait_for_event( void )
  {
    __asm__ volatile ( "wfe" : : : "memory" );
  }
#endif


static inline uint32_t CPU_swap_u32( uint32_t value )
{
#if defined(__thumb2__)
  __asm__ volatile (
    "rev %0, %0"
    : "=r" (value)
    : "0" (value)
  );
  return value;
#elif defined(__thumb__)
  uint32_t byte1, byte2, byte3, byte4, swapped;

  byte4 = (value >> 24) & 0xff;
  byte3 = (value >> 16) & 0xff;
  byte2 = (value >> 8)  & 0xff;
  byte1 =  value & 0xff;

  swapped = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
  return swapped;
#else
  uint32_t tmp = value; /* make compiler warnings go away */
  __asm__ volatile ("EOR %1, %0, %0, ROR #16\n"
                "BIC %1, %1, #0xff0000\n"
                "MOV %0, %0, ROR #8\n"
                "EOR %0, %0, %1, LSR #8\n"
                : "=r" (value), "=r" (tmp)
                : "0" (value), "1" (tmp));
  return value;
#endif
}

static inline uint16_t CPU_swap_u16( uint16_t value )
{
#if defined(__thumb2__)
  __asm__ volatile (
    "rev16 %0, %0"
    : "=r" (value)
    : "0" (value)
  );
  return value;
#else
  return (uint16_t) (((value & 0xffU) << 8) | ((value >> 8) & 0xffU));
#endif
}

typedef uint32_t CPU_Counter_ticks;

uint32_t _CPU_Counter_frequency( void );

CPU_Counter_ticks _CPU_Counter_read( void );

RTEMS_NO_RETURN void *_CPU_Thread_Idle_body( uintptr_t ignored );

#if defined(ARM_MULTILIB_ARCH_V4)

typedef enum {
  ARM_EXCEPTION_RESET = 0,
  ARM_EXCEPTION_UNDEF = 1,
  ARM_EXCEPTION_SWI = 2,
  ARM_EXCEPTION_PREF_ABORT = 3,
  ARM_EXCEPTION_DATA_ABORT = 4,
  ARM_EXCEPTION_RESERVED = 5,
  ARM_EXCEPTION_IRQ = 6,
  ARM_EXCEPTION_FIQ = 7,
  MAX_EXCEPTIONS = 8,
  ARM_EXCEPTION_MAKE_ENUM_32_BIT = 0x7fffffff
} Arm_symbolic_exception_name;

#endif /* defined(ARM_MULTILIB_ARCH_V4) */

typedef struct {
  uint32_t register_fpexc;
  uint32_t register_fpscr;
  uint64_t register_d0;
  uint64_t register_d1;
  uint64_t register_d2;
  uint64_t register_d3;
  uint64_t register_d4;
  uint64_t register_d5;
  uint64_t register_d6;
  uint64_t register_d7;
  uint64_t register_d8;
  uint64_t register_d9;
  uint64_t register_d10;
  uint64_t register_d11;
  uint64_t register_d12;
  uint64_t register_d13;
  uint64_t register_d14;
  uint64_t register_d15;
  uint64_t register_d16;
  uint64_t register_d17;
  uint64_t register_d18;
  uint64_t register_d19;
  uint64_t register_d20;
  uint64_t register_d21;
  uint64_t register_d22;
  uint64_t register_d23;
  uint64_t register_d24;
  uint64_t register_d25;
  uint64_t register_d26;
  uint64_t register_d27;
  uint64_t register_d28;
  uint64_t register_d29;
  uint64_t register_d30;
  uint64_t register_d31;
} ARM_VFP_context;

typedef struct {
  union {
    struct {
      uint32_t register_r0;
      uint32_t register_r1;
      uint32_t register_r2;
      uint32_t register_r3;
      uint32_t register_r4;
      uint32_t register_r5;
      uint32_t register_r6;
      uint32_t register_r7;
      uint32_t register_r8;
      uint32_t register_r9;
      uint32_t register_r10;
      uint32_t register_r11;
      uint32_t register_r12;
      uint32_t register_sp;
      void *register_lr;
      void *register_pc;
    };

    uint32_t registers[ 16 ];
  };
#if defined(ARM_MULTILIB_ARCH_V4)
  uint32_t register_cpsr;
  Arm_symbolic_exception_name vector;
#elif defined(ARM_MULTILIB_ARCH_V6M) || defined(ARM_MULTILIB_ARCH_V7M)
  uint32_t register_xpsr;
  uint32_t vector;
#endif
  const ARM_VFP_context *vfp_context;
  uint32_t reserved_for_stack_alignment;
} CPU_Exception_frame;

void _CPU_Exception_frame_print( const CPU_Exception_frame *frame );

RTEMS_NO_RETURN void _ARM_Exception_default( CPU_Exception_frame *frame );

/** Type that can store a 32-bit integer or a pointer. */
typedef uintptr_t CPU_Uint32ptr;

#ifdef __cplusplus
}
#endif

#endif /* ASM */

/** @} */

#endif /* _RTEMS_SCORE_CPU_H */
