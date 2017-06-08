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

#ifndef __PSD_LOAD_IMG_RES__
#define __PSD_LOAD_IMG_RES__

#include "common.h"
#include "load-utils.h"

/* Image Resource IDs */
typedef enum {
  PSD_PS2_IMAGE_INFO    = 1000,         /* 0x03e8 - Obsolete - ps 2.0 image info */
  PSD_MAC_PRINT_INFO    = 1001,         /* 0x03e9 - Optional - Mac print manager print info record */
  PSD_PS2_COLOR_TAB     = 1003,         /* 0x03eb - Obsolete - ps 2.0 indexed color table */
  PSD_RESN_INFO         = 1005,         /* 0x03ed - ResolutionInfo structure */
  PSD_ALPHA_NAMES       = 1006,         /* 0x03ee - Alpha channel names */
  PSD_DISPLAY_INFO      = 1007,         /* 0x03ef - Superceded by PSD_DISPLAY_INFO_NEW for ps CS3 and higher - DisplayInfo structure */
  PSD_CAPTION           = 1008,         /* 0x03f0 - Optional - Caption string */
  PSD_BORDER_INFO       = 1009,         /* 0x03f1 - Border info */
  PSD_BACKGROUND_COL    = 1010,         /* 0x03f2 - Background color */
  PSD_PRINT_FLAGS       = 1011,         /* 0x03f3 - Print flags */
  PSD_GREY_HALFTONE     = 1012,         /* 0x03f4 - Greyscale and multichannel halftoning info */
  PSD_COLOR_HALFTONE    = 1013,         /* 0x03f5 - Color halftoning info */
  PSD_DUOTONE_HALFTONE  = 1014,         /* 0x03f6 - Duotone halftoning info */
  PSD_GREY_XFER         = 1015,         /* 0x03f7 - Greyscale and multichannel transfer functions */
  PSD_COLOR_XFER        = 1016,         /* 0x03f8 - Color transfer functions */
  PSD_DUOTONE_XFER      = 1017,         /* 0x03f9 - Duotone transfer functions */
  PSD_DUOTONE_INFO      = 1018,         /* 0x03fa - Duotone image information */
  PSD_EFFECTIVE_BW      = 1019,         /* 0x03fb - Effective black & white values for dot range */
  PSD_OBSOLETE_01       = 1020,         /* 0x03fc - Obsolete */
  PSD_EPS_OPT           = 1021,         /* 0x03fd - EPS options */
  PSD_QUICK_MASK        = 1022,         /* 0x03fe - Quick mask info */
  PSD_OBSOLETE_02       = 1023,         /* 0x03ff - Obsolete */
  PSD_LAYER_STATE       = 1024,         /* 0x0400 - Layer state info */
  PSD_WORKING_PATH      = 1025,         /* 0x0401 - Working path (not saved) */
  PSD_LAYER_GROUP       = 1026,         /* 0x0402 - Layers group info */
  PSD_OBSOLETE_03       = 1027,         /* 0x0403 - Obsolete */
  PSD_IPTC_NAA_DATA     = 1028,         /* 0x0404 - IPTC-NAA record (IMV4.pdf) */
  PSD_IMAGE_MODE_RAW    = 1029,         /* 0x0405 - Image mode for raw format files */
  PSD_JPEG_QUAL         = 1030,         /* 0x0406 - JPEG quality */
  PSD_GRID_GUIDE        = 1032,         /* 0x0408 - Grid & guide info */
  PSD_THUMB_RES         = 1033,         /* 0x0409 - Thumbnail resource */
  PSD_COPYRIGHT_FLG     = 1034,         /* 0x040a - Copyright flag */
  PSD_URL               = 1035,         /* 0x040b - URL string */
  PSD_THUMB_RES2        = 1036,         /* 0x040c - Thumbnail resource */
  PSD_GLOBAL_ANGLE      = 1037,         /* 0x040d - Superceded by PSD_NEW_COLOR_SAMPLER for ps CS3 and higher - Global angle */
  PSD_COLOR_SAMPLER     = 1038,         /* 0x040e - Superceded by PSD_NEW_COLOR_SAMPLER for ps CS3 and higher - Color samplers resource */
  PSD_ICC_PROFILE       = 1039,         /* 0x040f - ICC Profile */
  PSD_WATERMARK         = 1040,         /* 0x0410 - Watermark */
  PSD_ICC_UNTAGGED      = 1041,         /* 0x0411 - Do not use ICC profile flag */
  PSD_EFFECTS_VISIBLE   = 1042,         /* 0x0412 - Show / hide all effects layers */
  PSD_SPOT_HALFTONE     = 1043,         /* 0x0413 - Spot halftone */
  PSD_DOC_IDS           = 1044,         /* 0x0414 - Document specific IDs */
  PSD_ALPHA_NAMES_UNI   = 1045,         /* 0x0415 - Unicode alpha names */
  PSD_IDX_COL_TAB_CNT   = 1046,         /* 0x0416 - Indexed color table count */
  PSD_IDX_TRANSPARENT   = 1047,         /* 0x0417 - Index of transparent color (if any) */
  PSD_GLOBAL_ALT        = 1049,         /* 0x0419 - Global altitude */
  PSD_SLICES            = 1050,         /* 0x041a - Slices */
  PSD_WORKFLOW_URL_UNI  = 1051,         /* 0x041b - Workflow URL - Unicode string */
  PSD_JUMP_TO_XPEP      = 1052,         /* 0x041c - Jump to XPEP (?) */
  PSD_ALPHA_ID          = 1053,         /* 0x041d - Alpha IDs */
  PSD_URL_LIST_UNI      = 1054,         /* 0x041e - URL list - unicode */
  PSD_VERSION_INFO      = 1057,         /* 0x0421 - Version info */
  PSD_EXIF_DATA         = 1058,         /* 0x0422 - Exif data block 1 */
  PSD_EXIF_DATA_3       = 1059,         /* 0X0423 - Exif data block 3 (?) */
  PSD_XMP_DATA          = 1060,         /* 0x0424 - XMP data block */
  PSD_CAPTION_DIGEST    = 1061,         /* 0x0425 - Caption digest */
  PSD_PRINT_SCALE       = 1062,         /* 0x0426 - Print scale */
  PSD_PIXEL_AR          = 1064,         /* 0x0428 - Pixel aspect ratio */
  PSD_LAYER_COMPS       = 1065,         /* 0x0429 - Layer comps */
  PSD_ALT_DUOTONE_COLOR = 1066,         /* 0x042A - Alternative Duotone colors */
  PSD_ALT_SPOT_COLOR    = 1067,         /* 0x042B - Alternative Spot colors */
  PSD_LAYER_SELECT_ID   = 1069,         /* 0x042D - Layer selection ID */
  PSD_HDR_TONING_INFO   = 1070,         /* 0x042E - HDR toning information */
  PSD_PRINT_INFO_SCALE  = 1071,         /* 0x042F - Print scale */
  PSD_LAYER_GROUP_E_ID  = 1072,         /* 0x0430 - Layer group(s) enabled ID */
  PSD_COLOR_SAMPLER_NEW = 1073,         /* 0x0431 - Color sampler resource for ps CS3 and higher PSD files */
  PSD_MEASURE_SCALE     = 1074,         /* 0x0432 - Measurement scale */
  PSD_TIMELINE_INFO     = 1075,         /* 0x0433 - Timeline information */
  PSD_SHEET_DISCLOSE    = 1076,         /* 0x0434 - Sheet discloser */
  PSD_DISPLAY_INFO_NEW  = 1077,         /* 0x0435 - DisplayInfo structure for ps CS3 and higher PSD files */
  PSD_ONION_SKINS       = 1078,         /* 0x0436 - Onion skins */
  PSD_COUNT_INFO        = 1080,         /* 0x0438 - Count information*/
  PSD_PRINT_INFO        = 1082,         /* 0x043A - Print information added in ps CS5*/
  PSD_PRINT_STYLE       = 1083,         /* 0x043B - Print style */
  PSD_MAC_NSPRINTINFO   = 1084,         /* 0x043C - Mac NSPrintInfo*/
  PSD_WIN_DEVMODE       = 1085,         /* 0x043D - Windows DEVMODE */
  PSD_AUTO_SAVE_PATH    = 1086,         /* 0x043E - Auto save file path */
  PSD_AUTO_SAVE_FORMAT  = 1087,         /* 0x043F - Auto save format */
  PSD_PATH_INFO_FIRST   = 2000,         /* 0x07d0 - First path info block */
  PSD_PATH_INFO_LAST    = 2998,         /* 0x0bb6 - Last path info block */
  PSD_CLIPPING_PATH     = 2999,         /* 0x0bb7 - Name of clipping path */
  PSD_PLUGIN_R_FIRST    = 4000,         /* 0x0FA0 - First plugin resource */
  PSD_PLUGIN_R_LAST     = 4999,         /* 0x1387 - Last plugin resource */
  PSD_IMAGEREADY_VARS   = 7000,         /* 0x1B58 - Imageready variables */
  PSD_IMAGEREADY_DATA   = 7001,         /* 0x1B59 - Imageready data sets */
  PSD_LIGHTROOM_WORK    = 8000,         /* 0x1F40 - Lightroom workflow */
  PSD_PRINT_FLAGS_2     = 10000         /* 0x2710 - Print flags */
} PSDImageResID;

guint32 read_img_res( FILE* f, GError **error );
guint32 read_resource( FILE* f, GError **error );

#endif
