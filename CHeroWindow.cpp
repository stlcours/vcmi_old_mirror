#include "stdafx.h"
#include "global.h"
#include "CHeroWindow.h"
#include "CGameInfo.h"
#include "hch\CHeroHandler.h"
#include "hch\CGeneralTextHandler.h"
#include "SDL.h"
#include "SDL_Extensions.h"
#include "CAdvmapInterface.h"
#include "AdventureMapButton.h"
#include "CMessage.h"
#include <sstream>

extern SDL_Surface * screen;
extern TTF_Font * GEOR16;

CHeroWindow::CHeroWindow(int playerColor): artFeet(0), artHead(0), artLHand(0),
	artLRing(0), artMach1(0), artMach2(0), artMach3(0), artMach4(0), artMisc1(0),
	artMisc2(0), artMisc3(0), artMisc4(0), artMisc5(0), artNeck(0), artRhand(0),
	artRRing(0), artShoulders(0), artSpellBook(0), artTorso(0),
	backpackPos(0), player(playerColor)
{
	background = CGI->bitmaph->loadBitmap("HEROSCR4.bmp");
	CSDL_Ext::blueToPlayersAdv(background, playerColor);
	pos.x = 65;
	pos.y = 8;
	pos.h = background->h;
	pos.w = background->w;
	curBack = NULL;
	curHero = NULL;

	quitButton = new AdventureMapButton<CHeroWindow>(std::string(), std::string(), &CHeroWindow::quit, 674, 524, "hsbtns.def", this);
	dismissButton = new AdventureMapButton<CHeroWindow>(std::string(), std::string(), &CHeroWindow::dismissCurrent, 519, 437, "hsbtns2.def", this);
	questlogButton = new AdventureMapButton<CHeroWindow>(std::string(), std::string(), &CHeroWindow::questlog, 379, 437, "hsbtns4.def", this);

	gar1button = new AdventureMapButton<CHeroWindow>(std::string(), std::string(), &CHeroWindow::gar1, 546, 491, "hsbtns6.def", this);
	gar2button = new AdventureMapButton<CHeroWindow>(std::string(), std::string(), &CHeroWindow::gar2, 604, 491, "hsbtns8.def", this);
	gar3button = new AdventureMapButton<CHeroWindow>(std::string(), std::string(), &CHeroWindow::gar3, 546, 527, "hsbtns7.def", this);
	gar4button = new AdventureMapButton<CHeroWindow>(std::string(), std::string(), &CHeroWindow::gar4, 604, 527, "hsbtns9.def", this);

	leftArtRoll = new AdventureMapButton<CHeroWindow>(std::string(), std::string(), &CHeroWindow::leftArtRoller, 379, 364, "hsbtns3.def", this);
	rightArtRoll = new AdventureMapButton<CHeroWindow>(std::string(), std::string(), &CHeroWindow::rightArtRoller, 632, 364, "hsbtns5.def", this);

	for(int g=0; g<8; ++g)
	{
		//heroList.push_back(new AdventureMapButton<CHeroWindow>(std::string(), std::string(), &CHeroWindow::switchHero, 677, 95+g*54, "hsbtns5.def", this));
		heroListMi.push_back(new LClickableAreaHero());
		heroListMi[g]->pos.x = 677;
		heroListMi[g]->pos.y = 95+g*54;
		heroListMi[g]->pos.h = 32;
		heroListMi[g]->pos.w = 48;
		heroListMi[g]->owner = this;
		heroListMi[g]->id = g;
	}

	skillpics = CGI->spriteh->giveDef("pskil42.def");
	flags = CGI->spriteh->giveDef("CREST58.DEF");
	//areas
	portraitArea = new LClickableAreaWText();
	portraitArea->pos.x = 83;
	portraitArea->pos.y = 26;
	portraitArea->pos.w = 58;
	portraitArea->pos.h = 64;

}

