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

#include "load-utils.h"

FILE* open_file( const gchar* filename, GError* error )
{
  FILE* f;
  struct stat st;

  g_debug ("Open file %s", gimp_filename_to_utf8 (filename));

  if( g_stat( filename, &st ) == -1 )
    {
      return NULL;
    }

  f = g_fopen( filename, "rb" );
  if( f == NULL )
    {
      return NULL;
    }

  return f;
}

void readPascalString( FILE* f, gchar* str, gint32 *str_len )
{

  guchar len;
  str = NULL;

  if( fread( &len, 1, 1, f ) < 1 )
    {
      return;
    }

  //Empty string
  if( len == 0 )
    {
      *str_len = 0;
      if( fread( &len, 1, 1, f ) < 1 )
        {
          g_debug("\t Empty pascal string");
        }
      return;
    }

  str = g_new( gchar, len+1 );

  if( fread( str, 1, len, f ) < 1 )
    {
      return;
    }
  str[len] = '\0';
  *str_len = (gint32) len;

}

void read4charSignature( FILE* f , gchar* signature )
{
  if( fread( signature, 4, 1, f ) < 1 )
    {
      signature = NULL;
    }
}

guint16 read16bitInteger( FILE* f )
{
  guint16 value;

  if( fread( &value, 2, 1, f ) < 1 )
    {
      return -1;
    }

  value = GUINT16_FROM_BE( value );
  return value;
}

guint32 read32bitInteger( FILE* f )
{
  guint32 value;

  if( fread( &value, 4, 1, f ) < 1 )
  {
    return -1;
  }

  value = GUINT32_FROM_BE( value );
  return value;
}

guint32 skipFileBlock( FILE* f, guint32 length )
{
  guint32 position = ftell(f);
  guint32 end = position + length;

  if( fseek( f, end, SEEK_SET ) < 0 ) return -1;

  return 0;
}

guint32 read_raw_imagedata( FILE* f, gint32 layer, GError* error )
{
  gint32 width, height, bpp, length, single_pixel, channels, npixels;
  GeglBuffer* buffer;
  guchar * buffer_pixels, *output_pixels;
  int i,j,k;
  const Babl* format;

  width = gimp_drawable_width( layer );
  height = gimp_drawable_height( layer );
  bpp = gimp_drawable_bpp( layer );

  format = NULL;
  format = get_drawable_format( layer, &channels );
  if( !format ) return -1;

  single_pixel = bpp / channels;
  length = width*height*bpp;

  buffer = gimp_drawable_get_buffer( layer );
  buffer_pixels = g_new( guchar, length );
  output_pixels = g_new( guchar, length );

  gegl_buffer_get( buffer,
                   GEGL_RECTANGLE( 0,0, width, height ),
                   1.0,
                   format,
                   buffer_pixels,
                   GEGL_AUTO_ROWSTRIDE, GEGL_ABYSS_NONE );

  g_debug("WRITTING IMAGEDATA");
  g_debug("width: %i,  height: %i,  channels: %i, depth: %i ",
          width, height, channels, single_pixel );

  for(i=0; i<length; i++)
    {
      g_debug(" Byte %i: %i ", i, buffer_pixels[i] );
    }

  npixels = width*height;
  //Reorder pixel data in RRR GGG BBB AAA ... order
  for( i=0; i<channels; i++)
    {
      for(j=0; j<npixels; j++ )
        {
          for( k=0; k<single_pixel; k++ )
            {
              output_pixels[ i*npixels*single_pixel + j*single_pixel + k ] = buffer_pixels[ j*channels*single_pixel + i*single_pixel + k];
            }
        }
    }

  for(i=0; i<length; i++)
    {
      g_debug(" Byte %i: %i ", i, buffer_pixels[i] );
    }

  //Dump data to file
  fwrite( output_pixels, 1, length, f );

  return 0;
}
