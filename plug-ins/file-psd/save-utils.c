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

#include "save-utils.h"

FILE* open_file_wb( const gchar* filename, GError** error )
{
  FILE* f;

  g_debug ("Create file %s", gimp_filename_to_utf8 (filename));

  f = g_fopen( filename, "wb" );
  if( f == NULL )
    {
      g_debug (" g_fopen() retrieval failed" );
      return NULL;
    }

  return f;
}

guint32 write1Byte( guchar c, FILE* f, GError **error )
{
  if( fwrite( &c, 1, 1, f ) == 0 )
    {
      return -1;
    }

  return 0;
}

guint32 write4charSignature( const gchar* sig, FILE* f, GError** error )
{
  size_t result = fwrite( sig, 4, 1, f);
  if( result != 4 )
    {
      return -1;
    }

  return 0;
}

guint32 write16bitInteger( gint16 value, FILE* f, GError** error )
{
  guchar b[2];

  b[1] = value & 255;
  b[0] = (value >> 8) & 255;

  if (fwrite (&b, 1, 2, f) == 0)
    {
      return -1;
    }

  return 0;
}

guint32 write32bitInteger( gint32 value, FILE* f, GError** error )
{
  guchar b[4];

  b[3] = value & 255;
  b[2] = (value >> 8) & 255;
  b[1] = (value >> 16) & 255;
  b[0] = (value >> 24) & 255;

  if (fwrite (&b, 1, 4, f) == 0)
    {
      return -1;
    }

  return 0;
}

const Babl* get_drawable_format( gint32 layer, gint32* channels )
{
  guint32 bpp, pixel;
  gboolean alpha;
  const Babl* format  = NULL;

  alpha= gimp_drawable_has_alpha( layer );
  bpp = gimp_drawable_bpp( layer );

  //RGB
  if( gimp_drawable_is_rgb( layer ) )
    {
      if( alpha )
        {
          *channels = 4;
          pixel = bpp / *channels;
          switch( pixel ){
          case 1:
            format = babl_format("R'G'B'A u8");
            break;
          case 2:
            format = babl_format("R'G'B'A u16");
            break;
          case 4:
            format = babl_format("R'G'B'A u32");
            break;
          }
        }
      else{
        *channels = 3;
        pixel = bpp / *channels;
        switch( pixel )
          {
          case 1:
            format = babl_format("R'G'B' u8");
            break;
          case 2:
            format = babl_format("R'G'B' u16");
            break;
          case 4:
            format = babl_format("R'G'B' u32");
            break;
          }
      }
    }

  //GRAY
  if( gimp_drawable_is_gray( layer ) )
    {
      if( alpha )
        {
          *channels = 2;
          pixel = bpp / *channels;
          switch( pixel ){
          case 1:
            format = babl_format("Y'A u8");
            break;
          case 2:
            format = babl_format("Y'A u16");
            break;
          case 4:
            format = babl_format("Y'A u32");
            break;
          }
        }
      else{
        *channels = 1;
        pixel = bpp / *channels;
        switch( pixel )
          {
          case 1:
            format = babl_format("Y' u8");
            break;
          case 2:
            format = babl_format("Y' u16");
            break;
          case 4:
            format = babl_format("Y' u32");
            break;
          }
      }
    }

  //INDEXED
  if( gimp_drawable_is_indexed( layer ) )
    {
      if( alpha )
        {
          *channels = 2;
          pixel = bpp / *channels;
          switch( pixel )
            {
            case 2:
              format = gimp_drawable_get_format(layer);
              break;
            }
        }
      else{
        *channels = 1;
        pixel = bpp / *channels;
        switch( pixel )
          {
          case 1:
            format = gimp_drawable_get_format(layer);
            break;
          }
      }
    }

  return format;
}

guint32 write_raw_imagedata( FILE* f, gint32 layer, GError** error )
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

  //Dump data to file
  fwrite( output_pixels, 1, length, f );

  return 0;
}