CHeroWindow::~CHeroWindow()
{
	SDL_FreeSurface(background);
	delete quitButton;
	delete dismissButton;
	delete questlogButton;
	delete gar1button;
	delete gar2button;
	delete gar3button;
	delete gar4button;
	delete leftArtRoll;
	delete rightArtRoll;

	for(int g=0; g<heroListMi.size(); ++g)
		delete heroListMi[g];

	if(curBack)
		SDL_FreeSurface(curBack);

	delete skillpics;
	delete flags;

	delete artFeet;
	delete artHead;
	delete artLHand;
	delete artLRing;
	delete artMach1;
	delete artMach2;
	delete artMach3;
	delete artMach4;
	delete artMisc1;
	delete artMisc2;
	delete artMisc3;
	delete artMisc4;
	delete artMisc5;
	delete artNeck;
	delete artRhand;
	delete artRRing;
	delete artShoulders;
	delete artSpellBook;
	delete artTorso;
	for(int g=0; g<backpack.size(); ++g)
	{
		delete backpack[g];
	}
	backpack.clear();

	delete portraitArea;
}

void CHeroWindow::show(SDL_Surface *to)
{
	if(!to)
		to=ekran;
	blitAt(curBack,pos.x,pos.y,to);
	quitButton->show();
	dismissButton->show();
	questlogButton->show();
	gar1button->show();
	gar2button->show();
	gar3button->show();
	gar4button->show();
	leftArtRoll->show();
	rightArtRoll->show();

	artFeet->show(to);
	artHead->show(to);
	artLHand->show(to);
	artLRing->show(to);
	artMach1->show(to);
	artMach2->show(to);
	artMach3->show(to);
	artMach4->show(to);
	artMisc1->show(to);
	artMisc2->show(to);
	artMisc3->show(to);
	artMisc4->show(to);
	artMisc5->show(to);
	artNeck->show(to);
	artRhand->show(to);
	artRRing->show(to);
	artShoulders->show(to);
	artSpellBook->show(to);
	artTorso->show(to);
	for(int d=0; d<backpack.size(); ++d)
	{
		backpack[d]->show(to);
	}
}

