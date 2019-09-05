/**************************************************
 *
 * Intrinsic functions for the IAR Embedded Workbench for MSP430.
 *
 * Copyright 2002-2008 IAR Systems AB.
 *
 * $Revision: 12738 $
 *
 **************************************************/

#ifndef __INTRINSICS_H
#define __INTRINSICS_H

#ifndef _SYSTEM_BUILD
  #pragma system_include
#endif

#pragma language=save
#pragma language=extended


/*
 * Interrupt state, used by "__get_interrupt_state" and
 * "__set_interrupt_state".
 */

typedef unsigned short __istate_t;

/* Deprecated. */
typedef __istate_t istate_t;


#ifdef __cplusplus
extern "C"
{
#endif
  __intrinsic void __no_operation(void);
  __intrinsic void __enable_interrupt(void);
  __intrinsic void __disable_interrupt(void);

  __intrinsic __istate_t __get_interrupt_state(void);
  __intrinsic void       __set_interrupt_state(__istate_t);

  __intrinsic void __op_code(unsigned short);

  __intrinsic unsigned short __swap_bytes(unsigned short);

  __intrinsic long __code_distance(void);

  __intrinsic void           __bic_SR_register(unsigned short);
  __intrinsic void           __bis_SR_register(unsigned short);
  __intrinsic unsigned short __get_SR_register(void);

  __intrinsic void           __bic_SR_register_on_exit(unsigned short);
  __intrinsic void           __bis_SR_register_on_exit(unsigned short);
  __intrinsic unsigned short __get_SR_register_on_exit(void);

  /* Binary encoded decimal operations. */
  __intrinsic unsigned short     __bcd_add_short(unsigned short,
                                                 unsigned short);

  __intrinsic unsigned long      __bcd_add_long (unsigned long,
                                                 unsigned long);

  __intrinsic unsigned long long __bcd_add_long_long(unsigned long long,
                                                     unsigned long long);

  /* Saturated operations. */
  __intrinsic signed char __saturated_add_signed_char     (signed char,
                                                           signed char);
  __intrinsic short       __saturated_add_signed_short    (short, short);
  __intrinsic long        __saturated_add_signed_long     (long, long);
  __intrinsic long long   __saturated_add_signed_long_long(long long,
                                                           long long);

  __intrinsic signed char __saturated_sub_signed_char     (signed char,
                                                           signed char);
  __intrinsic short       __saturated_sub_signed_short    (short, short);
  __intrinsic long        __saturated_sub_signed_long     (long, long);
  __intrinsic long long   __saturated_sub_signed_long_long(long long,
                                                           long long);

  __intrinsic unsigned char  __saturated_add_unsigned_char (unsigned char,
                                                            unsigned char);
  __intrinsic unsigned short __saturated_add_unsigned_short(unsigned short,
                                                            unsigned short);
  __intrinsic unsigned long  __saturated_add_unsigned_long (unsigned long,
                                                            unsigned long);
  __intrinsic unsigned long long __saturated_add_unsigned_long_long(
    unsigned long long,
    unsigned long long);

  __intrinsic unsigned char  __saturated_sub_unsigned_char (unsigned char,
                                                            unsigned char);
  __intrinsic unsigned short __saturated_sub_unsigned_short(unsigned short,
                                                            unsigned short);
  __intrinsic unsigned long  __saturated_sub_unsigned_long (unsigned long,
                                                            unsigned long);
  __intrinsic unsigned long long __saturated_sub_unsigned_long_long(
    unsigned long long,
    unsigned long long);


  /*
   * Support for efficient switch:es. E.g. switch(__even_in_range(x, 10))
   *
   * Note that the value must be even and in the range from 0 to
   * __bound, inclusive. No code will be generated that checks this.
   *
   * This is typically used inside interrupt dispatch functions, to
   * switch on special processor registers like TAIV.
   */

  __intrinsic unsigned short __even_in_range(unsigned short __value,
                                             unsigned short __bound);

  /* Insert a delay with a specific number of cycles. */
  __intrinsic void __delay_cycles(unsigned long __cycles);

  /*
   * The following R4/R5 intrinsic functions are only available when
   * the corresponding register is locked.
   */

  __intrinsic unsigned short __get_R4_register(void);
  __intrinsic void           __set_R4_register(unsigned short);

  __intrinsic unsigned short __get_R5_register(void);
  __intrinsic void           __set_R5_register(unsigned short);

  __intrinsic unsigned short __get_SP_register(void);
  __intrinsic void           __set_SP_register(unsigned short);


  /*
   * If the application provides this function, it is called by the
   * startup code before variables are initialized. If the function
   * returns 0 the data segments will not be initialized.
   */
  __intrinsic int __low_level_init(void);


  /* ----------------------------------------
   * MSP430X-specific intrinsic functions.
   */

  /*
   * Intrinsic functions to allow access to the full 1 Mbyte memory
   * range in small data model.
   *
   * The functions are available in medium and large data model
   * aswell, however it is recommended to access memory using normal
   * __data20 variables and/or pointers.
   *
   * Please note that interrupts must be disabled when the following
   * intrinsics are used.
   */

  __intrinsic void __data20_write_char (unsigned long  __addr,
                                        unsigned char  __value);

  __intrinsic void __data20_write_short(unsigned long  __addr,
                                        unsigned short __value);

  __intrinsic void __data20_write_long (unsigned long  __addr,
                                        unsigned long  __value);

  __intrinsic unsigned char  __data20_read_char (unsigned long __addr);
  __intrinsic unsigned short __data20_read_short(unsigned long __addr);
  __intrinsic unsigned long  __data20_read_long (unsigned long __addr);

  /*
   * The following two functions can be used to access 20-bit SFRs in the
   * lower 64kB. They are only available in extended mode (--core=430X).
   */
  __intrinsic void __data16_write_addr (unsigned short __addr,
                                        unsigned long  __value);

  __intrinsic unsigned long  __data16_read_addr (unsigned short __addr);

#if __REGISTER_MODEL__ == __REGISTER_MODEL_REG20__
  int __data20_setjmp(__JMP_BUF_ELEMENT_TYPE__ __data20 *);
  void __data20_longjmp(__JMP_BUF_ELEMENT_TYPE__ __data20 *, int);
#endif

#ifdef __cplusplus
}
#endif


