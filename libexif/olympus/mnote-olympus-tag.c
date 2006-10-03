/* mnote-olympus-tag.c:
 *
 * Copyright © 2002 Lutz Müller <lutz@users.sourceforge.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details. 
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <config.h>
#include "mnote-olympus-tag.h"

#include <libexif/i18n.h>
#include <libexif/exif-utils.h>

#include <stdlib.h>

static struct {
	MnoteOlympusTag tag;
	const char *name;
	const char *title;
	const char *description;
} table[] = {

	/* Nikon v2 */
	{MNOTE_NIKON_TAG_FIRMWARE,     "Firmware", N_("Firmware Version"), NULL},
	{MNOTE_NIKON_TAG_ISO,          "ISO", N_("ISO Setting"), NULL},
	{MNOTE_NIKON_TAG_COLORMODE1,   "COLORMODE1", N_("Colormode (?)"), NULL},
	{MNOTE_NIKON_TAG_QUALITY,      "QUALITY", N_("Quality"), NULL},
	{MNOTE_NIKON_TAG_WHITEBALANCE, "WHITEBALANCE", N_("Whitebalance"), NULL},
	{MNOTE_NIKON_TAG_SHARPENING,   "SHARPENING",   N_("Image Sharpening"), NULL},
	{MNOTE_NIKON_TAG_FOCUSMODE,    "FOCUSMODE",   N_("Focus Mode"), NULL},
	{MNOTE_NIKON_TAG_FLASHSETTING, "FLASHSETTING",   N_("Flash Setting"), NULL},
	{MNOTE_NIKON_TAG_FLASHMODE,    "FLASHMODE",    N_("Flash Mode"), NULL},
	{MNOTE_NIKON_TAG_WHITEBALANCEFINE,"WHITEBALANCEFINE",N_("Whitebalance fine ajustment"), NULL},
	{MNOTE_NIKON_TAG_WHITEBALANCERB,  "WHITEBALANCERB", N_("Whitebalance RB"), NULL},
	{MNOTE_NIKON_TAG_UNKNOWN_0X000D,  NULL, NULL, NULL},
	{MNOTE_NIKON_TAG_ISOSELECTION,    "ISOSELECTION", N_("Isoselection"), NULL},
	{MNOTE_NIKON_TAG_PREVIEWIMAGE,    "PREVIEWIMAGE", N_("Preview Image"), NULL},
	{MNOTE_NIKON_TAG_EXPOSUREDIFF,    "EXPOSUREDIFF", N_("Exposurediff ?"), NULL},
	{MNOTE_NIKON_TAG_FLASHEXPCOMPENSATION, "FLASHEXPCOMPENSATION", N_("Flash exposure compensation"), NULL},
	{MNOTE_NIKON_TAG_ISO2,            "ISO", N_("ISO Setting"), NULL},
	{MNOTE_NIKON_TAG_IMAGEBOUNDARY,   "IMAGEBOUNDARY", N_("Image Boundary"), NULL},
	{MNOTE_NIKON_TAG_UNKNOWN_0X0017,  NULL, NULL, NULL},
	{MNOTE_NIKON_TAG_FLASHEXPOSUREBRACKETVAL,  "FLASHEXPOSUREBRACKETVAL", N_("Flash exposure bracket value"), NULL},
	{MNOTE_NIKON_TAG_EXPOSUREBRACKETVAL,  "EXPOSUREBRACKETVAL", N_("Exposure bracket value"), NULL},
	{MNOTE_NIKON_TAG_IMAGEADJUSTMENT, "ImageAdjustment", N_("Image Adjustment"), NULL},
	{MNOTE_NIKON_TAG_TONECOMPENSATION, "TONECOMPENSATION", N_("Tonecompensation"), NULL},
	{MNOTE_NIKON_TAG_ADAPTER,         "ADAPTER", N_("Adapter"), NULL},
	{MNOTE_NIKON_TAG_LENSTYPE,        "LENSTYPE", N_("Lenstype"), NULL},
	{MNOTE_NIKON_TAG_LENS,            "LENS", N_("Lens"), NULL},
	{MNOTE_NIKON_TAG_MANUALFOCUSDISTANCE, "MANUALFOCUSDISTANCE", N_("Manual Focus Distance"), NULL},
	{MNOTE_NIKON_TAG_DIGITALZOOM,     "DigitalZoom", N_("Digital Zoom"), NULL},
	{MNOTE_NIKON_TAG_FLASHUSED,       "FLASHUSED", N_("Flash used"), NULL},
	{MNOTE_NIKON_TAG_AFFOCUSPOSITION, "AFFOCUSPOSITION", N_("AF Focus position"), NULL},
	{MNOTE_NIKON_TAG_BRACKETING,      "BRACKETING", N_("Bracketing"), NULL},
	{MNOTE_NIKON_TAG_UNKNOWN_0X008A,  NULL, NULL, NULL},
	{MNOTE_NIKON_TAG_LENS_FSTOPS,     "LENSFSTOPS", N_("Lens F stops"), NULL},
	{MNOTE_NIKON_TAG_CURVE,           "CURVE,", N_("Contrast curve"), NULL},
	{MNOTE_NIKON_TAG_COLORMODE,       "COLORMODE,", N_("Colormode"), NULL},
	{MNOTE_NIKON_TAG_LIGHTTYPE,       "LIGHTTYPE,", N_("Lighttype"), NULL},
	{MNOTE_NIKON_TAG_UNKNOWN_0X0091,  NULL, NULL, NULL},
	{MNOTE_NIKON_TAG_HUE,             "HUE", N_("Hue Adjustment"), NULL},
	{MNOTE_NIKON_TAG_SATURATION,      "SATURATION", N_("Saturation"), NULL},
	{MNOTE_NIKON_TAG_NOISEREDUCTION,  "NOISEREDUCTION,", N_("Noisereduction"), NULL},
	{MNOTE_NIKON_TAG_UNKNOWN_0X0097,  NULL, NULL, NULL},
	{MNOTE_NIKON_TAG_UNKNOWN_0X0098,  NULL, NULL, NULL},
	{MNOTE_NIKON_TAG_SENSORPIXELSIZE, "SENSORPIXELSIZE", N_("Sensor pixel size"), NULL},
	{MNOTE_NIKON_TAG_UNKNOWN_0X009B,  NULL, NULL, NULL},
	{MNOTE_NIKON_TAG_SERIALNUMBER,    "SERIALNUMBER", N_("Serial number"), NULL},
	{MNOTE_NIKON_TAG_IMAGE_DATASIZE,  "IMAGEDATASIZE", N_("Image datasize"), NULL},
	{MNOTE_NIKON_TAG_UNKNOWN_0X00A3,  NULL, NULL, NULL},
	{MNOTE_NIKON_TAG_TOTALPICTURES,   "TOTALPICTURES,", N_("Total number of pictures taken"), NULL},
	{MNOTE_NIKON_TAG_UNKNOWN_0X00A8,  NULL, NULL, NULL},
	{MNOTE_NIKON_TAG_OPTIMIZATION,    "OPTIMIZATION,", N_("Optimize Image"), NULL},
	{MNOTE_NIKON_TAG_SATURATION,      "SATURATION", N_("Saturation"), NULL},
	{MNOTE_NIKON_TAG_VARIPROGRAM,     "VARIPROGRAM", N_("Vari Program"), NULL},
	{MNOTE_NIKON_TAG_CAPTUREEDITORDATA, "CAPTUREEDITORDATA", N_("Capture Editor Data"), NULL},
	{MNOTE_NIKON_TAG_CAPTUREEDITORVER, "CAPTUREEDITORVER", N_("Capture Editor Version"), NULL},
	{MNOTE_NIKON_TAG_UNKNOWN_0X0E0E,  NULL, NULL, NULL},
	{MNOTE_NIKON_TAG_UNKNOWN_0X0E10,  NULL, NULL, NULL},
	{MNOTE_NIKON1_TAG_UNKNOWN_0X0002, NULL, NULL, NULL},
	{MNOTE_NIKON1_TAG_QUALITY,        "QUALITY", N_("Quality"), NULL},
	{MNOTE_NIKON1_TAG_COLORMODE,      "COLORMODE,", N_("Colormode"), NULL},
	{MNOTE_NIKON1_TAG_IMAGEADJUSTMENT, "ImageAdjustment", N_("Image Adjustment"), NULL},
	{MNOTE_NIKON1_TAG_CCDSENSITIVITY, "CCDSensitivity", N_("CCD Sensitivity"), NULL},
	{MNOTE_NIKON1_TAG_WHITEBALANCE,   "WhiteBalance", N_("Whitebalance"), NULL},
	{MNOTE_NIKON1_TAG_FOCUS,          "Focus", N_("Focus"), NULL},
	{MNOTE_NIKON1_TAG_UNKNOWN_0X0009, NULL, NULL, NULL},
	{MNOTE_NIKON1_TAG_DIGITALZOOM,    "DigitalZoom", N_("Digital Zoom"), NULL},
	{MNOTE_NIKON1_TAG_CONVERTER,      "Converter", N_("Converter"), NULL},

	/* Olympus */
	{MNOTE_OLYMPUS_TAG_MODE, "Mode", N_("Speed/Sequence/Panorama direction"), NULL},
	{MNOTE_OLYMPUS_TAG_QUALITY, "Quality", N_("Quality"), NULL},
	{MNOTE_OLYMPUS_TAG_MACRO, "Macro", N_("Macro"), NULL},
	{MNOTE_OLYMPUS_TAG_UNKNOWN_1, NULL, NULL, NULL},
	{MNOTE_OLYMPUS_TAG_DIGIZOOM, "DigiZoom", N_("Digital Zoom"), NULL},
	{MNOTE_OLYMPUS_TAG_UNKNOWN_2, NULL, NULL, NULL},
	{MNOTE_OLYMPUS_TAG_UNKNOWN_3, NULL, NULL, NULL},
	{MNOTE_OLYMPUS_TAG_VERSION, "FirmwareVersion", N_("Firmware version"), NULL},
	{MNOTE_OLYMPUS_TAG_INFO, "Info", N_("Info"), NULL},
	{MNOTE_OLYMPUS_TAG_ID, "CameraID", N_("Camera ID"), NULL},
	{MNOTE_OLYMPUS_TAG_UNKNOWN_4, NULL, NULL, NULL},
	{MNOTE_OLYMPUS_TAG_FLASHMODE, "FlashMode", N_("Flash Mode"), NULL},
	{MNOTE_OLYMPUS_TAG_FOCUSDIST, "ManualFocusDistance", N_("Manual Focus Distance"), NULL},
	{MNOTE_OLYMPUS_TAG_SHARPNESS, "Sharpness", N_("Sharpness Setting"), NULL},
	{MNOTE_OLYMPUS_TAG_WBALANCE, "WhiteBalance", N_("White Balance Setting"), NULL},
	{MNOTE_OLYMPUS_TAG_CONTRAST, "Contrast", N_("Contrast Setting"), NULL},
	{MNOTE_OLYMPUS_TAG_MANFOCUS, "ManualFocus", N_("Manual Focus"), NULL},
	{0, NULL, NULL, NULL}
};

const char *
mnote_olympus_tag_get_name (MnoteOlympusTag t)
{
	unsigned int i;

	for (i = 0; i < sizeof (table) / sizeof (table[0]); i++)
		if (table[i].tag == t) return (table[i].name);
	return NULL;
}

const char *
mnote_olympus_tag_get_title (MnoteOlympusTag t)
{
	unsigned int i;

	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	for (i = 0; i < sizeof (table) / sizeof (table[0]); i++)
		if (table[i].tag == t) return (_(table[i].title));
	return NULL;
}

const char *
mnote_olympus_tag_get_description (MnoteOlympusTag t)
{
	unsigned int i;

	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	for (i = 0; i < sizeof (table) / sizeof (table[0]); i++)
		if (table[i].tag == t) return (_(table[i].description));
	return NULL;
}