void CHeroWindow::setHero(const CGHeroInstance *hero)
{
	if(!hero) //something strange... no hero? it should happen
	{
		return; 
	}
	curHero = hero;
	portraitArea->text = hero->biography;

	delete artFeet;
	delete artHead;
	delete artLHand;
	delete artLRing;
	delete artMach1;
	delete artMach2;
	delete artMach3;
	delete artMach4;
	delete artMisc1;
	delete artMisc2;
	delete artMisc3;
	delete artMisc4;
	delete artMisc5;
	delete artNeck;
	delete artRhand;
	delete artRRing;
	delete artShoulders;
	delete artSpellBook;
	delete artTorso;
	for(int g=0; g<backpack.size(); ++g)
	{
		delete backpack[g];
	}
	backpack.clear();

	artFeet = new CArtPlace(hero->artFeet);
	artFeet->pos.x = 515;
	artFeet->pos.y = 295;
	artFeet->pos.h = artFeet->pos.w = 44;
	artFeet->activate();
	artHead = new CArtPlace(hero->artHead);
	artHead->pos.x = 509;
	artHead->pos.y = 30;
	artHead->pos.h = artHead->pos.h = 44;
	artHead->activate();
	artLHand = new CArtPlace(hero->artLHand);
	artLHand->pos.x = 564;
	artLHand->pos.y = 183;
	artLHand->pos.h = artLHand->pos.h = 44;
	artLHand->activate();
	artLRing = new CArtPlace(hero->artLRing);
	artLRing->pos.x = 610;
	artLRing->pos.y = 183;
	artLRing->pos.h = artLRing->pos.h = 44;
	artLRing->activate();
	artMach1 = new CArtPlace(hero->artMach1);
	artMach1->pos.x = 564;
	artMach1->pos.y = 30;
	artMach1->pos.h = artMach1->pos.h = 44;
	artMach1->activate();
	artMach2 = new CArtPlace(hero->artMach2);
	artMach2->pos.x = 610;
	artMach2->pos.y = 30;
	artMach2->pos.h = artMach2->pos.h = 44;
	artMach2->activate();
	artMach3 = new CArtPlace(hero->artMach3);
	artMach3->pos.x = 610;
	artMach3->pos.y = 76;
	artMach3->pos.h = artMach3->pos.h = 44;
	artMach3->activate();
	artMach4 = new CArtPlace(hero->artMach4);
	artMach4->pos.x = 610;
	artMach4->pos.y = 122;
	artMach4->pos.h = artMach4->pos.h = 44;
	artMach4->activate();
	artMisc1 = new CArtPlace(hero->artMisc1);
	artMisc1->pos.x = 383;
	artMisc1->pos.y = 143;
	artMisc1->pos.h = artMisc1->pos.h = 44;
	artMisc1->activate();
	artMisc2 = new CArtPlace(hero->artMisc2);
	artMisc2->pos.x = 399;
	artMisc2->pos.y = 194;
	artMisc2->pos.h = artMisc2->pos.h = 44;
	artMisc2->activate();
	artMisc3 = new CArtPlace(hero->artMisc3);
	artMisc3->pos.x = 415;
	artMisc3->pos.y = 245;
	artMisc3->pos.h = artMisc3->pos.h = 44;
	artMisc3->activate();
	artMisc4 = new CArtPlace(hero->artMisc4);
	artMisc4->pos.x = 431;
	artMisc4->pos.y = 296;
	artMisc4->pos.h = artMisc4->pos.h = 44;
	artMisc4->activate();
	artMisc5 = new CArtPlace(hero->artMisc5);
	artMisc5->pos.x = 381;
	artMisc5->pos.y = 296;
	artMisc5->pos.h = artMisc5->pos.h = 44;
	artMisc5->activate();
	artNeck = new CArtPlace(hero->artNeck);
	artNeck->pos.x = 508;
	artNeck->pos.y = 79;
	artNeck->pos.h = artNeck->pos.h = 44;
	artNeck->activate();
	artRhand = new CArtPlace(hero->artRhand);
	artRhand->pos.x = 383;
	artRhand->pos.y = 68;
	artRhand->pos.h = artRhand->pos.h = 44;
	artRhand->activate();
	artRRing = new CArtPlace(hero->artRRing);
	artRRing->pos.x = 431;
	artRRing->pos.y = 68;
	artRRing->pos.h = artRRing->pos.h = 44;
	artRRing->activate();
	artShoulders = new CArtPlace(hero->artShoulders);
	artShoulders->pos.x = 567;
	artShoulders->pos.y = 240;
	artShoulders->pos.h = artShoulders->pos.h = 44;
	artShoulders->activate();
	artSpellBook = new CArtPlace(hero->artSpellBook);
	artSpellBook->pos.x = 610;
	artSpellBook->pos.y = 310;
	artSpellBook->pos.h = artSpellBook->pos.h = 44;
	artSpellBook->activate();
	artTorso = new CArtPlace(hero->artTorso);
	artTorso->pos.x = 509;
	artTorso->pos.y = 130;
	artTorso->pos.h = artTorso->pos.h = 44;
	artTorso->activate();
	for(int s=0; s<5 && s<curHero->artifacts.size(); ++s)
	{
		CArtPlace * add = new CArtPlace(curHero->artifacts[(s+backpackPos) % curHero->artifacts.size() ]);
		add->pos.x = 403 + 46*s;
		add->pos.y = 365;
		add->pos.h = add->pos.h = 44;
		add->activate();
		backpack.push_back(add);
	}
}

void CHeroWindow::quit()
{
	for(int i=0; i<LOCPLINT->objsToBlit.size(); ++i)
	{
		if( dynamic_cast<CHeroWindow*>( LOCPLINT->objsToBlit[i] ) )
		{
			LOCPLINT->objsToBlit.erase(LOCPLINT->objsToBlit.begin()+i);
		}
	}
	quitButton->deactivate();
	dismissButton->deactivate();
	questlogButton->deactivate();
	gar1button->deactivate();
	gar2button->deactivate();
	gar3button->deactivate();
	gar4button->deactivate();
	leftArtRoll->deactivate();
	rightArtRoll->deactivate();
	for(int g=0; g<heroListMi.size(); ++g)
	{
		heroListMi[g]->deactivate();
	}

	LOCPLINT->adventureInt->show();

	SDL_FreeSurface(curBack);
	curBack = NULL;
	/*for(int v=0; v<LOCPLINT->lclickable.size(); ++v)
	{
		if(dynamic_cast<CArtPlace*>(LOCPLINT->lclickable[v]))
			LOCPLINT->lclickable.erase(LOCPLINT->lclickable.begin()+v);
	}*/
	portraitArea->deactivate();

	delete artFeet;
	artFeet = 0;
	delete artHead;
	artHead = 0;
	delete artLHand;
	artLHand = 0;
	delete artLRing;
	artLRing = 0;
	delete artMach1;
	artMach1 = 0;
	delete artMach2;
	artMach2 = 0;
	delete artMach3;
	artMach3 = 0;
	delete artMach4;
	artMach4 = 0;
	delete artMisc1;
	artMisc1 = 0;
	delete artMisc2;
	artMisc2 = 0;
	delete artMisc3;
	artMisc3 = 0;
	delete artMisc4;
	artMisc4 = 0;
	delete artMisc5;
	artMisc5 = 0;
	delete artNeck;
	artNeck = 0;
	delete artRhand;
	artRhand = 0;
	delete artRRing;
	artRRing = 0;
	delete artShoulders;
	artShoulders = 0;
	delete artSpellBook;
	artSpellBook = 0;
	delete artTorso;
	artTorso = 0;
	for(int g=0; g<backpack.size(); ++g)
	{
		delete backpack[g];
	}
	backpack.clear();
}