void get_psd_layer_blending_mode( gint32 layer, gchar* psdMode )
{

  switch (gimp_layer_get_mode (layer ) )
    {
    case GIMP_LAYER_MODE_NORMAL_LEGACY:
      strcpy (psdMode, "norm");
      break;
    case GIMP_LAYER_MODE_DARKEN_ONLY:
    case GIMP_LAYER_MODE_DARKEN_ONLY_LEGACY:
      strcpy (psdMode, "dark");
      break;
    case GIMP_LAYER_MODE_LIGHTEN_ONLY:
    case GIMP_LAYER_MODE_LIGHTEN_ONLY_LEGACY:
      strcpy (psdMode, "lite");
      break;
    case GIMP_LAYER_MODE_LCH_HUE:
    case GIMP_LAYER_MODE_HSV_HUE_LEGACY:
      strcpy (psdMode, "hue ");
      break;
    case GIMP_LAYER_MODE_LCH_CHROMA:
    case GIMP_LAYER_MODE_HSV_SATURATION_LEGACY:
      strcpy (psdMode, "sat ");
      break;
    case GIMP_LAYER_MODE_LCH_COLOR:
    case GIMP_LAYER_MODE_HSL_COLOR_LEGACY:
      strcpy (psdMode, "colr");
      break;
    case GIMP_LAYER_MODE_ADDITION:
    case GIMP_LAYER_MODE_ADDITION_LEGACY:
      strcpy (psdMode, "lddg");
      break;
    case GIMP_LAYER_MODE_MULTIPLY:
    case GIMP_LAYER_MODE_MULTIPLY_LEGACY:
      strcpy (psdMode, "mul ");
      break;
    case GIMP_LAYER_MODE_SCREEN:
    case GIMP_LAYER_MODE_SCREEN_LEGACY:
      strcpy (psdMode, "scrn");
      break;
    case GIMP_LAYER_MODE_DISSOLVE:
      strcpy (psdMode, "diss");
      break;
    case GIMP_LAYER_MODE_DIFFERENCE:
    case GIMP_LAYER_MODE_DIFFERENCE_LEGACY:
      strcpy (psdMode, "diff");
      break;
    case GIMP_LAYER_MODE_LCH_LIGHTNESS:
    case GIMP_LAYER_MODE_HSV_VALUE_LEGACY:                  /* ? */
      strcpy (psdMode, "lum ");
      break;
    case GIMP_LAYER_MODE_HARDLIGHT:
    case GIMP_LAYER_MODE_HARDLIGHT_LEGACY:
      strcpy (psdMode, "hLit");
      break;
    case GIMP_LAYER_MODE_OVERLAY_LEGACY:
    case GIMP_LAYER_MODE_SOFTLIGHT_LEGACY:
      strcpy (psdMode, "sLit");
      break;
    case GIMP_LAYER_MODE_OVERLAY:
      strcpy (psdMode, "over");
      break;
    case GIMP_LAYER_MODE_DODGE:
    case GIMP_LAYER_MODE_DODGE_LEGACY:
      strcpy (psdMode, "div");
      break;
    case GIMP_LAYER_MODE_EXCLUSION:
      strcpy (psdMode, "smud");
      break;
    case GIMP_LAYER_MODE_BURN:
    case GIMP_LAYER_MODE_BURN_LEGACY:
      strcpy (psdMode, "idiv");
      break;
    case GIMP_LAYER_MODE_LINEAR_BURN:
      strcpy (psdMode, "lbrn");
      break;
    case GIMP_LAYER_MODE_LINEAR_LIGHT:
      strcpy (psdMode, "lLit");
      break;
    case GIMP_LAYER_MODE_PIN_LIGHT:
      strcpy (psdMode, "pLit");
      break;
    case GIMP_LAYER_MODE_VIVID_LIGHT:
      strcpy (psdMode, "vLit");
      break;
    case GIMP_LAYER_MODE_HARD_MIX:
      strcpy (psdMode, "hMix");
      break;
      //Unsupported layer blending mode
    default:
      psdMode = NULL;
    }

}

gint32
write_pascal_string (const gchar  *src,
                      guint16       mod_len,
                      FILE         *f,
                      GError      **error)
{
  /*
   *  Converts utf-8 string to current locale and writes as pascal
   *  string with padding to a multiple of mod_len.
   */

  gchar  *str;
  gchar  *pascal_str;
  gchar   null_str = 0x0;
  guchar  pascal_len;
  gint32  bytes_written = 0;
  gsize   len;

  if (src == NULL)
    {
      /* Write null string as two null bytes (0x0) */
      if (fwrite (&null_str, 1, 1, f) < 1
          || fwrite (&null_str, 1, 1, f) < 1)
        {
          return -1;
        }
      bytes_written += 2;
    }
  else
    {
      str = g_locale_from_utf8 (src, -1, NULL, &len, NULL);
      if (len > 255)
        pascal_len = 255;
      else
        pascal_len = len;
      pascal_str = g_strndup (str, pascal_len);
      g_free (str);
      if (fwrite (&pascal_len, 1, 1, f) < 1
          || fwrite (pascal_str, pascal_len, 1, f) < 1)
        {
          g_free (pascal_str);
          return -1;
        }
      bytes_written++;
      bytes_written += pascal_len;
     g_debug ("Pascal string: %s, bytes_written: %d",
                        pascal_str, bytes_written);
      g_free (pascal_str);
    }

  /* Pad with nulls */
  if (mod_len > 0)
    {
      while (bytes_written % mod_len != 0)
        {
          if (fwrite (&null_str, 1, 1, f) < 1)
            {
              return -1;
            }
          bytes_written++;
        }
    }

  return bytes_written;
}
