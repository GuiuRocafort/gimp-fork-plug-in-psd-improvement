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

#include "psd-load-header.h"

guint32 read_file_header( FILE* f, PSDimage *img, GError** error )
{
  gchar signature[4];
  guint16 version;
  guint32 reserved1;
  guint16 reserved2;

  read4charSignature( f, signature );
  version = read16bitInteger( f );
  reserved1 = read32bitInteger( f );
  reserved2 = read16bitInteger( f );
  img->channels = read16bitInteger( f );
  img->height = read32bitInteger( f );
  img->width = read32bitInteger( f );
  img->depth = read16bitInteger ( f );
  img->color_mode = read16bitInteger( f );
  img->has_alpha = FALSE;

  g_debug ("\n\n\tSig: %.4s\n\tVer: %d\n\tChannels: "
           "%d\n\tSize: %dx%d\n\tBPS: %d\n\tMode: %d\n",
           signature, version, img->channels,
           img->height, img->width,
           img->depth, img->color_mode );

  if( g_strcmp0( signature, "8BPS" ) != 0 )
    {
      //FIXME
      //g_error("Signature is not 8BPS");
    }

  if( version != 1 )
    {
      g_error("PSD Version not supported");
      return -1;
    }

  if( reserved1 != 0 || reserved2 != 0 )
    {
      g_error("Reserved bytes not set to 0 ");
      return -1;
    }

  if( img->channels < 1 || img->channels > 56 )
    {
      g_error("Invalid number of channels");
      return -1;
    }

  if( img->height < 1 || img->height > 30000 )
    {
      g_error("Invalid height value" );
      return -1;
    }

  if( img->width < 1 || img->width > 30000 )
    {
      g_error("Invalid width value");
      return -1;
    }

  switch (img->depth)
    {
    case 32:
      img->gimp_precision = GIMP_PRECISION_U32_GAMMA;
      break;
    case 16:
      img->gimp_precision = GIMP_PRECISION_U16_GAMMA;
      break;
    case 8:
      img->gimp_precision = GIMP_PRECISION_U8_GAMMA;
      break;
    case 1:
      g_error( "1 bit depth not supported ");
      return -1;
      break;
    default:
      /* Precision not supported */
      g_error ("Invalid precision value");
      return -1;
      break;
    }

  switch (img->color_mode)
    {
    case PSD_GRAYSCALE:
      img->gimp_base_type = GIMP_GRAY;
      break;
    case PSD_RGB:
      img->gimp_base_type = GIMP_RGB;
      break;
    case PSD_INDEXED:
      img->gimp_base_type = GIMP_INDEXED;
      break;
    case PSD_BITMAP:
    case PSD_CMYK:
    case PSD_MULTICHANNEL:
    case PSD_DUOTONE:
    case PSD_LAB:
      g_error(" Not supported color mode %i ", img->color_mode );
      return -1;
      break;
    default:
      /* Color mode already validated - should not be here */
      g_warning ("Invalid color mode");
      return -1;
      break;
    }

  //RGB
  if( img->gimp_base_type == GIMP_RGB )
    {
      if( img->channels == 3 )
        {
          img->gimp_image_type = GIMP_RGB_IMAGE;
          switch( img->gimp_precision )
            {
            case GIMP_PRECISION_U8_GAMMA:
              img->pixel_format = babl_format("R'G'B' u8");
              break;
            case GIMP_PRECISION_U16_GAMMA:
              img->pixel_format = babl_format("R'G'B' u16");
              break;
            case GIMP_PRECISION_U32_GAMMA:
              img->pixel_format = babl_format("R'G'B' u32");
              break;
            default:
              return -1;
            }
        }
      else if( img->channels == 4 )
        {
          img->has_alpha = TRUE;
          img->gimp_image_type = GIMP_RGBA_IMAGE;
          switch( img->gimp_precision )
            {
            case GIMP_PRECISION_U8_GAMMA:
              img->pixel_format = babl_format("R'G'B'A u8");
              break;
            case GIMP_PRECISION_U16_GAMMA:
              img->pixel_format = babl_format("R'G'B'A u16");
              break;
            case GIMP_PRECISION_U32_GAMMA:
              img->pixel_format = babl_format("R'G'B'A u32");
              break;
            default:
              return -1;
            }
        }
      else{
        g_error("Invalid combination of color mode and channels ");
        return -1;
      }
    }

  // GRAYSCALE
  if( img->gimp_base_type == GIMP_GRAY )
    {
      if( img->channels == 1 )
        {
          img->gimp_image_type = GIMP_GRAY_IMAGE;
          switch( img->gimp_precision )
            {
            case GIMP_PRECISION_U8_GAMMA:
              img->pixel_format = babl_format("Y' u8");
              break;
            case GIMP_PRECISION_U16_GAMMA:
              img->pixel_format = babl_format("Y' u16");
              break;
            case GIMP_PRECISION_U32_GAMMA:
              img->pixel_format = babl_format("Y' u32");
              break;
            default:
              return -1;
            }
        }
      else if( img->channels == 2 )
        {
          img->gimp_image_type = GIMP_GRAYA_IMAGE;
          img->has_alpha = TRUE;
          switch( img->gimp_precision )
            {
            case GIMP_PRECISION_U8_GAMMA:
              img->pixel_format = babl_format("Y'A u8");
              break;
            case GIMP_PRECISION_U16_GAMMA:
              img->pixel_format = babl_format("Y'A u16");
              break;
            case GIMP_PRECISION_U32_GAMMA:
              img->pixel_format = babl_format("Y'A u32");
              break;
            default:
              return -1;
            }
        }
      else{
        return -1;
      }
    }

  // INDEXED
  if( img->gimp_base_type == GIMP_INDEXED )
    {
      if( img->channels == 1 )
        {
          img->gimp_image_type = GIMP_GRAY_IMAGE;
          switch( img->gimp_precision )
            {
            case GIMP_PRECISION_U8_GAMMA:
              img->pixel_format = babl_format("Y' u8");
              break;
            case GIMP_PRECISION_U16_GAMMA:
              img->pixel_format = babl_format("Y' u16");
              break;
            case GIMP_PRECISION_U32_GAMMA:
              img->pixel_format = babl_format("Y' u32");
              break;
            default:
              return -1;
            }
        }
      else if( img->channels == 2 )
        {
          img->gimp_image_type = GIMP_GRAYA_IMAGE;
          img->has_alpha = TRUE;
          switch( img->gimp_precision )
            {
            case GIMP_PRECISION_U8_GAMMA:
              img->pixel_format = babl_format("Y'A u8");
              break;
            case GIMP_PRECISION_U16_GAMMA:
              img->pixel_format = babl_format("Y'A u16");
              break;
            case GIMP_PRECISION_U32_GAMMA:
              img->pixel_format = babl_format("Y'A u32");
              break;
            default:
              return -1;
            }
        }
      else{
        return -1;
      }
    }

  return 0;
}
