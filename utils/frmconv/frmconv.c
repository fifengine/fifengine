//frmconv - A tool to convert the frm file format to png/mng
//Copyright (C) 2005 IanOut Team (RogerWilco)
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//version 2 as published by the Free Software Foundation; 
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA


// frmconv is inspired by frm2bmp by Borg Locutus (dystopia@iname.com) and 
// the documentation of the frm file format (written by Noid and Anchorite)
// was used extensively



// ToDo: *Add mng support for true animations

#ifdef _WIN32
#define ssize_t int
#include <io.h>
#include <sys/types.h>
#include <Winsock2.h>
#else
#include <netinet/in.h>
#include <getopt.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "frm.h"
#include "out_png.h"

const char helpstr[] = "frmconv v1.0 - (c) 2005 by IanOut Team (RogerWilco)\nUsage: %s [-m] -f <frm-file> [-o <basename>]\n";
const char verstr[] = "frmconv v1.0 - (c) 2005 by IanOut Team (RogerWilco)\n";

int main(int argc, char *argv[]) {
	char *buf=NULL, *inputname=NULL, *outputname=NULL;
	frm_frame *frame;
	frm_anim *anim;
	FILE *outfile=NULL, *inputfile;
	short int mng=0;
	int c, i;
	
	if (argc > 6) {
		printf(helpstr,argv[0]);
		return 0;	
	}
	
	while ( (c = getopt(argc, argv, "mf:o:")) ) {
		if (c == -1)
			break;
			
		switch (c) {
			case 'm':
				mng = 1;
				printf("Sorry, mng support is not working.\n");
				return 2;
				break;
			case 'f':
				inputname = optarg;
				break;
			case 'o':
				outputname = optarg;
				break;
		}
	}
	
	if (!inputname) {
		printf("frmconv v1.0 - (c) 2005 by IanOut Team (RogerWilco)\nUsage: %s [-m] -f <frm-file> [-o <basename>]\n", argv[0]);
		return 1;
	}
	
	if (0 != (inputfile = fopen(inputname, "rb"))) {
		anim = loadFRMAnim(inputfile);
	} else {
		printf("Couldn't open %s.\n", inputname);
		return 2;
	}
		
	
	if (anim) {
		if (outputname) {
			buf = malloc(strlen(outputname)+8);
			if (!buf) {
				freefrmanim(anim);
				return 1;
			}
			strcpy(buf, outputname);
		}
		
		frame = anim->frames;
		
		for (i=0; frame && i<anim->nrofframes && i<999; ++i) {
			if (outputname) {
				sprintf(buf, "%s%03d.png", outputname, i);
				outfile = fopen(buf, "wb");
			}
			
			if (mng) {
			} else {
				writePNGFile(frame, anim->palette, outputname?outfile:stdout);
			}
			
			if (outfile)
				fclose(outfile);
				
			frame = frame->next_frame;
		}
	}
	
	fclose(inputfile);
	freefrmanim(anim);
	return 0;
}
