#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include "Actor.h"
#include <vector>
#include "Compiler.h"
#include "Field.h"

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
		:GameWorld(assetDir)
	{
		m_ticks = 0;
		ant0production = 0;
		ant1production = 0;
		ant2production = 0;
		ant3production = 0;
		weHaveAWinner = false;
		currentMaxAnts = 5;
	}
	~StudentWorld();

	virtual int init();
	virtual int move();
	virtual void cleanUp();
	bool canIMove(int x, int y);
	std::vector<Actor*> returnActorsHere(int x, int y);
	void makeActor(int x, int y, int type, int amount, int colonyType);
	void makeAnt(int x, int y, int colonyType, Compiler* c);
	void addAnt0(int amount);
	void addAnt1(int amount);
	void addAnt2(int amount);
	void addAnt3(int amount);
	std::string makeText();
	void newWinner(std::string s);

private:
	std::vector<Actor*>  m_actors[VIEW_WIDTH][VIEW_HEIGHT];
	int m_ticks;
	int ant0production;
	int ant1production;
	int ant2production;
	int ant3production;
	std::string a0, a1, a2, a3;
	bool weHaveAWinner;
	int currentMaxAnts;
	Compiler *compilerForEntrant0, *compilerForEntrant1,
			 *compilerForEntrant2, *compilerForEntrant3;
	bool hasCleanedUp;
};

#endif // STUDENTWORLD_H_
