/* FriBidi - Library of BiDi algorithm
 * Copyright (C) 1999 Dov Grobgeld
 * Copyright (C) 2001 Behdad Esfahbod
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#include "fribidi_char_sets.h"

typedef struct
{
  /* Convert the character string "s" to unicode string "us" and
     return it's length. */
  gint (*charset_to_unicode) (guchar * s,
			      /* output */
			      FriBidiChar * us);
  /* Convert the unicode string "us" with length "length" to character
     string "s" and return it's length. */
  gint (*unicode_to_charset) (FriBidiChar * us, gint length,
			      /* output */
			      guchar * s);
  /* Charset's name. */
  guchar *name;
  /* Charset's title. */
  guchar *title;
  /* Comments, if any. */
  guchar *(*desc) (void);
  /* Some charsets like CapRTL may need to change some fribidis tables, by
     calling this function, they can do this changes. */
    gboolean (*enter) (void);
  /* Some charsets like CapRTL may need to change some fribidis tables, by
     calling this function, they can undo their changes, perhaps to enter
     another mode. */
    gboolean (*leave) (void);
}
FriBidiCharSetHandler;

/* Each charset must define the functions and strings named below
   (in _FRIBIDI_ADD_CHAR_SET) or define them as NULL, if not any. */

#define _FRIBIDI_ADD_CHAR_SET(char_set) \
  { \
    fribidi_##char_set##_to_unicode, \
    fribidi_unicode_to_##char_set, \
    fribidi_char_set_name_##char_set, \
    fribidi_char_set_title_##char_set, \
    fribidi_char_set_desc_##char_set, \
    fribidi_char_set_enter_##char_set, \
    fribidi_char_set_leave_##char_set, \
  }

FriBidiCharSetHandler fribidi_char_sets[FRIBIDI_CHAR_SETS_NUM + 1] = {
  {NULL, NULL, "Not Implemented", NULL, NULL, NULL},
  _FRIBIDI_ADD_CHAR_SET (utf8),
  _FRIBIDI_ADD_CHAR_SET (cap_rtl),
  _FRIBIDI_ADD_CHAR_SET (iso8859_6),
  _FRIBIDI_ADD_CHAR_SET (iso8859_8),
  _FRIBIDI_ADD_CHAR_SET (cp1255),
  _FRIBIDI_ADD_CHAR_SET (cp1256),
  _FRIBIDI_ADD_CHAR_SET (isiri_3342),
};

#undef _FRIBIDI_ADD_CHAR_SET

/* Return the charset which name is "s". */
FriBidiCharSet
fribidi_parse_charset (guchar * s)
{
  gint i;

  for (i = FRIBIDI_CHAR_SETS_NUM; i; i--)
    /* Function strcasecmp() is used here, cab be replaced with strcmp(),
       if strcasecmp() is not available. */
    if (strcasecmp (s, fribidi_char_sets[i].name) == 0)
      return i;

  return FRIBIDI_CHARSET_NOT_FOUND;
}


/* Convert the character string "s" in charset "char_set" to unicode
   string "us" and return it's length. */
gint
fribidi_charset_to_unicode (FriBidiCharSet char_set, guchar * s,
			    /* output */
			    FriBidiChar * us)
{
  fribidi_char_set_enter (char_set);
  return fribidi_char_sets[char_set].charset_to_unicode == NULL ? 0 :
    (*fribidi_char_sets[char_set].charset_to_unicode) (s, us);
}

/* Convert the unicode string "us" with length "length" to character
   string "s" in charset "char_set" and return it's length. */
gint
fribidi_unicode_to_charset (FriBidiCharSet char_set, FriBidiChar * us,
			    gint length,
			    /* output */
			    gchar * s)
{
  fribidi_char_set_enter (char_set);
  return fribidi_char_sets[char_set].unicode_to_charset == NULL ? 0 :
    (*fribidi_char_sets[char_set].unicode_to_charset) (us, length, s);
}

/* Return the string containing the name of the charset. */
guchar *
fribidi_char_set_name (FriBidiCharSet char_set)
{
  return fribidi_char_sets[char_set].name == NULL ? (guchar *) "" :
    fribidi_char_sets[char_set].name;
}

/* Return the string containing the title of the charset. */
guchar *
fribidi_char_set_title (FriBidiCharSet char_set)
{
  return fribidi_char_sets[char_set].title == NULL ?
    fribidi_char_set_name (char_set) : fribidi_char_sets[char_set].title;
}

/* Return the string containing the comments about the charset, if any. */
guchar *
fribidi_char_set_desc (FriBidiCharSet char_set)
{
  return fribidi_char_sets[char_set].desc == NULL ?
    NULL : fribidi_char_sets[char_set].desc ();
}

static FriBidiCharSet current_char_set = FRIBIDI_CHARSET_DEFAULT;

/* Some charsets like CapRTL may need to change some fribidis tables, by
   calling this function, they can do this changes. */
gboolean
fribidi_char_set_enter (FriBidiCharSet char_set)
{
  if (char_set != current_char_set && fribidi_char_sets[char_set].enter)
    {
      fribidi_char_set_leave (current_char_set);
      current_char_set = char_set;
      return (*fribidi_char_sets[char_set].enter) ();
    }
  else
    return TRUE;
}

/* Some charsets like CapRTL may need to change some fribidis tables, by
   calling this function, they can undo their changes, maybe to enter
   another mode. */
gboolean
fribidi_char_set_leave (FriBidiCharSet char_set)
{
  if (char_set == current_char_set && fribidi_char_sets[char_set].leave)
    return (*fribidi_char_sets[char_set].leave) ();
  else
    return TRUE;
}
