/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 2012-2014, 2018 Chris Johns <chrisj@rtems.org>
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
/**
 * @file
 *
 * @ingroup rtems_rtl
 *
 * @brief RTEMS Run-Time Linker Object File Symbol Table.
 */

#if !defined (_RTEMS_RTL_SYM_H_)
#define _RTEMS_RTL_SYM_H_

#include <rtems.h>
#include "rtl-obj-fwd.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * An object file symbol.
 */
typedef struct rtems_rtl_obj_sym
{
  rtems_chain_node node;    /**< The node's link in the chain. */
  const char*      name;    /**< The symbol's name. */
  void*            value;   /**< The value of the symbol. */
  uint32_t         data;    /**< Format specific data. */
} rtems_rtl_obj_sym;

/**
 * Table of symbols stored in a hash table.
 */
typedef struct rtems_rtl_symbols
{
  rtems_chain_control* buckets;
  size_t               nbuckets;
} rtems_rtl_symbols;

/**
 * A TLS variable offset call. There is one per base image TLS
 * variable.
 */
typedef size_t (*rtems_rtl_tls_offset_func)(void);

/**
 * A TLS symbol offset entry. It is used with an exported symbol table
 * to find a TSL table offset for a variable at runtime.
 */
typedef struct rtems_rtl_tls_offset
{
  size_t                    index;  /** exported symbol table index */
  rtems_rtl_tls_offset_func offset; /** TLS offset function */
} rtems_rtl_tls_offset;

/**
 * Open a symbol table with the specified number of buckets.
 *
 * @param symbols The symbol table to open.
 * @param buckets The number of buckets in the hash table.
 * @retval true The symbol is open.
 * @retval false The symbol table could not created. The RTL
 *               error has the error.
 */
bool rtems_rtl_symbol_table_open (rtems_rtl_symbols* symbols,
                                  size_t             buckets);

/**
 * Close the table and erase the hash table.
 *
 * @param symbols Close the symbol table.
 */
void rtems_rtl_symbol_table_close (rtems_rtl_symbols* symbols);

/**
 * Insert a symbol into a symbol table.
 *
 * @param symbols Symbol table
 * @param symbols Symbol to add
 */
void rtems_rtl_symbol_global_insert (rtems_rtl_symbols* symbols,
                                     rtems_rtl_obj_sym* symbol);

/**
 * Add a table of exported symbols to the symbol table.
 *
 * The export table is a series of symbol records and each record has two
 * fields:
 *
 *  1. label
 *  2. address
 *
 * The 'label' is an ASCIIZ string of variable length. The address is of size
 * of an unsigned long for the target running the link editor. The byte order
 * is defined by the machine type because the table should be built by the
 * target compiler.
 *
 * The table is terminated with a nul string followed by the bytes 0xDE, 0xAD,
 * 0xBE, and 0xEF. This avoids alignments issues.
 *
 * @param obj The object table the symbols are for.
 * @param esyms The exported symbol table.
 * @param size The size of the table in bytes.
 * @param tls_offsets The TLS offsets table. If NULL none provided.
 * @param tls_size The number TLS offset entries in the table.
 */
bool rtems_rtl_symbol_global_add (rtems_rtl_obj*              obj,
                                  const unsigned char*        esyms,
                                  unsigned int                size,
                                  const rtems_rtl_tls_offset* tls_offsets,
                                  unsigned int                tls_size);

/**
 * Find a symbol given the symbol label in the global symbol table.
 *
 * @param name The name as an ASCIIZ string.
 * @retval NULL No symbol found.
 * @return rtems_rtl_obj_sym* Reference to the symbol.
 */
rtems_rtl_obj_sym* rtems_rtl_symbol_global_find (const char* name);

/**
 * Sort an object file's local and global symbol table. This needs to
 * be done before calling @ref rtems_rtl_symbol_obj_find as it
 * performs a binary search on the tables.
 *
 * @param obj The object file to sort.
 */
void rtems_rtl_symbol_obj_sort (rtems_rtl_obj* obj);

/**
 * Find a symbol given the symbol label in the local object file.
 *
 * @param obj The object file to search.
 * @param name The name as an ASCIIZ string.
 * @retval NULL No symbol found.
 * @return rtems_rtl_obj_sym* Reference to the symbol.
 */
rtems_rtl_obj_sym* rtems_rtl_symbol_obj_find (rtems_rtl_obj* obj,
                                              const char*    name);

/**
 * Add the object file's symbols to the global table.
 *
 * @param obj The object file the symbols are to be added.
 */
void rtems_rtl_symbol_obj_add (rtems_rtl_obj* obj);

/**
 * Erase the object file's local symbols.
 *
 * @param obj The object file the local symbols are to be erased from.
 */
void rtems_rtl_symbol_obj_erase_local (rtems_rtl_obj* obj);

/**
 * Erase the object file's symbols.
 *
 * @param obj The object file the symbols are to be erased from.
 */
void rtems_rtl_symbol_obj_erase (rtems_rtl_obj* obj);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