void CHeroWindow::activate()
{
	LOCPLINT->curint = this;
	quitButton->activate();
	dismissButton->activate();
	questlogButton->activate();
	gar1button->activate();
	gar2button->activate();
	gar3button->activate();
	gar4button->activate();
	leftArtRoll->activate();
	rightArtRoll->activate();
	portraitArea->activate();
	for(int g=0; g<heroListMi.size(); ++g)
	{
		heroListMi[g]->activate();
	}
	redrawCurBack();

	if(artFeet)
		artFeet->activate();
	if(artHead)
		artHead->activate();
	if(artLHand)
		artLHand->activate();
	if(artLRing)
		artLRing->activate();
	if(artMach1)
		artMach1->activate();
	if(artMach2)
		artMach2->activate();
	if(artMach3)
		artMach3->activate();
	if(artMach4)
		artMach4->activate();
	if(artMisc1)
		artMisc1->activate();
	if(artMisc2)
		artMisc2->activate();
	if(artMisc3)
		artMisc3->activate();
	if(artMisc4)
		artMisc4->activate();
	if(artMisc5)
		artMisc5->activate();
	if(artNeck)
		artNeck->activate();
	if(artRhand)
		artRhand->activate();
	if(artRRing)
		artRRing->activate();
	if(artShoulders)
		artShoulders->activate();
	if(artSpellBook)
		artSpellBook->activate();
	if(artTorso)
		artTorso->activate();
	for(int f=0; f<backpack.size(); ++f)
	{
		if(backpack[f])
			backpack[f]->activate();
	}

	//LOCPLINT->lclickable.push_back(artFeet);
}

void CHeroWindow::deactivate()
{
	quitButton->deactivate();
	dismissButton->deactivate();
	questlogButton->deactivate();
	gar1button->deactivate();
	gar2button->deactivate();
	gar3button->deactivate();
	gar4button->deactivate();
	leftArtRoll->deactivate();
	rightArtRoll->deactivate();
	portraitArea->deactivate();
	for(int g=0; g<heroListMi.size(); ++g)
	{
		heroListMi[g]->deactivate();
	}

	if(artFeet)
		artFeet->deactivate();
	if(artHead)
		artHead->deactivate();
	if(artLHand)
		artLHand->deactivate();
	if(artLRing)
		artLRing->deactivate();
	if(artMach1)
		artMach1->deactivate();
	if(artMach2)
		artMach2->deactivate();
	if(artMach3)
		artMach3->deactivate();
	if(artMach4)
		artMach4->deactivate();
	if(artMisc1)
		artMisc1->deactivate();
	if(artMisc2)
		artMisc2->deactivate();
	if(artMisc3)
		artMisc3->deactivate();
	if(artMisc4)
		artMisc4->deactivate();
	if(artMisc5)
		artMisc5->deactivate();
	if(artNeck)
		artNeck->deactivate();
	if(artRhand)
		artRhand->deactivate();
	if(artRRing)
		artRRing->deactivate();
	if(artShoulders)
		artShoulders->deactivate();
	if(artSpellBook)
		artSpellBook->deactivate();
	if(artTorso)
		artTorso->deactivate();
	for(int f=0; f<backpack.size(); ++f)
	{		
		if(backpack[f])
			backpack[f]->deactivate();
	}
}

void CHeroWindow::dismissCurrent()
{
}

void CHeroWindow::questlog()
{
}

void CHeroWindow::gar1()
{
}

void CHeroWindow::gar2()
{
}

void CHeroWindow::gar3()
{
}

void CHeroWindow::gar4()
{
}

