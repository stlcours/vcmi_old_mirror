#pragma once
#include "CPlayerInterface.h"
#include <boost/function.hpp>
#include <boost/bind.hpp>
class AdventureMapButton 
	: public ClickableL, public ClickableR, public Hoverable, public KeyInterested, public CButtonBase
{
public:
	std::string name; //for status bar 
	std::string helpBox; //for right-click help
	char key; //key shortcut
	boost::function<void()> callback;
	bool colorChange,
		actOnDown; //runs when mouse is pressed down over it, not when up

	void clickRight (tribool down);
	void clickLeft (tribool down);
	virtual void hover (bool on);
	void keyPressed (SDL_KeyboardEvent & key);
	void activate(); // makes button active
	void deactivate(); // makes button inactive (but doesn't delete)

	AdventureMapButton(); //c-tor
	AdventureMapButton( std::string Name, std::string HelpBox, boost::function<void()> Callback, int x, int y, std::string defName, bool activ=false,  std::vector<std::string> * add = NULL, bool playerColoredButton = false );//c-tor
};


//template<typename T>
class CSlider : public IShowable, public MotionInterested, public ClickableL
{
public:
	AdventureMapButton left, right, slider; //if vertical then left=up
	int capacity,//how many elements can be active at same time
		amount, //how many elements
		value; //first active element
	bool horizontal, moving;
	CDefEssential *imgs ;

	boost::function<void(int)> moved;
	//void(T::*moved)(int to);
	//T* owner;

	void redrawSlider(); 

	void sliderClicked();
	void moveLeft();
	void clickLeft (tribool down);
	void mouseMoved (SDL_MouseMotionEvent & sEvent);
	void moveRight();
	void moveTo(int to);
	void activate(); // makes button active
	void deactivate(); // makes button inactive (but doesn't delete)
	void show(SDL_Surface * to = NULL);
	CSlider(int x, int y, int totalw, boost::function<void(int)> Moved, int Capacity, int Amount, 
		int Value=0, bool Horizontal=true);
	~CSlider();
};	
