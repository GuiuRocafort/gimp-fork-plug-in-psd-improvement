/* GIMP - The GNU Image Manipulation Program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * GIMP PSD Plug-in
 * Copyright 2007 by John Marshall
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __PSD_LOAD_UTILS__
#define __PSD_LOAD_UTILS__

#include "config.h"

#include <string.h>
#include <errno.h>

#include <glib/gstdio.h>
#include <zlib.h>
#include <libgimp/gimp.h>
#include "save-utils.h"


/* File opening */
FILE* open_file( const gchar* filename, GError** error );

/* Data reading functions */
void readPascalString( FILE* f, gchar* str, gint32 *str_len );
void read4charSignature( FILE* f, gchar* signature );
guint16 read16bitInteger( FILE* f );
guint32 read32bitInteger( FILE* f );

/* Block skipping */
guint32 skipFileBlock( FILE* f, guint32 length );

guint32 read_raw_imagedata( FILE* f, gint32 layer, GError** error );

#endif