void CHeroWindow::leftArtRoller()
{
	if(curHero->artifacts.size()>5) //if it is <=5, we have nothing to scroll
	{
		backpackPos+=curHero->artifacts.size()-1; //set new offset

		for(int s=0; s<5 && s<curHero->artifacts.size(); ++s) //set new data
		{
			backpack[s]->ourArt = curHero->artifacts[(s+backpackPos) % curHero->artifacts.size() ];
		}
	}
}

void CHeroWindow::rightArtRoller()
{
	if(curHero->artifacts.size()>5) //if it is <=5, we have nothing to scroll
	{
		backpackPos+=1; //set new offset

		for(int s=0; s<5 && s<curHero->artifacts.size(); ++s) //set new data
		{
			backpack[s]->ourArt = curHero->artifacts[(s+backpackPos) % curHero->artifacts.size() ];
		}
	}
}

void CHeroWindow::switchHero()
{
	//int y;
	//SDL_GetMouseState(NULL, &y);
	//for(int g=0; g<heroListMi.size(); ++g)
	//{
	//	if(y>=94+54*g)
	//	{
	//		//quit();
	//		setHero(LOCPLINT->cb->getHeroInfo(player, g, false));
	//		//LOCPLINT->openHeroWindow(curHero);
	//		redrawCurBack();
	//	}
	//}
}

