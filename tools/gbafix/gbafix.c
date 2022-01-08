/*
	"$Id: gbafix.c,v 1.2 2008-07-30 17:12:51 wntrmute Exp $"

	DevkitPro GBA ROM fix utility

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Library General Public
	License as published by the Free Software Foundation; either
	version 2 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Library General Public License for more details.

	You should have received a copy of the GNU Library General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
	USA.

	Please report all bugs and problems through the bug tracker at
	"http://sourceforge.net/tracker/?group_id=114505&atid=668551".

	"$Header: /lvm/shared/ds/ds/cvs/devkitpro-cvsbackup/tools/gba/gbatools/gbafix.c,v 1.2 2008-07-30 17:12:51 wntrmute Exp $"

*/
//---------------------------------------------------------------------------------
// gbafix.c
//---------------------------------------------------------------------------------
/*
	Gameboy Advance ROM fixer (by Dark Fader / BlackThunder / WinterMute / Diegoisawesome)
	Validates header of GBA roms.

	History
	-------
	v1.05 - added debug offset argument, (Diegoisawesome)
	v1.04 - converted to plain C, (WinterMute)
	v1.03 - header.fixed, header.device_type
	v1.02 - redefined the options (rgbfix style), checksum=0
	v1.01 - fix in parameters
	v1.00 - logo, complement
*/

#pragma pack(1)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define VER		"1.05"
#define ARGV	argv[arg]
#define VALUE	(ARGV+2)
#define NUMBER	strtoul(VALUE, NULL, 0)

typedef struct
{
	uint32_t	start_code;			// B instruction
	uint8_t		logo[0xA0-0x04];	// logo data
	uint8_t		title[0xC];			// game title name
	uint32_t	game_code;			//
	uint16_t	maker_code;			//
	uint8_t		fixed;				// 0x96
	uint8_t		unit_code;			// 0x00
	uint8_t		device_type;		// 0x00
	uint8_t		unused[7];			//
	uint8_t		game_version;		// 0x00
	uint8_t		complement;			// 800000A0..800000BC
	uint16_t	checksum;			// 0x0000
} Header;


Header header;

unsigned short checksum_without_header = 0;

