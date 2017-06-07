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

guint32 save_psd_header( FILE* f, GError** error, gint32 merged_layer, gint32 precision )
{
  gint32 channels;
  gint32 width;
  gint32 height;
  gint16 color_mode;

  write4charSignature( "8BPS", f, error );
  write16bitInteger( 1, f, error );

  //Reserved
  write32bitInteger( 0, f, error );
  write16bitInteger( 0, f, error );

  width = gimp_drawable_width( merged_layer );
  height = gimp_drawable_height( merged_layer );
  get_drawable_format( merged_layer, &channels );

  switch( precision )
    {
    case GIMP_PRECISION_U8_LINEAR:
      return -1;
      break;
    case GIMP_PRECISION_U8_GAMMA:
      precision = 8;
      break;
    case GIMP_PRECISION_U16_LINEAR:
      return -1;
      break;
    case GIMP_PRECISION_U16_GAMMA:
      precision = 16;
      break;
    case GIMP_PRECISION_U32_LINEAR:
      return -1;
    case GIMP_PRECISION_U32_GAMMA:
      precision = 32;
      break;
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

  if( gimp_drawable_is_rgb( merged_layer ) )   color_mode = 3;
  if( gimp_drawable_is_indexed( merged_layer ) ) color_mode = 2;
  if( gimp_drawable_is_gray( merged_layer ) ) color_mode = 1;

  write16bitInteger( channels,f, error );
  write32bitInteger( height, f, error );
  write32bitInteger( width, f, error );

  write16bitInteger( precision, f, error );
  write16bitInteger( color_mode, f, error );

  g_debug ("\n\n\tChannels: "
           "%d\n\tSize: %dx%d\n\tBPS: %d\n\tMode: %d\n",
           channels,
           height,
           width,
           precision,
           color_mode );

  return 0;
}