void CHeroWindow::redrawCurBack()
{
	if(curBack)
		SDL_FreeSurface(curBack);
	curBack = CSDL_Ext::copySurface(background);

	blitAt(skillpics->ourImages[0].bitmap, 32, 111, curBack);
	blitAt(skillpics->ourImages[1].bitmap, 102, 111, curBack);
	blitAt(skillpics->ourImages[2].bitmap, 172, 111, curBack);
	blitAt(skillpics->ourImages[5].bitmap, 242, 111, curBack);
	blitAt(skillpics->ourImages[4].bitmap, 20, 230, curBack);
	blitAt(skillpics->ourImages[3].bitmap, 162, 230, curBack);

	blitAt(curHero->type->portraitLarge, 19, 19, curBack);

	CSDL_Ext::printAtMiddle(curHero->name, 190, 40, GEORXX, tytulowy, curBack);

	std::stringstream secondLine;
	secondLine<<"Level "<<curHero->level<<" "<<curHero->type->heroClass->name;
	CSDL_Ext::printAtMiddle(secondLine.str(), 190, 66, TNRB16, zwykly, curBack);

	//primary skliis names
	CSDL_Ext::printAtMiddle(CGI->generaltexth->jktexts[1], 53, 98, GEOR13, tytulowy, curBack);
	CSDL_Ext::printAtMiddle(CGI->generaltexth->jktexts[2], 123, 98, GEOR13, tytulowy, curBack);
	CSDL_Ext::printAtMiddle(CGI->generaltexth->jktexts[3], 193, 98, GEOR13, tytulowy, curBack);
	CSDL_Ext::printAtMiddle(CGI->generaltexth->jktexts[4], 263, 98, GEOR13, tytulowy, curBack);

	//dismiss / quest log
	std::vector<std::string> * toPrin = CMessage::breakText(CGI->generaltexth->jktexts[8].substr(1, CGI->generaltexth->jktexts[8].size()-2));
	if(toPrin->size()==1)
	{
		CSDL_Ext::printAt((*toPrin)[0], 372, 440, GEOR13, zwykly, curBack);
	}
	else
	{
		CSDL_Ext::printAt((*toPrin)[0], 372, 431, GEOR13, zwykly, curBack);
		CSDL_Ext::printAt((*toPrin)[1], 372, 447, GEOR13, zwykly, curBack);
	}
	delete toPrin;

	toPrin = CMessage::breakText(CGI->generaltexth->jktexts[9].substr(1, CGI->generaltexth->jktexts[9].size()-2));
	if(toPrin->size()==1)
	{
		CSDL_Ext::printAt((*toPrin)[0], 512, 440, GEOR13, zwykly, curBack);
	}
	else
	{
		CSDL_Ext::printAt((*toPrin)[0], 512, 431, GEOR13, zwykly, curBack);
		CSDL_Ext::printAt((*toPrin)[1], 512, 447, GEOR13, zwykly, curBack);
	}
	delete toPrin;

	//printing primary skills' amounts
	std::stringstream primarySkill1;
	primarySkill1<<curHero->primSkills[0];
	CSDL_Ext::printAtMiddle(primarySkill1.str(), 53, 165, TNRB16, zwykly, curBack);

	std::stringstream primarySkill2;
	primarySkill2<<curHero->primSkills[1];
	CSDL_Ext::printAtMiddle(primarySkill2.str(), 123, 165, TNRB16, zwykly, curBack);

	std::stringstream primarySkill3;
	primarySkill3<<curHero->primSkills[2];
	CSDL_Ext::printAtMiddle(primarySkill3.str(), 193, 165, TNRB16, zwykly, curBack);

	std::stringstream primarySkill4;
	primarySkill4<<curHero->primSkills[3];
	CSDL_Ext::printAtMiddle(primarySkill4.str(), 263, 165, TNRB16, zwykly, curBack);

	blitAt(LOCPLINT->luck42->ourImages[curHero->getCurrentLuck()+3].bitmap, 239, 182, curBack);
	blitAt(LOCPLINT->morale42->ourImages[curHero->getCurrentMorale()+3].bitmap, 181, 182, curBack);

	blitAt(flags->ourImages[player].bitmap, 606, 8, curBack);

	//hero list blitting
	for(int g=0; g<LOCPLINT->cb->howManyHeroes(); ++g)
	{
		const CGHeroInstance * cur = LOCPLINT->cb->getHeroInfo(player, g, false);
		blitAt(cur->type->portraitSmall, 611, 87+g*54, curBack);
		//printing yellow border
		if(cur->name == curHero->name)
		{
			for(int f=0; f<cur->type->portraitSmall->w; ++f)
			{
				for(int h=0; h<cur->type->portraitSmall->h; ++h)
					if(f==0 || h==0 || f==cur->type->portraitSmall->w-1 || h==cur->type->portraitSmall->h-1)
					{
						CSDL_Ext::SDL_PutPixel(curBack, 611+f, 87+g*54+h, 240, 220, 120);
					}
			}
		}
	}

	//secondary skills
	if(curHero->secSkills.size()>=1)
	{
		blitAt(CGI->abilh->abils44->ourImages[curHero->secSkills[0].first*3+3+curHero->secSkills[0].second].bitmap, 18, 276, curBack);
		CSDL_Ext::printAt(CGI->abilh->levels[curHero->secSkills[0].second], 69, 279, GEOR13, zwykly, curBack);
		CSDL_Ext::printAt(CGI->abilh->abilities[curHero->secSkills[0].first]->name, 69, 299, GEOR13, zwykly, curBack);
	}
	if(curHero->secSkills.size()>=2)
	{
		blitAt(CGI->abilh->abils44->ourImages[curHero->secSkills[1].first*3+3+curHero->secSkills[1].second].bitmap, 161, 276, curBack);
		CSDL_Ext::printAt(CGI->abilh->levels[curHero->secSkills[1].second], 213, 279, GEOR13, zwykly, curBack);
		CSDL_Ext::printAt(CGI->abilh->abilities[curHero->secSkills[1].first]->name, 213, 299, GEOR13, zwykly, curBack);
	}
	if(curHero->secSkills.size()>=3)
	{
		blitAt(CGI->abilh->abils44->ourImages[curHero->secSkills[2].first*3+3+curHero->secSkills[2].second].bitmap, 18, 324, curBack);
		CSDL_Ext::printAt(CGI->abilh->levels[curHero->secSkills[2].second], 69, 327, GEOR13, zwykly, curBack);
		CSDL_Ext::printAt(CGI->abilh->abilities[curHero->secSkills[2].first]->name, 69, 347, GEOR13, zwykly, curBack);
	}
	if(curHero->secSkills.size()>=4)
	{
		blitAt(CGI->abilh->abils44->ourImages[curHero->secSkills[3].first*3+3+curHero->secSkills[3].second].bitmap, 161, 324, curBack);
		CSDL_Ext::printAt(CGI->abilh->levels[curHero->secSkills[3].second], 213, 327, GEOR13, zwykly, curBack);
		CSDL_Ext::printAt(CGI->abilh->abilities[curHero->secSkills[3].first]->name, 213, 347, GEOR13, zwykly, curBack);
	}
	if(curHero->secSkills.size()>=5)
	{
		blitAt(CGI->abilh->abils44->ourImages[curHero->secSkills[4].first*3+3+curHero->secSkills[4].second].bitmap, 18, 372, curBack);
		CSDL_Ext::printAt(CGI->abilh->levels[curHero->secSkills[4].second], 69, 375, GEOR13, zwykly, curBack);
		CSDL_Ext::printAt(CGI->abilh->abilities[curHero->secSkills[4].first]->name, 69, 395, GEOR13, zwykly, curBack);
	}
	if(curHero->secSkills.size()>=6)
	{
		blitAt(CGI->abilh->abils44->ourImages[curHero->secSkills[5].first*3+3+curHero->secSkills[5].second].bitmap, 161, 372, curBack);
		CSDL_Ext::printAt(CGI->abilh->levels[curHero->secSkills[5].second], 213, 375, GEOR13, zwykly, curBack);
		CSDL_Ext::printAt(CGI->abilh->abilities[curHero->secSkills[5].first]->name, 213, 395, GEOR13, zwykly, curBack);
	}
	if(curHero->secSkills.size()>=7)
	{
		blitAt(CGI->abilh->abils44->ourImages[curHero->secSkills[6].first*3+3+curHero->secSkills[6].second].bitmap, 18, 420, curBack);
		CSDL_Ext::printAt(CGI->abilh->levels[curHero->secSkills[6].second], 69, 423, GEOR13, zwykly, curBack);
		CSDL_Ext::printAt(CGI->abilh->abilities[curHero->secSkills[6].first]->name, 69, 443, GEOR13, zwykly, curBack);
	}
	if(curHero->secSkills.size()>=8)
	{
		blitAt(CGI->abilh->abils44->ourImages[curHero->secSkills[7].first*3+3+curHero->secSkills[7].second].bitmap, 161, 420, curBack);
		CSDL_Ext::printAt(CGI->abilh->levels[curHero->secSkills[7].second], 213, 423, GEOR13, zwykly, curBack);
		CSDL_Ext::printAt(CGI->abilh->abilities[curHero->secSkills[7].first]->name, 213, 443, GEOR13, zwykly, curBack);
	}

	//printing special ability
	blitAt(CGI->heroh->un44->ourImages[curHero->subID].bitmap, 18, 180, curBack);

	//printing necessery texts
	CSDL_Ext::printAt(CGI->generaltexth->jktexts[6].substr(1, CGI->generaltexth->jktexts[6].size()-2), 69, 231, GEOR13, tytulowy, curBack);
	std::stringstream expstr;
	expstr<<curHero->exp;
	CSDL_Ext::printAt(expstr.str(), 69, 247, GEOR16, zwykly, curBack);
	CSDL_Ext::printAt(CGI->generaltexth->jktexts[7].substr(1, CGI->generaltexth->jktexts[7].size()-2), 212, 231, GEOR13, tytulowy, curBack);
	std::stringstream manastr;
	manastr<<curHero->mana<<'/'<<curHero->primSkills[3]*10;
	CSDL_Ext::printAt(manastr.str(), 212, 247, GEOR16, zwykly, curBack);
}

