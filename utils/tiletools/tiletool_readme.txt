== Install ==

Basically just run "make" and pray :-)

Compile imgtool.cpp + usage_tilemaker.cpp into "tilemaker"
and link to SDL_image, SDL and png.

And you probably want to uncompress: tilemask_initial.tar.gz
into the working directory.

Note: I added a generated cpp file to the svn repo because
      non-unix users probably don't have those tools.

== Usage ==

Once you have a working "tilemaker" you need a few input images 
(24 bit depth), which SDL_image can load (I used bmp and jpg, 
to make sure there is no alpha).

Assume I have "green.bmp" and "blue.bmp"...
./tilemaker --create test1.png --mask tilemask_grey.bmp \
  --first green.bmp

Look at test1.png...

./tilemaker --edit test1.png --mask tilemask_grey.bmp \
  --first blue.bmp --col 1

Look at test1.png ...

Now combine images into one tile:

./tilemaker --edit test1.png --mask tilemask_north.bmp \
  --first blue.bmp --second green.bmp --row 1
./tilemaker --edit test1.png --mask tilemask_north.bmp \
  --first green.bmp --first blue.bmp --row 1 --col 1

Look at test1.png ...

== Future plans ==

* more mask images, the 1/2 border should meet the tile border
  in the same pixels as now, but the border should not be 
  straight lines...
* masks with fuzzy 1/2 border

With code changes:
* Enable alpha blending of src images.
