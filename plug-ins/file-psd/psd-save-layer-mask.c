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

#include "psd-save-layer-mask.h"

guint32 save_layer_mask(FILE* f, gint32* layers, gint32 nLayers, GError **error  )
{
  long int length_position = ftell(f);

  //Write 0 length for now
  write32bitInteger( 0, f, error );

  /* Layer info */
  if( save_layer_info( f, layers, nLayers, error ) == -1 )
    return -1;

  return 0;
}


guint32 save_layer_info( FILE* f, gint32* layers, gint32 nLayers, GError **error )
{
  long int length_position = ftell(f);
  gint i;

  //Write 0 layer info length
  write32bitInteger( 0, f, error );

  //Layer count
  write16bitInteger( nLayers, f, error );

  //Layers are saved in inverse order
  for( i=nLayers;  i>0; i-- )
    {
      //Write Layer records
      save_layer_records( f, layers[i], error );

      //Write Channel image data
    }

  return 0;
}

guint32 save_layer_records( FILE* f , gint32 layer, GError **error )
{
  long int extra_data_length;
  gint x,y, width, height;
  gint32 channels;
  gchar* blend_mode = NULL, *layerName;
  guchar opacity, flags;
  gint i;

  height = gimp_drawable_height( layer );
  width = gimp_drawable_width( layer );
  gimp_drawable_offsets( layer, &x, &y );

  //Top, left, bottom, right
  write32bitInteger( y, f, error );
  write32bitInteger( x, f, error );
  write32bitInteger( width, f, error );
  write32bitInteger( height, f, error );

  //Number of channels
  get_drawable_format( layer, &channels );

  //TODO
  //Channel information
  for( i=0; i<channels; i++ )
    {
      //Channel ID
      write16bitInteger( 0, f, error );
      //Channel data length
      write32bitInteger( 0, f, error );
    }

  //Blend mode signature
  write4charSignature( "8BIM", f, error );

  //Blend mode key ( get blend modes )
  get_psd_layer_blending_mode( layer, blend_mode );
  if( blend_mode == NULL )
    {
      return -1;
    }
  write4charSignature( blend_mode, f, error );

  //Opacity 1 byte
  opacity = gimp_layer_get_opacity( layer ) / 100.0;
  write1Byte( opacity, f, error );

  //Clipping 1 byte ( not used in GIMP )
  write1Byte( '0', f, error );

  //Flags 1 byte
  flags = 0;
  if (gimp_layer_get_lock_alpha( layer ) ) flags |= 1;
  if (! gimp_item_get_visible ( layer ) ) flags |= 2;

  //Filler
  write1Byte( '0', f, error );

  //Length of extra data field
  extra_data_length = ftell(f);

  //Layer mask data
  if( save_layer_mask_data( f, layer, error ) == -1 ) return -1;

  //Layer blending ranges
  write32bitInteger( 0, f, error );

  /* //Layer name */
  /* layerName = gimp_item_get_name( layer ); */
  /* write_pascal_string( layerName, mode_len, f, error ); */

  return 0;
}


guint32 save_layer_mask_data( FILE* f, gint32 layer, GError **error )
{
  long int length_position = ftell(f);
  gint32 top, left, right, bottom;
  gint mask;
  gboolean apply;
  gchar* name;

  //Size of layer mask data
  write32bitInteger( 0, f, error );

  mask = gimp_layer_get_mask( layer );
  if( mask >= 0 )
    {
      g_debug("\t\t Layer mask size: 20" );
      //Layer mask size
      write32bitInteger( 20, f, error );
      //Top
      write32bitInteger( 0, f, error );
      //Left
      write32bitInteger( 0, f, error );
      //Bottom
      write32bitInteger( gimp_drawable_height(mask), f, error );
      //Right
      write32bitInteger( gimp_drawable_width(mask), f, error );
      //Default color
      write1Byte( '0', f, error  );
      //Flags TODO
      write1Byte('0', f, error );
      //Padding
      write16bitInteger( 0, f, error );
    }
  else{
    //WRITTING empty layer mask
    write32bitInteger( 0, f, error );
  }

  return 0;
}