CArtPlace::CArtPlace(CArtifact *art): ourArt(art), active(false){}
void CArtPlace::activate()
{
	if(!active)
	{
		ClickableL::activate();
		active = true;
	}
}
void CArtPlace::clickLeft(boost::logic::tribool down)
{
}
void CArtPlace::deactivate()
{
	if(active)
	{
		active = false;
		ClickableL::deactivate();
	}
}
void CArtPlace::show(SDL_Surface *to)
{
	if(ourArt)
	{
		blitAt(CGI->arth->artDefs->ourImages[ourArt->id].bitmap, pos.x, pos.y, to);
	}
}
CArtPlace::~CArtPlace()
{
	deactivate();
}

void LClickableArea::activate()
{
	ClickableL::activate();
}
void LClickableArea::deactivate()
{
	ClickableL::deactivate();
}
void LClickableArea::clickLeft(boost::logic::tribool down)
{
	if(!down)
	{
		LOCPLINT->showInfoDialog("TEST TEST AAA", std::vector<SComponent*>());
	}
}

void LClickableAreaWText::clickLeft(boost::logic::tribool down)
{
	if(!down)
	{
		LOCPLINT->showInfoDialog(text, std::vector<SComponent*>());
	}
}

void LClickableAreaHero::clickLeft(boost::logic::tribool down)
{
	if(!down)
	{
		const CGHeroInstance * buf = LOCPLINT->cb->getHeroInfo(owner->player, id, false);
		owner->setHero(buf);
		owner->redrawCurBack();
	}
}
