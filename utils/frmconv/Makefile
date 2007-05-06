# A small makefile

.PHONY:	clean

frmconv:	frmconv.o out_png.o frm.o
	gcc -Wall --std=c99 ${CFLAGS} -o frmconv frm.o frmconv.o out_png.o -lz -lm -lpng

frmconv.o:	frmconv.c frm.h out_png.h
	gcc -Wall --std=c99 ${CFLAGS} -c frmconv.c

out_png.o:	out_png.c out_png.h frm.h
	gcc -Wall --std=c99 ${CFLAGS} -c out_png.c

frm.o:	frm.c frm.h pal.h
	gcc -Wall --std=c99 ${CFLAGS} -c frm.c

clean:
	rm -f *.o frmconv
