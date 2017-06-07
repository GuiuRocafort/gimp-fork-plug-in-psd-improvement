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

#include "psd-load-layer-mask.h"

guint32 read_layer_mask_info( FILE* f )
{
  guint32 skip;
  guint32 length;

  g_debug("LAYER AND MASK");

  length = read32bitInteger( f );
  if( length == -1 ) return -1;

  g_debug("Length: %d" , length );

  if( length > 0 )
    {
      g_debug("SKIPPING\n" );
      skip = skipFileBlock( f, length );
      if( skip == -1 )
        {
          return -1;
        }
    }

  return 0;
}
