/**
 * Allegro ActionListener example for Guichan.
 */

// Include all necessary headers.
#include <iostream>
#include <sstream>         // Used for int to string conversion
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
gcn::Gui* gui;            // A Gui object - binds it all together
gcn::Container* top;      // A top container
gcn::ImageFont* guiFont;  // A font
gcn::Button* button1;     // A button for actionlistening
gcn::Button* button2;     // Another button for actionlistening
gcn::Label* label1;       // And a label for button1 click count display
gcn::Label* label2;       // And another label for button2 click count display

int clickCountButton1 = 0; // Count clicks for button1
int clickCountButton2 = 0; // Count clicks for button2

/*
 * To be able to recieve actions we must have a class inheriting from
 * ActionListener and implementing the action function.
 * When an action is generated in a Widget (i.e when a button is clicked)
 * that widget calls the action function for all its action listeners.
 * Below we make a class to be able to recieve our buttons actions.
 */
class ButtonActionListener : public gcn::ActionListener
{
public:
	// Implement the action function in ActionListener to recieve actions
	// The eventId tells us which widget called the action function.
	void action(const gcn::ActionEvent& actionEvent)
	{
		std::string str;
		std::ostringstream os(str);

		// Here we use the widget pointer to check which widget the action
        // originated from.
		if (actionEvent.getSource() == button1)
		{
			clickCountButton1++;
			os << "Button1 clicks " << clickCountButton1;
			// Change the caption to show number of clicks
			label1->setCaption(os.str());
			// Adjust the label to fit the new caption
			label1->adjustSize();
		}
		// Here we use the event id in order to check what action occured.
		else if (actionEvent.getId() == "button2")
		{
			clickCountButton2++;
			os << "Button2 clicks " << clickCountButton2;
			// Change the caption to show number of clicks
			label2->setCaption(os.str());
			// Adjust the label to fit the new caption
			label2->adjustSize();
		}
	}
};

ButtonActionListener* buttonActionListener; // A pointer to the above class

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
	// Set gui to use the SDLGraphics object.
	gui->setGraphics(graphics);
	// Set gui to use the SDLInput object
	gui->setInput(input);
	// Set the top container
	gui->setTop(top);
	// Load the image font.
	guiFont = new gcn::ImageFont("fixedfont.bmp", " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
	// The global font is static and must be set.
	gcn::Widget::setGlobalFont(guiFont);

	// Create buttons
	button1 = new gcn::Button("Button 1");
	button2 = new gcn::Button("Button 2");
	// Set the buttons position
	button1->setPosition(120, 230);
	button2->setPosition(420, 230);
	// Add the buttons to the top container
	top->add(button1);
	top->add(button2);

	// Create labels
	label1 = new gcn::Label("Button1 clicks 0");
	label2 = new gcn::Label("Button2 clicks 0");
	// Set the labels position
	label1->setPosition(100, 200);
	label2->setPosition(400, 200);
	// Add the labels to the top container
	top->add(label1);
	top->add(label2);


	// Set the buttons action event id's.
	button1->setActionEventId("button1");
	button2->setActionEventId("button2");

	// Make an instance of the ButtonActionListener
	buttonActionListener = new ButtonActionListener();

	// Add the ButtonActionListener to the buttons action listeners
	button1->addActionListener(buttonActionListener);
	button2->addActionListener(buttonActionListener);
}

/**
 * Halts the application
 */
void halt()
{
	/*
	 * Destroy our action listener
	 */
	delete buttonActionListener;

	/*
	 * Destroy Guichan stuff
	 */
	delete label1;
	delete label2;
	delete button1;
	delete button2;
	delete guiFont;
	delete top;
	delete gui;

	/*
	 * Destroy Guichan SDL stuff
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
