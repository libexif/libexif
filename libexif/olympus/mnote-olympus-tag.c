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

#include <stdlib.h>

static struct {
	MnoteOlympusTag tag;
	const char *name;
	const char *title;
	const char *description;
} table[] = {

	/* Nikon */
        {MNOTE_NIKON_TAG_FIRMWARE, "Firmware", N_("Firmware Version"), ""},
        {MNOTE_NIKON_TAG_ISO,          "ISO", N_("ISO Setting"), ""},
        {MNOTE_NIKON_TAG_COLORMODE1,      "COLORMODE1", N_("Colormode (?)"), ""},
        {MNOTE_NIKON_TAG_QUALITY,      "QUALITY", N_("Quality"), ""},
        {MNOTE_NIKON_TAG_WHITEBALANCE, "WHITEBALANCE", N_("Whitebalance"), ""},
        {MNOTE_NIKON_TAG_SHARPENING,   "SHARPENING",   N_("Image Sharpening"), ""},
        {MNOTE_NIKON_TAG_FOCUSMODE,    "FOCUSMODE",   N_("Focus Mode"), ""},
        {MNOTE_NIKON_TAG_FLASHSETTING, "FLASHSETTING",   N_("Flash Setting"), ""},
        {MNOTE_NIKON_TAG_FLASHMODE,    "FLASHMODE",    N_("Flash Mode"), ""},
        {MNOTE_NIKON_TAG_WHITEBALANCEFINE,"WHITEBALANCEFINE",N_("Whitebalance fine ajustment"), ""},
        {MNOTE_NIKON_TAG_WHITEBALANCERB,  "WHITEBALANCERB", N_("Whitebalance RB"), ""},
        {MNOTE_NIKON_TAG_UNKNOWN_0X000D,  "UNKNOWN_0X000D", N_("Unknown tag 0x000d"), ""},
        {MNOTE_NIKON_TAG_ISOSELECTION,  "ISOSELECTION", N_("Isoselection"), ""},        {MNOTE_NIKON_TAG_UNKNOWN_0X0011,  "UNKNOWN_0X0011", N_("Unknown tag 0x0011"), ""},
        {MNOTE_NIKON_TAG_EXPOSUREDIFF,  "EXPOSUREDIFF", N_("Exposurediff ?"), ""},
        {MNOTE_NIKON_TAG_FLASHCOMPENSATION,     "FLASHCOMPENSATION", N_("Flashcompensation ?"), ""},
        {MNOTE_NIKON_TAG_ISO2,          "ISO", N_("ISO Setting"), ""},
        {MNOTE_NIKON_TAG_UNKNOWN_0X0016,  "UNKNOWN_0X0016", N_("Unknown tag 0x0016"), ""},
        {MNOTE_NIKON_TAG_UNKNOWN_0X0017,  "UNKNOWN_0X0017", N_("Unknown tag 0x0017"), ""},
        {MNOTE_NIKON_TAG_UNKNOWN_0X0018,  "UNKNOWN_0X0018", N_("Unknown tag 0x0018"), ""},
        {MNOTE_NIKON_TAG_UNKNOWN_0X0019,  "UNKNOWN_0X0019", N_("Unknown tag 0x0019"), ""},
        {MNOTE_NIKON_TAG_TONECOMPENSATION,  "TONECOMPENSATION", N_("Tonecompensation"), ""},
        {MNOTE_NIKON_TAG_LENSTYPE,          "LENSTYPE", N_("Lenstype"), ""},
        {MNOTE_NIKON_TAG_LENS,          "LENS", N_("Lens"), ""},                                                                                 
        {MNOTE_NIKON_TAG_MANUALFOCUSDISTANCE,   "MANUALFOCUSDISTANCE", N_("Manual Focus Distance"), ""},
        {MNOTE_NIKON_TAG_FLASHUSED,          "FLASHUSED", N_("Flash used"), ""},                                                                                
        {MNOTE_NIKON_TAG_AFFOCUSPOSITION,  "AFFOCUSPOSITION", N_("AF Focus position"), ""},
        {MNOTE_NIKON_TAG_BRACKETING,          "BRACKETING", N_("Bracketing"), ""},
        {MNOTE_NIKON_TAG_UNKNOWN_0X008A,  "UNKNOWN_0X008A", N_("Unknown tag 0x008a"), ""},
        {MNOTE_NIKON_TAG_UNKNOWN_0X008B,  "UNKNOWN_0X008B", N_("Unknown tag 0x008b"), ""},
        {MNOTE_NIKON_TAG_CURVE,          "CURVE,", N_("Contrast curve"), ""},                                                                                 
        {MNOTE_NIKON_TAG_COLORMODE,          "COLORMODE,", N_("Colormode"), ""},                                                                                
        {MNOTE_NIKON_TAG_LIGHTYPE,          "LIGHTYPE,", N_("Lightype"), ""},
        {MNOTE_NIKON_TAG_UNKNOWN_0X0091,  "UNKNOWN_0X0091", N_("Unknown tag 0x0091"), ""},
        {MNOTE_NIKON_TAG_HUE,          "Hue,", N_("Hue Adjustment"), ""}, 
        {MNOTE_NIKON_TAG_NOISEREDUCTION ,  "NOISEREDUCTION,", N_("Noisereduction"), ""},
        {MNOTE_NIKON_TAG_UNKNOWN_0X0097,  "UNKNOWN_0X0097", N_("Unknown tag 0x0097"), ""},
        {MNOTE_NIKON_TAG_UNKNOWN_0X0098,  "UNKNOWN_0X0098", N_("Unknown tag 0x0098"), ""},
        {MNOTE_NIKON_TAG_UNKNOWN_0X009A,  "UNKNOWN_0X009A", N_("Unknown tag 0x009a"), ""},
        {MNOTE_NIKON_TAG_UNKNOWN_0X00A0,  "UNKNOWN_0X00A0", N_("Unknown tag 0x00a0"), ""},
        {MNOTE_NIKON_TAG_UNKNOWN_0X00A2,  "UNKNOWN_0X00A2", N_("Unknown tag 0x00a2"), ""},
        {MNOTE_NIKON_TAG_UNKNOWN_0X00A3,  "UNKNOWN_0X00A3", N_("Unknown tag 0x00a3"), ""},
        {MNOTE_NIKON_TAG_TOTALPICTURES  ,  "TOTALPICTURES,", N_("Total number of pictures taken"), ""},
        {MNOTE_NIKON_TAG_UNKNOWN_0X00A8,  "UNKNOWN_0X00A8", N_("Unknown tag 0x00a8"), ""},
        {MNOTE_NIKON_TAG_OPTIMIZATION   ,  "OPTIMIZATION,", N_("Optimize Image"), ""},
        {MNOTE_NIKON_TAG_UNKNOWN_0X00AA,  "UNKNOWN_0X00AA", N_("Unknown tag 0x00aa"), ""},
        {MNOTE_NIKON_TAG_UNKNOWN_0X00AB,  "UNKNOWN_0X00AB", N_("Unknown tag 0x00ab"), ""},
        {MNOTE_NIKON_TAG_CAPTUREEDITORDATA,  "CAPTUREEDITORDATA,", N_("Capture Editor Data"), ""},

	/* Olympus */
	{MNOTE_OLYMPUS_TAG_MODE, "Mode", N_("Speed/Sequence/Panorama direction"), ""},
	{MNOTE_OLYMPUS_TAG_QUALITY, "Quality", N_("Quality"), ""},
	{MNOTE_OLYMPUS_TAG_MACRO, "Macro", N_("Macro"), ""},
	{MNOTE_OLYMPUS_TAG_UNKNOWN_1, "Unknown1", N_("Unknown 1"), ""},
	{MNOTE_OLYMPUS_TAG_DIGIZOOM, "DigiZoom", N_("Digital Zoom"), ""},
	{MNOTE_OLYMPUS_TAG_UNKNOWN_2, "Unknown2", N_("Unknown 2"), ""},
	{MNOTE_OLYMPUS_TAG_UNKNOWN_3, "Unknown3", N_("Unknown 3"), ""},
	{MNOTE_OLYMPUS_TAG_VERSION, "FirmwareVersion", N_("Firmware version"), ""},
	{MNOTE_OLYMPUS_TAG_INFO, "Info", N_("Info"), ""},
	{MNOTE_OLYMPUS_TAG_ID, "CameraID", N_("Camera ID"), ""},
	{MNOTE_OLYMPUS_TAG_UNKNOWN_4, "Unknown4", N_("Unknown 4"), ""},
	{MNOTE_OLYMPUS_TAG_FLASHMODE, "FlashMode", N_("Flash Mode"), ""},
	{MNOTE_OLYMPUS_TAG_FOCUSDIST, "ManualFocusDistance", N_("Manual Focus Distance"), ""},
	{MNOTE_OLYMPUS_TAG_SHARPNESS, "Sharpness", N_("Sharpness Setting"), ""},
	{MNOTE_OLYMPUS_TAG_WBALANCE, "WhiteBalance", N_("White Balance Setting"), ""},
	{MNOTE_OLYMPUS_TAG_CONTRAST, "Contrast", N_("Contrast Setting"), ""},
	{MNOTE_OLYMPUS_TAG_MANFOCUS, "ManualFocus", N_("Manual Focus"), ""},
	{0, NULL, NULL, NULL}
};

const char *
mnote_olympus_tag_get_name (MnoteOlympusTag t)
{
	unsigned int i;

	for (i = 0; table[i].name; i++) if (table[i].tag == t) break;
	return (table[i].name);
}

const char *
mnote_olympus_tag_get_title (MnoteOlympusTag t)
{
	unsigned int i;

	bindtextdomain (GETTEXT_PACKAGE, LIBMNOTE_LOCALEDIR);
	for (i = 0; table[i].title; i++) if (table[i].tag == t) break;
	return (_(table[i].title));
}

const char *
mnote_olympus_tag_get_description (MnoteOlympusTag t)
{
	unsigned int i;

	bindtextdomain (GETTEXT_PACKAGE, LIBMNOTE_LOCALEDIR);
	for (i = 0; table[i].description; i++) if (table[i].tag == t) break;
	return (_(table[i].description));
}