const Header good_header =
{
	// start_code
	0xEA00002E,
	// logo
	{ 0x24,0xFF,0xAE,0x51,0x69,0x9A,0xA2,0x21,0x3D,0x84,0x82,0x0A,0x84,0xE4,0x09,0xAD,
	0x11,0x24,0x8B,0x98,0xC0,0x81,0x7F,0x21,0xA3,0x52,0xBE,0x19,0x93,0x09,0xCE,0x20,
	0x10,0x46,0x4A,0x4A,0xF8,0x27,0x31,0xEC,0x58,0xC7,0xE8,0x33,0x82,0xE3,0xCE,0xBF,
	0x85,0xF4,0xDF,0x94,0xCE,0x4B,0x09,0xC1,0x94,0x56,0x8A,0xC0,0x13,0x72,0xA7,0xFC,
	0x9F,0x84,0x4D,0x73,0xA3,0xCA,0x9A,0x61,0x58,0x97,0xA3,0x27,0xFC,0x03,0x98,0x76,
	0x23,0x1D,0xC7,0x61,0x03,0x04,0xAE,0x56,0xBF,0x38,0x84,0x00,0x40,0xA7,0x0E,0xFD,
	0xFF,0x52,0xFE,0x03,0x6F,0x95,0x30,0xF1,0x97,0xFB,0xC0,0x85,0x60,0xD6,0x80,0x25,
	0xA9,0x63,0xBE,0x03,0x01,0x4E,0x38,0xE2,0xF9,0xA2,0x34,0xFF,0xBB,0x3E,0x03,0x44,
	0x78,0x00,0x90,0xCB,0x88,0x11,0x3A,0x94,0x65,0xC0,0x7C,0x63,0x87,0xF0,0x3C,0xAF,
	0xD6,0x25,0xE4,0x8B,0x38,0x0A,0xAC,0x72,0x21,0xD4,0xF8,0x07 } ,
	// title
	{ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
	// game code
	0x00000000,
	// maker code
	0x3130,
	// fixed
	0x96,
	// unit_code
	0x00,
	// device type
	0x00,
	// unused
	{ 0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
	// game version
	0x00,
	// complement
	0x00,
	// checksum
	0x0000
};

//---------------------------------------------------------------------------------
char HeaderComplement()
/*---------------------------------------------------------------------------------
	Calculate Header complement check
---------------------------------------------------------------------------------*/
{
	int n;
	char c = 0;
	char *p = (char *)&header + 0xA0;
	for (n=0; n<0xBD-0xA0; n++)
	{
		c += *p++;
	}
	return -(0x19+c);
}


//---------------------------------------------------------------------------------
int main(int argc, char *argv[])
//---------------------------------------------------------------------------------
{
	int arg;
	char *argfile = 0;
	FILE *infile;

	int size,bit;

	// show syntax
	if (argc <= 1)
	{
		printf("GBA ROM fixer v"VER" by Dark Fader / BlackThunder / WinterMute / Diegoisawesome \n");
		printf("Syntax: gbafix <rom.gba> [-p] [-t[title]] [-c<game_code>] [-m<maker_code>] [-r<version>] [-d<debug>]\n");
		printf("\n");
		printf("parameters:\n");
		printf("	-p              Pad to next exact power of 2. No minimum size!\n");
		printf("	-t[<title>]     Patch title. Stripped filename if none given.\n");
		printf("	-c<game_code>   Patch game code (four characters)\n");
		printf("	-m<maker_code>  Patch maker code (two characters)\n");
		printf("	-r<version>     Patch game version (number)\n");
		printf("	-d<debug>       Enable debugging handler and set debug entry point (0 or 1)\n");
		return -1;
	}

	// get filename
	for (arg=1; arg<argc; arg++)
	{
		if ((ARGV[0] != '-')) { argfile=ARGV; break; }
	}

	// check filename
	if (!argfile)
	{
		printf("Filename needed!\n");
		return -1;
	}

	// read file
	infile = fopen(argfile, "r+b");
	if (!infile) { printf("Error opening input file!\n"); return -1; }
	fseek(infile, 0, SEEK_SET);
	fread(&header, sizeof(header), 1, infile);

	// fix some data
	memcpy(header.logo, good_header.logo, sizeof(header.logo));
	memcpy(&header.fixed, &good_header.fixed, sizeof(header.fixed));
	memcpy(&header.device_type, &good_header.device_type, sizeof(header.device_type));

	// parse command line
	for (arg=1; arg<argc; arg++)
	{
		if ((ARGV[0] == '-'))
		{
			switch (ARGV[1])
			{
				case 'p':	// pad
				{
					fseek(infile, 0, SEEK_END);
					size = ftell(infile);
					for (bit=31; bit>=0; bit--) if (size & (1<<bit)) break;
					if (size != (1<<bit))
					{
						int todo = (1<<(bit+1)) - size;
						while (todo--) fputc(0xFF, infile);
					}
					fseek(infile, 0, SEEK_SET);
					break;
				}

				case 't':	// title
				{
					char title[256];
					memset(title, 0, sizeof(title));
					if (VALUE[0])
					{
						strncpy(title, VALUE, sizeof(header.title));
					}
					else
					{
						// use filename
						char s[256], *begin=s, *t; strcpy(s, argfile);
						t = strrchr(s, '\\'); if (t) begin = t+1;
						t = strrchr(s, '/'); if (t) begin = t+1;
						t = strrchr(s, '.'); if (t) *t = 0;
						strncpy(title, begin, sizeof(header.title));
						printf("%s\n",begin);
					}
					memcpy(header.title, title, sizeof(header.title));	// copy
					break;
				}

				case 'c':	// game code
				{
					//if (!VALUE[0]) { printf("Need value for %s\n", ARGV); break; }
					//header.game_code = NUMBER;
					header.game_code = VALUE[0] | VALUE[1]<<8 | VALUE[2]<<16 | VALUE[3]<<24;
					break;
				}

				case 'm':	// maker code
				{
					//if (!VALUE[0]) { printf("Need value for %s\n", ARGV); break; }
					//header.maker_code = (unsigned short)NUMBER;
					header.maker_code = VALUE[0] | VALUE[1]<<8;
					break;
				}

				case 'v':	// ignored, compatability with other gbafix
				{
					break;
				}

				case 'r':	// version
				{
					if (!VALUE[0]) { printf("Need value for %s\n", ARGV); break; }
					header.game_version = (unsigned char)NUMBER;
					break;
				}

				case 'd':	// debug
				{
					if (!VALUE[0]) { printf("Need value for %s\n", ARGV); break; }
					header.logo[0x9C-0x04] = 0xA5;	// debug enable
					header.device_type = (unsigned char)((NUMBER & 1) << 7);	// debug handler entry point
					break;
				}

			default:
				{
					printf("Invalid option: %s\n", ARGV);
				}
			}
		}
	}

	// update complement check & total checksum
	header.complement = 0;
	header.checksum = 0;	// must be 0
	header.complement = HeaderComplement();
	//header.checksum = checksum_without_header + HeaderChecksum();

	fseek(infile, 0, SEEK_SET);
	fwrite(&header, sizeof(header), 1, infile);
	fclose(infile);

	printf("ROM fixed!\n");

	return 0;
}
