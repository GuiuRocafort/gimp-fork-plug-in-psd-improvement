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

/*  Local function prototypes  */
#include "psd.h"

static void  query (void);
static void  run   (const gchar     *name,
                    gint             nparams,
                    const GimpParam *param,
                    gint            *nreturn_vals,
                    GimpParam      **return_vals);

static void setSuccessReturnValue( gint *nreturn_vals,
                            GimpParam **return_vals,
                            guint32 image_ID );

static void setExecutionErrorReturnValue( gint* nreturn_vals,
                                          GimpParam **return_vals,
                                          GError** error  );

static void setSuccessThumbnailReturnValue( gint *nreturn_vals,
                                            GimpParam **return_vals,
                                            guint32 image_ID,
                                            gint width,
                                            gint height );
/*  Local variables  */

GimpPlugInInfo PLUG_IN_INFO =
{
  NULL,  /* init_proc  */
  NULL,  /* quit_proc  */
  query, /* query_proc */
  run,   /* run_proc   */
};


MAIN ()

static void
query (void)
{
  /* File Load */
  static const GimpParamDef load_args[] =
  {
    { GIMP_PDB_INT32,  "run-mode",     "The run mode { RUN-INTERACTIVE (0), RUN-NONINTERACTIVE (1) }" },
    { GIMP_PDB_STRING, "filename",     "The name of the file to load" },
    { GIMP_PDB_STRING, "raw-filename", "The name of the file to load" }
  };

  static const GimpParamDef load_return_vals[] =
  {
    { GIMP_PDB_IMAGE, "image", "Output image" }
  };

  /* Thumbnail Load */
  static const GimpParamDef thumb_args[] =
  {
    { GIMP_PDB_STRING, "filename",     "The name of the file to load"  },
    { GIMP_PDB_INT32,  "thumb-size",   "Preferred thumbnail size"      }
  };

  static const GimpParamDef thumb_return_vals[] =
  {
    { GIMP_PDB_IMAGE,  "image",        "Thumbnail image"               },
    { GIMP_PDB_INT32,  "image-width",  "Width of full-sized image"     },
    { GIMP_PDB_INT32,  "image-height", "Height of full-sized image"    }
  };

  /* File save */
  static const GimpParamDef save_args[] =
  {
    { GIMP_PDB_INT32,    "run-mode",     "The run mode { RUN-INTERACTIVE (0), RUN-NONINTERACTIVE (1) }" },
    { GIMP_PDB_IMAGE,    "image",        "Input image" },
    { GIMP_PDB_DRAWABLE, "drawable",     "Drawable to save" },
    { GIMP_PDB_STRING,   "filename",     "The name of the file to save the image in" },
    { GIMP_PDB_STRING,   "raw-filename", "The name of the file to save the image in" },
    { GIMP_PDB_INT32,    "compression",  "Compression type: { NONE (0), LZW (1), PACKBITS (2)" },
    { GIMP_PDB_INT32,    "fill-order",   "Fill Order: { MSB to LSB (0), LSB to MSB (1)" }
  };

  /* File load */
  gimp_install_procedure (LOAD_PROC,
                          "Loads images from the Photoshop PSD file format",
                          "This plug-in loads images in Adobe "
                          "Photoshop (TM) native PSD format.",
                          "John Marshall",
                          "John Marshall",
                          "2007",
                          N_("Photoshop image"),
                          NULL,
                          GIMP_PLUGIN,
                          G_N_ELEMENTS (load_args),
                          G_N_ELEMENTS (load_return_vals),
                          load_args, load_return_vals);

  gimp_register_file_handler_mime (LOAD_PROC, "image/x-psd");
  gimp_register_magic_load_handler (LOAD_PROC,
                                    "psd",
                                    "",
                                    "0,string,8BPS");

  /* Thumbnail load */
  gimp_install_procedure (LOAD_THUMB_PROC,
                          "Loads thumbnails from the Photoshop PSD file format",
                          "This plug-in loads thumbnail images from Adobe "
                          "Photoshop (TM) native PSD format files.",
                          "John Marshall",
                          "John Marshall",
                          "2007",
                          NULL,
                          NULL,
                          GIMP_PLUGIN,
                          G_N_ELEMENTS (thumb_args),
                          G_N_ELEMENTS (thumb_return_vals),
                          thumb_args, thumb_return_vals);

  gimp_register_thumbnail_loader (LOAD_PROC, LOAD_THUMB_PROC);

  gimp_install_procedure (SAVE_PROC,
                          "saves files in the Photoshop(tm) PSD file format",
                          "This filter saves files of Adobe Photoshop(tm) native PSD format.  These files may be of any image type supported by GIMP, with or without layers, layer masks, aux channels and guides.",
                          "Monigotes",
                          "Monigotes",
                          "2000",
                          N_("Photoshop image"),
                          "RGB*, GRAY*, INDEXED*",
                          GIMP_PLUGIN,
                          G_N_ELEMENTS (save_args), 0,
                          save_args, NULL);

  gimp_register_file_handler_mime (SAVE_PROC, "image/x-psd");
  gimp_register_save_handler (SAVE_PROC, "psd", "");
}

