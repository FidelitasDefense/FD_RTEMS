/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup rtems_rtl
 *
 * @brief RTEMS Run-Time Linker Object File Symbol Table.
 */

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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <rtems/rtl/rtl.h>
#include "rtl-error.h"
#include <rtems/rtl/rtl-sym.h>
#include <rtems/rtl/rtl-trace.h>

static uint_fast32_t
rtems_rtl_symbol_hash (const char *s)
{
  uint_fast32_t h = 5381;
  unsigned char c;
  for (c = *s; c != '\0'; c = *++s)
    h = h * 33 + c;
  return h & 0xffffffff;
}

static const rtems_rtl_tls_offset*
rtems_rtl_symbol_find_tls_offset (size_t                      index,
                                  const rtems_rtl_tls_offset* tls_offsets,
                                  size_t                      tls_size)
{
  size_t entry;
  for (entry = 0; entry < tls_size; ++entry)
  {
    if (tls_offsets[entry].index == index)
    {
      return &tls_offsets[entry];
    }
  }
  return NULL;
}

bool
rtems_rtl_symbol_table_open (rtems_rtl_symbols* symbols,
                             size_t             buckets)
{
  symbols->buckets = rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_SYMBOL,
                                          buckets * sizeof (rtems_chain_control),
                                          true);
  if (!symbols->buckets)
  {
    rtems_rtl_set_error (ENOMEM, "no memory for global symbol table");
    return false;
  }
  symbols->nbuckets = buckets;
  for (buckets = 0; buckets < symbols->nbuckets; ++buckets)
    rtems_chain_initialize_empty (&symbols->buckets[buckets]);
  return true;
}

void
rtems_rtl_symbol_table_close (rtems_rtl_symbols* symbols)
{
  rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_SYMBOL, symbols->buckets);
}

void
rtems_rtl_symbol_global_insert (rtems_rtl_symbols* symbols,
                                rtems_rtl_obj_sym* symbol)
{
  uint_fast32_t hash = rtems_rtl_symbol_hash (symbol->name);
  rtems_chain_append (&symbols->buckets[hash % symbols->nbuckets],
                      &symbol->node);
}

bool
rtems_rtl_symbol_global_add (rtems_rtl_obj*              obj,
                             const unsigned char*        esyms,
                             unsigned int                size,
                             const rtems_rtl_tls_offset* tls_offsets,
                             unsigned int                tls_size)
{
  rtems_rtl_symbols* symbols;
  rtems_rtl_obj_sym* sym;
  size_t             count;
  size_t             s;
  uint32_t           marker;

  count = 0;
  s = 0;
  while ((s < size) && (esyms[s] != 0))
  {
    int l = strlen ((char*) &esyms[s]);
    if ((esyms[s + l] != '\0') || ((s + l) > size))
    {
      rtems_rtl_set_error (EINVAL, "invalid exported symbol table");
      return false;
    }
    ++count;
    s += l + sizeof (void *) + 1;
  }

  /*
   * Check this is the correct end of the table.
   */
  marker = esyms[s + 1];
  marker <<= 8;
  marker |= esyms[s + 2];
  marker <<= 8;
  marker |= esyms[s + 3];
  marker <<= 8;
  marker |= esyms[s + 4];

  if (marker != 0xdeadbeefUL)
  {
    rtems_rtl_set_error (ENOMEM, "invalid export symbol table");
    return false;
  }

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_GLOBAL_SYM))
    printf ("rtl: global symbol add: %zi\n", count);

  obj->global_size = count * sizeof (rtems_rtl_obj_sym);
  obj->global_table = rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_SYMBOL,
                                           obj->global_size, true);
  if (!obj->global_table)
  {
    obj->global_size = 0;
    rtems_rtl_set_error (ENOMEM, "no memory for global symbols");
    return false;
  }

  symbols = rtems_rtl_global_symbols ();

  obj->global_syms = count;

  count = 0;
  s = 0;
  sym = obj->global_table;

  while ((s < size) && (esyms[s] != 0))
  {
    /*
     * Copy the void* using a union and memcpy to avoid any strict aliasing or
     * alignment issues. The variable length of the label and the packed nature
     * of the table means casting is not suitable.
     */
    union {
      uint8_t data[sizeof (void*)];
      void*   voidp;
    } copy_voidp;
    const rtems_rtl_tls_offset* tls_off;
    int b;

    sym->name = (const char*) &esyms[s];
    s += strlen (sym->name) + 1;
    for (b = 0; b < sizeof (void*); ++b, ++s)
      copy_voidp.data[b] = esyms[s];
    tls_off = rtems_rtl_symbol_find_tls_offset (count, tls_offsets, tls_size);
    if (tls_off == NULL) {
      sym->value = copy_voidp.voidp;
    } else {
      sym->value = (void*) tls_off->offset();
    }
    if (rtems_rtl_trace (RTEMS_RTL_TRACE_GLOBAL_SYM))
      printf ("rtl: esyms: %s -> %8p\n", sym->name, sym->value);
    if (rtems_rtl_symbol_global_find (sym->name) == NULL)
      rtems_rtl_symbol_global_insert (symbols, sym);
    ++count;
    ++sym;
  }

  return true;
}

