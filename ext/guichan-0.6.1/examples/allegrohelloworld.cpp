/**
 * Allegro Hello World example for Guichan.
 */

// Include all necessary headers.
#include <iostream>
#include <guichan.hpp>
#include <guichan/allegro.hpp>
#include <allegro.h>

/*
 * Allegro Stuff we need
 */
BITMAP *screenBuffer;

/*
 * Guichan Allegro stuff we need
 */
gcn::AllegroInput* input;              // Input driver
gcn::AllegroGraphics* graphics;        // Graphics driver
gcn::AllegroImageLoader* imageLoader;  // For loading images

/*
 * Guichan stuff we need
 */
gcn::Gui* gui;           // A Gui object - binds it all together
gcn::Container* top;     // A top container
gcn::ImageFont* guiFont; // A font
gcn::Label* label;       // And a label for the Hello World text

/**
 * Initializes the Hello World
 */
void init()
{
	/*
	 * Here we initialize Allegro as we
	 * would do with any Allegro application.
	 */
	allegro_init();

	int bpp = desktop_color_depth();
	if (bpp == 0)
	{
		bpp = 16;
	}

	set_color_depth(bpp);

	if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0))
	{
		if (set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0))
		{
			throw GCN_EXCEPTION("Unable to set graphics mode");
		}
	}

	screenBuffer = create_bitmap(SCREEN_W, SCREEN_H);

	if (screenBuffer == NULL)
	{
		throw GCN_EXCEPTION("Unable to create a screen buffer");
	}

	install_keyboard();
	install_mouse();
	install_timer();

	/*
	 * Ok, we have Allegro set up, now its time to add the glue
	 * between Allegro and Guichan
	 */
	imageLoader = new gcn::AllegroImageLoader();
	// The ImageLoader in use is static and must be set to be
	// able to load images
	gcn::Image::setImageLoader(imageLoader);

	graphics = new gcn::AllegroGraphics();
	// Set the target for the graphics object to be the doublebuffer
	// for the screen. Drawing to the screen directly is not a good
	// idea, as it will produce flicker, unless you use page flipping.
	graphics->setTarget(screenBuffer);

	input = new gcn::AllegroInput();

	/*
	 * Last but not least it's time to initialize and create the gui
	 * with Guichan stuff.
	 */
	top = new gcn::Container();
	// Set the dimension of the top container to match the screen.
	top->setDimension(gcn::Rectangle(0, 0, 640, 480));
	gui = new gcn::Gui();
	// Set gui to use the AllegroGraphics object.
	gui->setGraphics(graphics);
	// Set gui to use the AllegroInput object
	gui->setInput(input);
	// Set the top container
	gui->setTop(top);
	// Load the image font.
	guiFont = new gcn::ImageFont("fixedfont.bmp", " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
	// The global font is static and must be set.
	gcn::Widget::setGlobalFont(guiFont);

	// Create a label with test hello world
	label = new gcn::Label("Hello World");
	// Set the labels position
	label->setPosition(280, 220);
	// Add the label to the top container
	top->add(label);
}

/**
 * Halts the application
 */
void halt()
{
	/*
	 * Destroy Guichan stuff
	 */
	delete label;
	delete guiFont;
	delete top;
	delete gui;

	/*
	 * Destroy Guichan Allegro stuff
	 */
	delete input;
	delete graphics;
	delete imageLoader;

	/*
	 * Destroy Allegro stuff
	 */
	destroy_bitmap(screenBuffer);
}

/**
 * Runs the application
 */
void run()
{
	while(!key[KEY_ESC])
	{
		// Let the gui perform it's logic (like handle input)
		gui->logic();

		// Draw the gui
		gui->draw();

		// We draw the mouse pointer manually, as Allegro's mouse
		// drawing code is so wierd.
		draw_sprite(screenBuffer, mouse_sprite, mouse_x, mouse_y);

		// Update the screen
		blit(screenBuffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
	}
}

int main(int argc, char **argv)
{
	try
	{
 		init();
		run();
		halt();
	}
	/*
	 * Catch all Guichan exceptions
	 */
	catch (gcn::Exception e)
	{
		std::cerr << e.getMessage() << std::endl;
		return 1;
	}
	/*
	 * Catch all Std exceptions
	 */
	catch (std::exception e)
	{
		std::cerr << "Std exception: " << e.what() << std::endl;
		return 1;
	}
	/*
	 * Catch all Unknown exceptions
	 */
	catch (...)
	{
		std::cerr << "Unknown exception" << std::endl;
		return 1;
	}

	return 0;
}
END_OF_MAIN()
