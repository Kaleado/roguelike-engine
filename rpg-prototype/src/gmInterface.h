#ifndef GMINTERFACE_H
#define GMINTERFACE_H

#include "gmRender.h"

class gmThing;

#define NOT_SELECTED -1
#define CANCEL -2

class gmSidePanel
{
public:
	std::vector <std::string> options;
	SDL_Surface* background = NULL;
	int finalSelection = NOT_SELECTED;
	int currentSelection = NOT_SELECTED;
	SDL_Surface* cursor = NULL;
	void show()
	{
		if (!background){ background = load_surface("res/side-panel-background.png"); }
		apply(SCREEN_WIDTH, 0, background, screen);
		TTF_Font* font = TTF_OpenFont("res/arial.ttf", 10);
		SDL_Colour fontColour = { 0, 0, 0 };
		SDL_Surface* text = NULL;
		for (int i = 0; i < options.size(); i++)
		{
			text = TTF_RenderText_Blended(font, options[i].c_str(), fontColour);
			apply(SCREEN_WIDTH + 3+12, 3 + 10 * (i % 23), text, screen);
		}
		if (currentSelection != NOT_SELECTED)
		{
			if (!cursor){ cursor = IMG_Load("res/list-cursor.png"); }
			apply(SCREEN_WIDTH + 3, 3 + 10 * currentSelection, cursor, screen);
		}
		SDL_FreeSurface(text);
		TTF_CloseFont(font);
		return;
	}
	int selectFromList()
	{
		SDL_Event e;
		finalSelection = NOT_SELECTED;
		currentSelection = NOT_SELECTED;
		while (finalSelection == NOT_SELECTED)
		{
			while (SDL_PollEvent(&e) != 0)
			{
				if (e.type == SDL_QUIT)
				{
					finalSelection = CANCEL;
					break;
				}
				if (e.type == SDL_KEYDOWN)
				{
					switch (e.key.keysym.sym)
					{
					case SDLK_KP_2:
					case SDLK_UP:
						currentSelection = --currentSelection < 0 ? 0 : currentSelection;
						break;
					case SDLK_KP_8:
					case SDLK_DOWN:
						currentSelection = ++currentSelection > options.size()-1? options.size()-1: currentSelection;
						break;
					case SDLK_RETURN:
					case SDLK_KP_ENTER:
						finalSelection = currentSelection;
						if (finalSelection != NOT_SELECTED){ return finalSelection; }
						break;
					case SDLK_ESCAPE:
						finalSelection = CANCEL;
						break;
					}
				}
				show();
				SDL_UpdateWindowSurface(gWindow);
			}
		}
		return finalSelection;
	}
}sidePanel;

SDL_Surface* messageLogBackground = NULL;
SDL_Surface* sidePanelBackground = NULL;
std::vector <std::string> messageLog;
TTF_Font* font;

void renderMessageLog()
{
	if (!font){ font = TTF_OpenFont("res/arial.ttf", 10); }
	SDL_Colour fontColour = {0, 0, 0};
	if (!messageLogBackground){ messageLogBackground = load_surface("res/message-log-background.png"); }
	apply(0, SCREEN_HEIGHT, messageLogBackground, screen);
	SDL_Surface* message = NULL;
	for (int i = 0; i < messageLog.size(); i++)
	{
		message = TTF_RenderText_Blended(font, messageLog[i].c_str(), fontColour);
		if (i % 23 == 0){ apply(0, SCREEN_HEIGHT, messageLogBackground, screen); }
		apply(3, SCREEN_HEIGHT + 3 + 10 * (i%23), message, screen);
	}
	SDL_FreeSurface(message);
	//TTF_CloseFont(font);
	return;
}

void renderInterface()
{
	renderMessageLog();
	//renderSidePanel();
	return;
}

#endif