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

#include "psd-load-colormode.h"

/* Local functions */

static void
psd_to_gimp_color_map (guchar *map256);


guint32 read_color_mode( FILE* f, PSDimage* img, gint32 image_ID, GError** error )
{
  guint32 length;
  guchar* color_map = NULL;

  g_debug( "COLOR MODE" );

  length = read32bitInteger( f );
  if( length == -1 ) return -1;
  g_debug ( "Length: %d\n", length );

  //Indexed
  if( img->color_mode == PSD_INDEXED  && length != 768 )
    {
      g_debug("Invalid Indexed Image length: %i", length );
      return -1;
    }

  //Indexed color map
  if( img->color_mode == PSD_INDEXED )
    {
      color_map = g_malloc( length );
      if( fread( color_map, length, 1 , f ) < 1 )
        {
          return -1;
        }

      psd_to_gimp_color_map( color_map );
      gimp_image_set_colormap( image_ID, color_map, 256 );

      g_free( color_map );
    }

  return 0;
}


static void
psd_to_gimp_color_map (guchar *map256)
{
  guchar *tmpmap;
  gint    i;

  tmpmap = g_malloc (768);

  for (i = 0; i < 256; ++i)
    {
      tmpmap[i*3  ] = map256[i];
      tmpmap[i*3+1] = map256[i+256];
      tmpmap[i*3+2] = map256[i+512];
    }

  memcpy (map256, tmpmap, 768);
  g_free (tmpmap);
}
