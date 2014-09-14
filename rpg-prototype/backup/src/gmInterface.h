#ifndef GMINTERFACE_H
#define GMINTERFACE_H

#include "gmRender.h"

class gmThing;

#define NOT_SELECTED -1
#define CANCEL -2

class gmInterface
{
public:
	SDL_Surface* background = NULL;
	void show();
};

class gmSidePanel : public gmInterface
	/*
	*USAGE:
	*sidePanel.heading = <heading as string>;
	*sidePanel.options = <list of options as string vector>;
	*sidePanel.selectFromList();
	*<function using sidePanel.finalSelections.pop() as a parameter, in order of pushing.>;
	*sidePanel.reset();
	*/
{
public:
	std::vector <std::string> options;
	std::string heading;
	std::vector <int> finalSelectionList;
	int currentSelection = NOT_SELECTED;
	SDL_Surface* cursor = NULL;
	TTF_Font* font = NULL;
	SDL_Colour fontColour;
	void show()
	{
		fontColour = { 0, 0, 0 };
		if (!background){ background = load_surface("res/side-panel-background.png"); }
		apply(SCREEN_WIDTH, 0, background, screen);
		if (font == NULL){ font = TTF_OpenFont("res/arial.ttf", 10); }
		SDL_Surface* text = NULL;
		text = TTF_RenderText_Blended(font, heading.c_str(), fontColour);
		apply(SCREEN_WIDTH + 3 + 12, 3, text, screen);
		for (int i = 0; i < options.size(); i++)
		{
			text = TTF_RenderText_Blended(font, options[i].c_str(), fontColour);
			apply(SCREEN_WIDTH + 3+24, 13 + 10 * i, text, screen);
		}
		if (currentSelection != NOT_SELECTED)
		{
			if (!cursor){ cursor = IMG_Load("res/list-cursor.png"); }
			apply(SCREEN_WIDTH + 3, 13 + 10 * currentSelection, cursor, screen);
		}
		SDL_FreeSurface(text);
		return;
	}
	void selectFromList()
	{
		if (options.size() > 0)
		{
			SDL_Event e;
			int finalSelection = NOT_SELECTED;
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
							if (finalSelection != NOT_SELECTED){ finalSelectionList.push_back(finalSelection); return; }
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
			finalSelectionList.push_back(finalSelection);
		}
		else{ finalSelectionList.push_back(CANCEL); }
		return;
	}
	void reset()
	{
		heading = "";
		options = std::vector<std::string>();
		currentSelection = NOT_SELECTED;
		finalSelectionList = std::vector <int>();
		return;
	}
}sidePanel;

class gmMessageLog : public gmInterface
{
public:
	std::vector <std::string> log;
	TTF_Font* font;
	void show()
	{
		if (log.size() > 100){ log.erase(log.begin(), log.begin() + 46); }
		if (!font){ font = TTF_OpenFont("res/arial.ttf", 10); }
		SDL_Colour fontColour = {0, 0, 0};
		if (!background){ background = load_surface("res/message-log-background.png"); }
		apply(0, SCREEN_HEIGHT, background, screen);
		SDL_Surface* message = NULL;
		for (int i = 0; i < log.size(); i++)
		{
			message = TTF_RenderText_Blended(font, log[i].c_str(), fontColour);
			if (i % 23 == 0){ apply(0, SCREEN_HEIGHT, background, screen); }
			apply(3, SCREEN_HEIGHT + 3 + 10 * (i%23), message, screen);
		}
		SDL_FreeSurface(message);
		return;
	}
}messageLog;

#endif