rtems_rtl_obj_sym*
rtems_rtl_symbol_global_find (const char* name)
{
  rtems_rtl_symbols*   symbols;
  uint_fast32_t        hash;
  rtems_chain_control* bucket;
  rtems_chain_node*    node;

  symbols = rtems_rtl_global_symbols ();

  hash = rtems_rtl_symbol_hash (name);
  bucket = &symbols->buckets[hash % symbols->nbuckets];
  node = rtems_chain_first (bucket);

  while (!rtems_chain_is_tail (bucket, node))
  {
    rtems_rtl_obj_sym* sym = (rtems_rtl_obj_sym*) node;
    /*
     * Use the hash. I could add this to the symbol but it uses more memory.
     */
    if (strcmp (name, sym->name) == 0)
      return sym;
    node = rtems_chain_next (node);
  }

  return NULL;
}

static int
rtems_rtl_symbol_obj_compare (const void* a, const void* b)
{
  const rtems_rtl_obj_sym* sa;
  const rtems_rtl_obj_sym* sb;
  sa = (const rtems_rtl_obj_sym*) a;
  sb = (const rtems_rtl_obj_sym*) b;
  return strcmp (sa->name, sb->name);
}

void
rtems_rtl_symbol_obj_sort (rtems_rtl_obj* obj)
{
  qsort (obj->local_table,
         obj->local_syms,
         sizeof (rtems_rtl_obj_sym),
         rtems_rtl_symbol_obj_compare);
  qsort (obj->global_table,
         obj->global_syms,
         sizeof (rtems_rtl_obj_sym),
         rtems_rtl_symbol_obj_compare);
}

rtems_rtl_obj_sym*
rtems_rtl_symbol_obj_find (rtems_rtl_obj* obj, const char* name)
{
  /*
   * Check the object file's symbols first. If not found search the
   * global symbol table.
   */
  if (obj->local_syms)
  {
    rtems_rtl_obj_sym* match;
    rtems_rtl_obj_sym  key = { 0 };
    key.name = name;
    match = bsearch (&key, obj->local_table,
                     obj->local_syms,
                     sizeof (rtems_rtl_obj_sym),
                     rtems_rtl_symbol_obj_compare);
    if (match != NULL)
      return match;
  }
  if (obj->global_syms)
  {
    rtems_rtl_obj_sym* match;
    rtems_rtl_obj_sym  key = { 0 };
    key.name = name;
    match = bsearch (&key, obj->global_table,
                     obj->global_syms,
                     sizeof (rtems_rtl_obj_sym),
                     rtems_rtl_symbol_obj_compare);
    if (match != NULL)
      return match;
  }
  return rtems_rtl_symbol_global_find (name);
}

void
rtems_rtl_symbol_obj_add (rtems_rtl_obj* obj)
{
  rtems_rtl_symbols* symbols;
  rtems_rtl_obj_sym* sym;
  size_t             s;

  symbols = rtems_rtl_global_symbols ();

  for (s = 0, sym = obj->global_table; s < obj->global_syms; ++s, ++sym)
    rtems_rtl_symbol_global_insert (symbols, sym);
}

void
rtems_rtl_symbol_obj_erase_local (rtems_rtl_obj* obj)
{
  if (obj->local_table)
  {
    rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_SYMBOL, obj->local_table);
    obj->local_table = NULL;
    obj->local_size = 0;
    obj->local_syms = 0;
  }
}

void
rtems_rtl_symbol_obj_erase (rtems_rtl_obj* obj)
{
  rtems_rtl_symbol_obj_erase_local (obj);
  if (obj->global_table)
  {
    rtems_rtl_obj_sym* sym;
    size_t             s;
    for (s = 0, sym = obj->global_table; s < obj->global_syms; ++s, ++sym)
        if (!rtems_chain_is_node_off_chain (&sym->node))
          rtems_chain_extract (&sym->node);
    rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_SYMBOL, obj->global_table);
    obj->global_table = NULL;
    obj->global_size = 0;
    obj->global_syms = 0;
  }
}
