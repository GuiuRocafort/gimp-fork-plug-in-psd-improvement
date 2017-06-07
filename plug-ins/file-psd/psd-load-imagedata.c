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

#include "psd-load-imagedata.h"

guint32 read_image_data( FILE* f, guint32 image_ID, PSDimage *img, GError** error  )
{
  guint32 compression;

  compression = read16bitInteger(f);

  g_debug("MERGED IMAGE DATA ");

  switch( compression ){
    case PSD_COMP_RAW:
      g_debug("Compression: RAW ");
      if( read_raw_image_data( f, image_ID, img, error ) == -1 ) return -1;
      break;
    case PSD_COMP_RLE:
      g_debug("Image Compression: RLE ");
      return -1;
      break;
    case PSD_COMP_ZIP:
      g_debug("Image Compression: ZIP w/o prediction " );
      return -1;
      break;
    case PSD_COMP_ZIP_PRED:
      g_debug("Image Compression: ZIP w prediction ");
      return -1;
      break;
    }

  return 0;
}

guint32 read_raw_image_data( FILE* f, guint32 image_ID, PSDimage *img ,GError **error )
{

  GeglBuffer* buffer;
  gint32 width, height, bpp, length, channels, npixels, layer_ID;
  guchar* input_pixels, **channel_data;
  const Babl* pixel_format;
  int i,j,k;

  width = img->width;
  height = img->height;
  bpp = img->depth / 8;
  channels = img->channels;
  npixels = width*height;
  length = npixels*channels*bpp;
  pixel_format = img->pixel_format;

  g_debug("Width: %i  Height: %i  BPP: %i Channels: %i, Color mode: %i ",
          img->width, img->height, img->depth, img->channels, img->color_mode );

  layer_ID = gimp_layer_new( image_ID,
                             "Background",
                             img->width,
                             img->height,
                             GIMP_RGB_IMAGE,
                             100,
                             GIMP_LAYER_MODE_NORMAL_LEGACY
                             );

  if( img->has_alpha )
    {
      gimp_layer_add_alpha( layer_ID );
    }

  buffer = gimp_drawable_get_buffer( layer_ID );

  input_pixels = g_new( guchar, length );
  channel_data = g_new( guchar* , channels );

  //Read channels
  for( i=0; i<channels; i++)
    {
      channel_data[i] = g_new( guchar, npixels*bpp );
      fread( channel_data [i], 1, npixels*bpp, f );
    }

  //Assemble pixel array
  for(i=0; i<npixels; i++ )
    {
      for(j=0; j<channels; j++)
        {
          for(k=0; k<bpp; k++)
            {
              input_pixels[ i*channels*bpp + j*bpp + k] = channel_data[j][i*bpp + k];
            }
        }
    }

  //Set GeglBuffer
  gegl_buffer_set( buffer,
                   GEGL_RECTANGLE( 0,0, width, height ),
                   0,
                   pixel_format,
                   input_pixels,
                   GEGL_AUTO_ROWSTRIDE
                   );

  g_object_unref( buffer );

  gimp_image_insert_layer( image_ID,
                           layer_ID,
                           -1,
                           0 );

  return 0;
}
