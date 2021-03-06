/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
 * Portions copyright (c) 2006-2009 Nokia Corporation.  All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/.
 */

#if defined(G_DISABLE_SINGLE_INCLUDES) && !defined (__GLIB_H_INSIDE__) && !defined (GLIB_COMPILATION)
#error "Only <glib.h> can be included directly."
#endif

#ifndef __G_STRING_H__
#define __G_STRING_H__

#include <glib/gtypes.h>
#include <glib/gunicode.h>
#include <glib/gutils.h>  /* for G_CAN_INLINE */

G_BEGIN_DECLS

typedef struct _GString		GString;
typedef struct _GStringChunk	GStringChunk;

struct _GString
{
  gchar  *str;
  gsize len;    
  gsize allocated_len;
};

/* String Chunks
 */
IMPORT_C GStringChunk* g_string_chunk_new	   (gsize size);  
IMPORT_C void	      g_string_chunk_free	   (GStringChunk *chunk);
IMPORT_C void	      g_string_chunk_clear	   (GStringChunk *chunk);
IMPORT_C gchar*	      g_string_chunk_insert	   (GStringChunk *chunk,
					    const gchar	 *string);
IMPORT_C gchar*	      g_string_chunk_insert_len	   (GStringChunk *chunk,
					    const gchar	 *string,
					    gssize        len);
IMPORT_C gchar*	      g_string_chunk_insert_const  (GStringChunk *chunk,
					    const gchar	 *string);


/* Strings
 */
IMPORT_C GString*     g_string_new	        (const gchar	 *init);
IMPORT_C GString*     g_string_new_len           (const gchar     *init,
                                         gssize           len);   
IMPORT_C GString*     g_string_sized_new         (gsize            dfl_size);  
IMPORT_C gchar*	     g_string_free	        (GString	 *string,
					 gboolean	  free_segment);
IMPORT_C gboolean     g_string_equal             (const GString	 *v,
					 const GString 	 *v2);
IMPORT_C guint        g_string_hash              (const GString   *str);
IMPORT_C GString*     g_string_assign            (GString	 *string,
					 const gchar	 *rval);
IMPORT_C GString*     g_string_truncate          (GString	 *string,
					 gsize		  len);    
IMPORT_C GString*     g_string_set_size          (GString         *string,
					 gsize            len);
IMPORT_C GString*     g_string_insert_len        (GString         *string,
                                         gssize           pos,   
                                         const gchar     *val,
                                         gssize           len);  
IMPORT_C GString*     g_string_append            (GString	 *string,
			                 const gchar	 *val);
IMPORT_C GString*     g_string_append_len        (GString	 *string,
			                 const gchar	 *val,
                                         gssize           len);  
IMPORT_C GString*     g_string_append_c          (GString	 *string,
					 gchar		  c);
IMPORT_C GString*     g_string_append_unichar    (GString	 *string,
					 gunichar	  wc);
IMPORT_C GString*     g_string_prepend           (GString	 *string,
					 const gchar	 *val);
IMPORT_C GString*     g_string_prepend_c         (GString	 *string,
					 gchar		  c);
IMPORT_C GString*     g_string_prepend_unichar   (GString	 *string,
					 gunichar	  wc);
IMPORT_C GString*     g_string_prepend_len       (GString	 *string,
			                 const gchar	 *val,
                                         gssize           len);  
IMPORT_C GString*     g_string_insert            (GString	 *string,
					 gssize		  pos,    
					 const gchar	 *val);
IMPORT_C GString*     g_string_insert_c          (GString	 *string,
					 gssize		  pos,    
					 gchar		  c);
IMPORT_C GString*     g_string_insert_unichar    (GString	 *string,
					 gssize		  pos,    
					 gunichar	  wc);
IMPORT_C GString*     g_string_overwrite         (GString	 *string,
					 gsize		  pos,    
					 const gchar	 *val);
IMPORT_C GString*     g_string_overwrite_len     (GString	 *string,
					 gsize		  pos,    
					 const gchar	 *val,
					 gssize           len);
IMPORT_C GString*     g_string_erase	        (GString	 *string,
					 gssize		  pos,
					 gssize		  len);
IMPORT_C GString*     g_string_ascii_down        (GString	 *string);
IMPORT_C GString*     g_string_ascii_up          (GString	 *string);
IMPORT_C void         g_string_vprintf           (GString	 *string,
					 const gchar	 *format,
					 va_list          args);
IMPORT_C void         g_string_printf            (GString	 *string,
					 const gchar	 *format,
					 ...) G_GNUC_PRINTF (2, 3);
IMPORT_C void         g_string_append_vprintf    (GString	 *string,
					 const gchar	 *format,
					 va_list          args);
IMPORT_C void         g_string_append_printf     (GString	 *string,
					 const gchar	 *format,
					 ...) G_GNUC_PRINTF (2, 3);
IMPORT_C GString *    g_string_append_uri_escaped(GString         *string,
					 const char      *unescaped,
					 const char      *reserved_chars_allowed,
					 gboolean         allow_utf8);

/* -- optimize g_strig_append_c --- */
#if (defined G_CAN_INLINE && !defined BUILDING_LIBGLIB)
static inline GString*
g_string_append_c_inline (GString *gstring,
                          gchar    c)
{
  if (gstring->len + 1 < gstring->allocated_len)
    {
      gstring->str[gstring->len++] = c;
      gstring->str[gstring->len] = 0;
    }
  else
    g_string_insert_c (gstring, -1, c);
  return gstring;
}
#define g_string_append_c(gstr,c)       g_string_append_c_inline (gstr, c)
#else
IMPORT_C	GString*     g_string_append_c          (GString	 *string,
					 gchar		  c);
#endif /* G_CAN_INLINE */


#ifndef G_DISABLE_DEPRECATED

/* The following two functions are deprecated and will be removed in
 * the next major release. They use the locale-specific tolower and
 * toupper, which is almost never the right thing.
 */

IMPORT_C GString*     g_string_down              (GString	 *string);
IMPORT_C GString*     g_string_up                (GString	 *string);

/* These aliases are included for compatibility. */
#define	g_string_sprintf	g_string_printf
#define	g_string_sprintfa	g_string_append_printf

#endif /* G_DISABLE_DEPRECATED */

G_END_DECLS

#endif /* __G_STRING_H__ */
