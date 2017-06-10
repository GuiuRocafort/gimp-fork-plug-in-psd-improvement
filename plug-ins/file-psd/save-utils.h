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

#ifndef __SAVE_UTILS__
#define __SAVE_UTILS__

#include "common.h"

FILE*
open_file_wb( const gchar* filename,
              GError** error );

guint32
write1Byte( guchar c,
            FILE* f,
            GError **error );

guint32
write4charSignature( const gchar* sig,
                     FILE* f,
                     GError** error );

guint32
write16bitInteger( gint16 value,
                   FILE* f,
                   GError** error );

guint32
write32bitInteger( gint32 value,
                   FILE* f,
                   GError** error );

const Babl*
get_drawable_format( gint32 layer,
                     gint32* channels );

guint32
write_raw_imagedata( FILE* f,
                     gint32 layer,
                     GError** error );

void
get_psd_layer_blending_mode( gint32 layer,
                             gchar* psdMode );

gint32
write_pascal_string (const gchar  *src,
                     guint16       mod_len,
                     FILE         *f,
                     GError      **error);

#endif
