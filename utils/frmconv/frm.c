// test
#ifdef _WIN32
#define ssize_t int
#include <io.h>
#include <sys/types.h>
#include <Winsock2.h>
#else
#include <netinet/in.h>
#include <unistd.h>
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "frm.h"
#include "pal.h"

#define ntoh32(BASE, OFF) ntohl( ( ((uint32_t)(BASE[(OFF+3)])) << 24 ) | ( ((uint32_t)(BASE[(OFF) + 2])) << 16 ) | ( ((uint32_t)(BASE[(OFF) + 1])) << 8 ) | ((uint32_t)(BASE[(OFF)])))
#define ntoh16(BASE, OFF) ntohs( (((uint16_t)(BASE[(OFF)+1])) << 8) | ((uint16_t)(BASE[(OFF)])) )


void freefrmanim(frm_anim *anim) {
	frm_frame *frp, *frp2;
	
	if (!anim)
		return;

	if ( (frp2 = (frp = anim->frames)) ) {
		do {
			frp2 = frp->next_frame;
			free(frp);
			frp = frp2;
		} while (frp);
	}
	
	free(anim);
}

frm_frame *fetchFRMFrame(unsigned char *buf, uint32_t buflen, uint8_t direct) {
	int i = 0;
	frm_frame *frame = malloc(sizeof(frm_frame));

	if (!frame || buflen < 8)
		return NULL;

	frame->orient = direct;
	frame->width = ntoh16(buf, FRMFRAME_OFFSET_WIDTH);
	frame->height = ntoh16(buf, FRMFRAME_OFFSET_HEIGHT);
	frame->framesize = ntoh32(buf, FRMFRAME_OFFSET_SIZE);

	if (frame->framesize != frame->width * frame->height) {
		printf("ERR: Framesize different: %d %d\n", frame->framesize, frame->width * frame->height);
		return NULL;
	}
	
	if (buflen < (12 + frame->width * frame->height) ) {
		return NULL;
	}
	
	frame->rowptr = malloc( sizeof(unsigned char *) * frame->height);
	if (!frame->rowptr) {
		printf("ERROR: no memory\n");
		free(frame);
		return NULL;
	}
	
	for(i=0; i < frame->height; ++i) {
		frame->rowptr[i] = malloc(sizeof(unsigned char) * frame->width);
		if (!frame->rowptr[i]) {
			printf("ERROR: no memory\n");
			free(frame);
			return NULL;
		}
		memcpy(frame->rowptr[i], &buf[12]+i*frame->width, frame->width);
	}
	
	frame->next_frame = NULL;
	return frame;
}



/*
 * frm_anim *loadFRMAnim(FILE *frmfile)
 * 
 * Paramter: frmfile is an already opened FILE handle.
 * Return: a pointer to frm_anim structure for the frm animation; NULL in case of an error
 * 
 * Note: 1) loadFRMAnim calls rewind and fseek on frmfile.
 *       2) loadFRMAnim does not close frmfile
 * 
 * Overview:
 * loadFRMAnim tries to load a frm file from frmfile and creates a frm_anim structure that has
 * to be freed later with freefrmanim(frmanim *). Please look at the frm_anim for an explanation of
 * the structure.
 * 
 */

frm_anim *loadFRMAnim(FILE *frmfile) {
	uint32_t frmfilesize, offset=0;
	uint8_t direct=0;
	unsigned char *filebuf;
	frm_anim *frmp;
	frm_frame *frame;

	// Get the length of the file frmfile:
	rewind(frmfile);
	fseek(frmfile, 0l, SEEK_END);
	frmfilesize = ftell(frmfile);
	rewind(frmfile);
	
	if (frmfilesize < 0x3e) {
		printf("Error: loadFRMAnim - File length to short. This cannout be a frm file.\n");
		return NULL;
	}
	
	filebuf = malloc(frmfilesize);
	if (!filebuf) {
		printf("Error: loadFRMAnim - Couldn't allocate %d Byte memory for the file.\n", frmfilesize);
		return NULL;
	}
	
	if (frmfilesize != fread(filebuf, 1, frmfilesize, frmfile)) {
		printf("Error: loadFRMAnim - Reading the whole file failed.\n");
		free(filebuf);
		return NULL;
	}
	
	if (NULL == (frmp = malloc(sizeof(frm_anim)))) {
		printf("Error: loadFRMAnim - Couldn't allocate %d Byte memory for the frm_anim struct.\n", sizeof(frm_anim));
		free(filebuf);
		return NULL;
	}
	
	frmp->nrofframes = 0;
	frmp->palette = pal;
	frmp->frames = NULL;
	frmp->frameperorient = ntoh16(filebuf, FRM_OFFSET_FRAMESPERORIENT);
	frmp->frameareasize = ntoh32(filebuf, FRM_OFFSET_FRAMEAREASIZE);
	
	frame = (frmp->frames = fetchFRMFrame(&filebuf[FRM_OFFSET_FRAMEDATA], frmfilesize-FRM_OFFSET_FRAMEDATA, direct));
	
	if (frame) {
		offset = FRM_OFFSET_FRAMEDATA + FRMFRAME_OFFSET_DATA + frame->framesize;
		frmp->nrofframes = 1;
	} else {
		printf("Warn: loadFRMAnim - Haven't found one frame.\n");
	}
		
	while (frame && (frame->next_frame = fetchFRMFrame(&filebuf[offset], frmfilesize-offset, direct) )) {
		frame = frame->next_frame;
		offset += FRMFRAME_OFFSET_DATA + frame->framesize;
		++frmp->nrofframes;
		
		if (offset > filebuf[FRM_OFFSET_FIRSTFRAME_ORIENT5]) {
			direct=5;
		} else if (offset > filebuf[FRM_OFFSET_FIRSTFRAME_ORIENT4]) {
			direct=4;
		} else if (offset > filebuf[FRM_OFFSET_FIRSTFRAME_ORIENT3]) {
			direct=3;
		} else if (offset > filebuf[FRM_OFFSET_FIRSTFRAME_ORIENT2]) {
			direct=2;
		} else if (offset > filebuf[FRM_OFFSET_FIRSTFRAME_ORIENT1]) {
			direct=1;
		} else if (offset > filebuf[FRM_OFFSET_FIRSTFRAME_ORIENT0]) {
			direct=0;
		} else { 
			printf("Error: loadFRMAnim - wrong offset.\n");
			freefrmanim(frmp);
			free(filebuf);
			return 0;
		}
	}
	free(filebuf);
	return frmp;
}
