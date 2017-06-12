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

#include "psd-load-img-res.h"

guint32 read_img_res( FILE* f, GError** error )
{
  guint32 length;

  g_debug("IMAGE RESOURCES");

  //Length of the section
  length = read32bitInteger( f );

  g_debug( "Length: %d\n" , length );

  g_debug("SKIPPING");
  skipFileBlock( f, length );

  return 0;
}

guint32 read_resource( FILE* f, GError** error )
{
  gchar signature[4];
  PSDImageResID resource;
  gchar* pascal_string = NULL;
  gint32 string_len;
  guint32 resource_length;
  long int start, end;

  read4charSignature( f, signature );

  resource = read16bitInteger( f );
  readPascalString( f, pascal_string, &string_len );
  resource_length = read32bitInteger( f );

  start = ftell(f);
  g_debug( "\tStart: %li ", start );

  g_debug("\tResource ID: %i ", resource );
  g_debug(" \tLenght: %d ", resource_length  );

  //LOAD RESOURCES HERE

  if( resource_length % 2 != 0 ){
    resource_length++;
  }

  //Set cursor to the end of the file resource
  end = start + resource_length;
  fseek( f, end, SEEK_SET );

  return 0;
}
