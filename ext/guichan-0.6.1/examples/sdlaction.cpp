/**
 * SDL ActionListner example for Guichan.
 */

// Include all necessary headers.
#include <iostream>
#include <sstream>         // Used for int to string conversion
#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include "SDL.h"

/*
 * Common stuff we need
 */
bool running = true;

/*
 * SDL Stuff we need
 */
SDL_Surface* screen;
SDL_Event event;

/*
 * Guichan SDL stuff we need
 */
gcn::SDLInput* input;             // Input driver
gcn::SDLGraphics* graphics;       // Graphics driver
gcn::SDLImageLoader* imageLoader; // For loading images

/*
 * Guichan stuff we need
 */
gcn::Gui* gui;            // A Gui object - binds it all together
gcn::Container* top;      // A top container
gcn::ImageFont* font;     // A font
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
	 * Here we initialize SDL as we would do with any SDL application.
	 */
	SDL_Init(SDL_INIT_VIDEO);
	screen = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE);
	// We want unicode
	SDL_EnableUNICODE(1);
	// We want to enable key repeat
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	/*
	 * Now it's time for Guichan SDL stuff
	 */
	imageLoader = new gcn::SDLImageLoader();
	// The ImageLoader in use is static and must be set to be
	// able to load images
	gcn::Image::setImageLoader(imageLoader);
	graphics = new gcn::SDLGraphics();
	// Set the target for the graphics object to be the screen.
	// In other words, we will draw to the screen.
	// Note, any surface will do, it doesn't have to be the screen.
	graphics->setTarget(screen);
	input = new gcn::SDLInput();

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
	font = new gcn::ImageFont("fixedfont.bmp", " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
	// The global font is static and must be set.
	gcn::Widget::setGlobalFont(font);

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
	delete font;
	delete top;
	delete gui;

	/*
	 * Destroy Guichan SDL stuff
	 */
	delete input;
	delete graphics;
	delete imageLoader;

	/*
	 * Destroy SDL stuff
	 */
	SDL_Quit();
}

/**
 * Checks input. On escape halt the application.
 */
void checkInput()
{
	/*
	 * Poll SDL events
	 */
	while(SDL_PollEvent(&event))
	{
		if (event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				running = false;
			}
			if (event.key.keysym.sym == SDLK_f)
			{
				if (event.key.keysym.mod & KMOD_CTRL)
				{
					// Works with X11 only
					SDL_WM_ToggleFullScreen(screen);
				}
			}
		}
		else if(event.type == SDL_QUIT)
		{
			running = false;
		}

		/*
		 * Now that we are done polling and using SDL events we pass
		 * the leftovers to the SDLInput object to later be handled by
		 * the Gui. (This example doesn't require us to do this 'cause a
		 * label doesn't use input. But will do it anyway to show how to
		 * set up an SDL application with Guichan.)
		 */
		input->pushInput(event);
	}
}

/**
 * Runs the application
 */
void run()
{
	while(running)
	{
		// Poll input
		checkInput();
		// Let the gui perform it's logic (like handle input)
		gui->logic();
		// Draw the gui
		gui->draw();
		// Update the screen
		SDL_Flip(screen);
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