/*
 * Alias for locations used for global register variables.  For example,
 * "__no_init __regvar int x @ __R4;".
 */

#define __R4 4
#define __R5 5

/*
 * Control bits in the processor status register, SR.
 */

#define __SR_GIE     (1<<3)
#define __SR_CPU_OFF (1<<4)
#define __SR_OSC_OFF (1<<5)
#define __SR_SCG0    (1<<6)
#define __SR_SCG1    (1<<7)


/*
 * Restore GIE without affecting other parts of the status register.
 *
 * Intended usage:
 *     __istate_t state = __get_interrupt_state();
 *     __disable_interrupt();
 *     ...
 *     __bis_GIE_interrupt_state(state);
 */

#define __bis_GIE_interrupt_state(s) (__bis_SR_register((s) & __SR_GIE))


/*
 * Functions for controlling the processor operation modes.
 */

#define __low_power_mode_0() (__bis_SR_register(  __SR_GIE      \
                                                | __SR_CPU_OFF))

#define __low_power_mode_1() (__bis_SR_register(  __SR_GIE      \
                                                | __SR_CPU_OFF  \
                                                | __SR_SCG0))

#define __low_power_mode_2() (__bis_SR_register(  __SR_GIE      \
                                                | __SR_CPU_OFF  \
                                                | __SR_SCG1))

#define __low_power_mode_3()                    \
  (__bis_SR_register(  __SR_GIE                 \
                     | __SR_CPU_OFF             \
                     | __SR_SCG0                \
                     | __SR_SCG1))

#define __low_power_mode_4()                    \
  (__bis_SR_register(  __SR_GIE                 \
                     | __SR_CPU_OFF             \
                     | __SR_SCG0                \
                     | __SR_SCG1                \
                     | __SR_OSC_OFF))

#define __low_power_mode_off_on_exit()          \
  (__bic_SR_register_on_exit(  __SR_CPU_OFF     \
                             | __SR_SCG0        \
                             | __SR_SCG1        \
                             | __SR_OSC_OFF))

#pragma language=restore

#endif /* __INTRINSICS_H */
