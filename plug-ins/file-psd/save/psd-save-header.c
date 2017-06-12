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

#include "psd-save-header.h"

guint32 save_psd_header( FILE* f, GIMPimage *img  ,GError** error )
{
  gint32 width, height;
  gint16 channels, depth, color_mode;

  /* First, initialize the variables and make
     conversions from gimp variables */

  width = img->width;
  height = img->height;

  switch( img->precision )
    {
    case GIMP_PRECISION_U8_GAMMA:
      depth = 8;
      break;
    case GIMP_PRECISION_U16_GAMMA:
      depth = 16;
      break;
    case GIMP_PRECISION_U32_GAMMA:
      depth = 32;
      break;
    case GIMP_PRECISION_U32_LINEAR:
    case GIMP_PRECISION_U16_LINEAR:
    case GIMP_PRECISION_U8_LINEAR:
    case GIMP_PRECISION_HALF_LINEAR:
    case GIMP_PRECISION_HALF_GAMMA:
    case GIMP_PRECISION_FLOAT_LINEAR:
    case GIMP_PRECISION_FLOAT_GAMMA:
    case GIMP_PRECISION_DOUBLE_LINEAR:
    case GIMP_PRECISION_DOUBLE_GAMMA:
    default:
      return -1;
      break;
    }

  switch( img->image_type )
    {
    case GIMP_RGB_IMAGE:
      channels = 3;
      color_mode = PSD_RGB;
      break;
    case GIMP_RGBA_IMAGE:
      channels = 4;
      color_mode = PSD_RGB;
      break;
    case GIMP_GRAY_IMAGE:
      channels = 1;
      color_mode = PSD_GRAYSCALE;
      break;
    case GIMP_GRAYA_IMAGE:
      channels = 2;
      color_mode = PSD_GRAYSCALE;
      break;
    case GIMP_INDEXED_IMAGE:
      channels = 1;
      color_mode = PSD_INDEXED;
      break;
    case GIMP_INDEXEDA_IMAGE:
      channels = 2;
      color_mode = PSD_INDEXED;
      break;
    default:
      return -1;
      break;
    }

  /* Sanitize variables before writting the header   */

  if( channels < 1 || channels > PSD_MAX_CHANNELS )
    {
      return -1;
    }

  if( width < 1 || width > PSD_MAX_DIMENSIONS ||
      height < 1 || height > PSD_MAX_DIMENSIONS )
    {
      return -1;
    }

  // Note Depth and color_mode are already sanitized

  /* Write the header */

  write4charSignature( "8BPS", f, error );

  //Version
  write16bitInteger( 1, f, error );

  //Reserved ( must be 0 )
  write32bitInteger( 0, f, error );
  write16bitInteger( 0, f, error );

  write16bitInteger( channels,f, error );
  write32bitInteger( height, f, error );
  write32bitInteger( width, f, error );

  write16bitInteger( depth, f, error );
  write16bitInteger( color_mode, f, error );

  g_debug ("\n\n\tChannels: "
           "%d\n\tSize: %dx%d\n\tBPS: %d\n\tMode: %d\n",
           channels,
           height,
           width,
           depth,
           color_mode );

  return 0;
}