static void
run (const gchar      *name,
     gint              nparams,
     const GimpParam  *param,
     gint             *nreturn_vals,
     GimpParam       **return_vals)
{
  GimpRunMode       run_mode;
  gint32            image_ID = -1;
  GError            *error = NULL;
  gboolean         resolution_loaded;
  gint width, height;

  run_mode = param[0].data.d_int32;

  INIT_I18N ();
  gegl_init (NULL, NULL);

  switch( run_mode){
  case GIMP_RUN_INTERACTIVE:
  case GIMP_RUN_WITH_LAST_VALS:
    gimp_ui_init( PLUG_IN_BINARY, FALSE );
    break;
  default:
    break;
  }

  //Load Procedure
  if (strcmp (name, LOAD_PROC) == 0)
    {
      image_ID = load_image( param[1].data.d_string, &resolution_loaded, &error );
      if(  image_ID != -1 )
        {
          setSuccessReturnValue( nreturn_vals, return_vals, image_ID );
        }
      else{
        g_debug("LOAD IMAGE RETURNED ERROR" );
        setExecutionErrorReturnValue( nreturn_vals, return_vals, &error );
      }
    }

  //Load thumbnail
  else if (strcmp (name, LOAD_THUMB_PROC) == 0)
    {
      image_ID = load_image_thumbnail( param[1].data.d_string, &width, &height, &error );
      if( image_ID != -1 )
        {
          setSuccessThumbnailReturnValue( nreturn_vals, return_vals, image_ID, width, height );
        }
      else{
        g_debug("LOAD THUMBNAIL RETURNED ERROR" );
        setExecutionErrorReturnValue( nreturn_vals, return_vals, &error );
      }
    }

  //Save File
  else if (strcmp (name, SAVE_PROC) == 0)
    {
      if( save_image( param[3].data.d_string, param[1].data.d_int32 , &error  ) != -1 )
        {
          setSuccessReturnValue( nreturn_vals, return_vals, image_ID );
        }
      else{
        g_debug("SAVE IMAGE RETURNED ERROR");
        setExecutionErrorReturnValue( nreturn_vals, return_vals, &error );
      }
    }

  //Unknown procedure
  else
    {
      //Call error
    }
}


static void
setSuccessReturnValue( gint *nreturn_vals, GimpParam **return_vals, guint32 image_ID )
{
  static GimpParam values[2];
  *nreturn_vals = 2;
  *return_vals = values;

  values[0].type = GIMP_PDB_STATUS;
  values[0].data.d_status = GIMP_PDB_SUCCESS;
  values[1].type = GIMP_PDB_IMAGE;
  values[1].data.d_image = image_ID;
}

static void
setSuccessThumbnailReturnValue( gint *nreturn_vals, GimpParam **return_vals, guint32 image_ID, gint width, gint height )
{
    static GimpParam values[4];
  *nreturn_vals = 4;
  *return_vals = values;

  values[0].type = GIMP_PDB_STATUS;
  values[0].data.d_status = GIMP_PDB_SUCCESS;
  values[1].type = GIMP_PDB_IMAGE;
  values[1].data.d_image = image_ID;
  values[2].type = GIMP_PDB_INT32;
  values[2].data.d_int32 = width;
  values[3].type = GIMP_PDB_INT32;
  values[3].data.d_int32 = height;

}

static void
setExecutionErrorReturnValue( gint* nreturn_vals, GimpParam **return_vals, GError** error  )
{
  static GimpParam values[2];
  *nreturn_vals = 2;
  *return_vals = values;

  values[0].type = GIMP_PDB_STATUS;
  values[0].data.d_status = GIMP_PDB_EXECUTION_ERROR;
  if( error )
    {
      values[1].type = GIMP_PDB_STRING;
      values[1].data.d_string =  "Execution ERROR ";
    }
}
