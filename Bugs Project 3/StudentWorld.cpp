#include "StudentWorld.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

StudentWorld::~StudentWorld() {
	cleanUp();
}

int StudentWorld::init()
{
	hasCleanedUp = false;
	m_ticks = 0;
	Field f;
	string fieldFile = getFieldFilename();
	string error;

	if (f.loadField(fieldFile, error) != Field::LoadResult::load_success)
	{
		setError(fieldFile + " " + error);
		return GWSTATUS_LEVEL_ERROR; 
	}


	compilerForEntrant0 = new Compiler;
	compilerForEntrant1 = new Compiler;
	compilerForEntrant2 = new Compiler;
	compilerForEntrant3 = new Compiler;

	vector<string> fileNames = getFilenamesOfAntPrograms();
	int numPlayers = fileNames.size();


	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			Field::FieldItem item = f.getContentsOf(i, j); 
			if (item == Field::FieldItem::rock)
			{
				Pebble* temp = new Pebble(i, j, this);
				m_actors[i][j].push_back(temp);
			}
			else if (item == Field::FieldItem::grasshopper)
			{
				BabyGrasshopper* temp = new BabyGrasshopper(i, j, this);
				m_actors[i][j].push_back(temp);
			}
			else if (item == Field::FieldItem::food) {
				Food* temp = new Food(i, j, this, 6000);
				m_actors[i][j].push_back(temp);
			}
			else if (item == Field::FieldItem::poison) {
				Poison* temp = new Poison(i, j, this);
				m_actors[i][j].push_back(temp);
			}
			else if (item == Field::FieldItem::water) {
				Pool* temp = new Pool(i, j, this);
				m_actors[i][j].push_back(temp);
			}
			else if (item == Field::FieldItem::anthill0) {
				if (0 < fileNames.size()) {
					if (!compilerForEntrant0->compile(fileNames[0], error)) {
						setError(fileNames[0] + " " + error);
						return GWSTATUS_LEVEL_ERROR;
					}
					a0 = compilerForEntrant0->getColonyName();
					Anthill* a0 = new Anthill(i, j, this, 0, compilerForEntrant0);
					m_actors[i][j].push_back(a0);
				}
			}
			else if (item == Field::FieldItem::anthill1) {
				if (1 < fileNames.size()) {
					if (!compilerForEntrant1->compile(fileNames[1], error)) {
						setError(fileNames[1] + " " + error);
						return GWSTATUS_LEVEL_ERROR;
					}
					a1 = compilerForEntrant1->getColonyName();
					Anthill* a1 = new Anthill(i, j, this, 1, compilerForEntrant1);
					m_actors[i][j].push_back(a1);
				}
			}
			else if (item == Field::FieldItem::anthill2) {
				if (2 < fileNames.size()) {
					if (!compilerForEntrant2->compile(fileNames[2], error)) {
						setError(fileNames[2] + " " + error);
						return GWSTATUS_LEVEL_ERROR;
					}
					a2 = compilerForEntrant2->getColonyName();
					Anthill* a2 = new Anthill(i, j, this, 2, compilerForEntrant2);
					m_actors[i][j].push_back(a2);
				}
			}
			else if (item == Field::FieldItem::anthill3) {
				if (3 < fileNames.size()) {
					if (!compilerForEntrant3->compile(fileNames[3], error)) {
						setError(fileNames[3] + " " + error);
						return GWSTATUS_LEVEL_ERROR;
					}
					a3 = compilerForEntrant3->getColonyName();
					Anthill* a3 = new Anthill(i, j, this, 3, compilerForEntrant3);
					m_actors[i][j].push_back(a3);
				}
			}
		}
	}
	
	return GWSTATUS_CONTINUE_GAME;
}
int StudentWorld::move()
{
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			for (int k = 0; k < m_actors[i][j].size(); k++) {
				m_actors[i][j][k]->setDone(false);
			}
		}
	}
	m_ticks++;
	for (int i = 0; i < 64; i++){
		for (int j = 0; j < 64; j++){
			for (int k = 0; k < m_actors[i][j].size(); k++){
				if (m_actors[i][j][k] != nullptr && !m_actors[i][j][k]->returnDone()){
					int x = m_actors[i][j][k]->getX();
					int y = m_actors[i][j][k]->getY();
					
					m_actors[i][j][k]->doSomething();
					int x1 = m_actors[i][j][k]->getX();
					int y1 = m_actors[i][j][k]->getY();
					if (x != x1 || y != y1)
					{
						Actor* temp = m_actors[i][j][k];
						vector<Actor*>::iterator iter = m_actors[i][j].begin();
						for (int l = 0; l < k; l++)
						{
							iter++;
						}
						m_actors[i][j].erase(iter);
						k--;
						m_actors[x1][y1].push_back(temp);
						temp->setDone(true);
					}
					else {
						m_actors[i][j][k]->setDone(true);
					}
				}
			}
		}
	}
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			for (int k = 0; k < m_actors[i][j].size(); k++) {
				if (!m_actors[i][j][k]->returnAlive()) {
					Actor* temp = m_actors[i][j][k];
					vector<Actor*>::iterator it = m_actors[i][j].begin();
					for (int l = 0; l < k; l++) {
						it++;
					}
					m_actors[i][j].erase(it);
					delete temp;
					k--;
				}
			}
		}
	}

	if (ant0production > currentMaxAnts) {
		currentMaxAnts = ant0production;
		newWinner(a0);
	}
	else if (ant1production > currentMaxAnts) {
		currentMaxAnts = ant1production;
		newWinner(a1);
	}
	else if (ant2production > currentMaxAnts) {
		currentMaxAnts = ant2production;
		newWinner(a2);
	}
	else if (ant3production > currentMaxAnts) {
		currentMaxAnts = ant3production;
		newWinner(a3);
	}

	setGameStatText(makeText());

	if (m_ticks >= 2000)
	{
		if (weHaveAWinner) {
			return GWSTATUS_PLAYER_WON;
		}
		else {
			return GWSTATUS_NO_WINNER;
		}
	}
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp() {
	if (!hasCleanedUp) {
		for (int i = 0; i < 64; i++)
		{
			for (int j = 0; j < 64; j++)
			{
				for (int k = 0; k < m_actors[i][j].size(); k++)
				{
					delete m_actors[i][j][k];
				}
				m_actors[i][j].clear();
			}
		}
		delete compilerForEntrant0;
		delete compilerForEntrant1;
		delete compilerForEntrant2;
		delete compilerForEntrant3;

		if (hasCleanedUp == false) {
			hasCleanedUp = true;
		}
	}
}

vector<Actor*> StudentWorld::returnActorsHere(int x, int y) {
	return m_actors[x][y];
}

void StudentWorld::makeActor(int x, int y, int type, int amount, int colonyType) {
	if (type == 2) {
		Food* temp = new Food(x, y, this, amount);
		m_actors[x][y].push_back(temp);
	}
	else if (type == 4) {
		AdultGrasshopper* temp = new AdultGrasshopper(x, y, this);
		m_actors[x][y].push_back(temp);
	}
	else if (type == 6) {
		Pheremone* temp = new Pheremone(x, y, this, colonyType);
		m_actors[x][y].push_back(temp);
	}
	else {
		return;
	}
}

void StudentWorld::makeAnt(int x, int y, int colonyType, Compiler* c) {
	Ant* temp = new Ant(x, y, this, colonyType, c);
	m_actors[x][y].push_back(temp);
}

bool StudentWorld::canIMove(int x, int y)
{
	if (x < 0 || y < 0 || x >= 64 || y >= 64){
		return false;
	}

	for (int k = 0; k < m_actors[x][y].size(); k++){
		if (m_actors[x][y][k]->returnType() == 0){
			return false;
		}
	}
	return true;
}

void StudentWorld::addAnt0(int amount) {
	ant0production += amount;
}

void StudentWorld::addAnt1(int amount) {
	ant1production += amount;
}

void StudentWorld::addAnt2(int amount) {
	ant2production += amount;
}

void StudentWorld::addAnt3(int amount) {
	ant3production += amount;
}

string StudentWorld::makeText() {
	ostringstream oss;
	oss << setw(5) << to_string(2000 - m_ticks);
	string justifiedTicks = oss.str();

	string s0, s1, s2, s3;
	if (ant0production < 10) {
		s0 = "0" + to_string(ant0production);
	}
	else {
		s0 = to_string(ant0production);
	}

	if (ant1production < 10) {
		s1 = "0" + to_string(ant1production);
	}
	else {
		s1 = to_string(ant1production);
	}

	if (ant2production < 10) {
		s2 = "0" + to_string(ant2production);
	}
	else {
		s2 = to_string(ant2production);
	}

	if (ant3production < 10) {
		s3 = "0" + to_string(ant3production);
	}
	else {
		s3 = to_string(ant3production);
	}
	if (a1 == "") {
		if (getWinnerName() == a0) {
			return "Ticks:" + justifiedTicks + " - " + a0 + "*: " + s0 + " ants";
		}
		else {
			return "Ticks:" + justifiedTicks + " - " + a0 + ": " + s0 + " ants";
		}
	}

	else if (a2 == "") {
		if (getWinnerName() == a0) {
			return "Ticks:" + justifiedTicks + " - " + a0 + "*: " + s0 + " ants  " + a1 + ": " + s1 + " ants";
		}
		else if (getWinnerName() == a1) {
			return "Ticks:" + justifiedTicks + " - " + a0 + ": " + s0 + " ants  " + a1 + "*: " + s1 + " ants";
		}
		else {
			return "Ticks:" + justifiedTicks + " - " + a0 + ": " + s0 + " ants  " + a1 + ": " + s1 + " ants";
		}
	}

	else if (a3 == "") {
		if (getWinnerName() == a0) {
			return "Ticks:" + justifiedTicks + " - " + a0 + "*: " + s0 + " ants  " + a1 + ": " + s1 + " ants  " + a2 + ": " + s2 + " ants";
		}
		else if (getWinnerName() == a1) {
			return "Ticks:" + justifiedTicks + " - " + a0 + ": " + s0 + " ants  " + a1 + "*: " + s1 + " ants  " + a2 + ": " + s2 + " ants";
		}
		else if (getWinnerName() == a2) {
			return "Ticks:" + justifiedTicks + " - " + a0 + ": " + s0 + " ants  " + a1 + ": " + s1 + " ants  " + a2 + "*: " + s2 + " ants";
		}
		else {
			return "Ticks:" + justifiedTicks + " - " + a0 + ": " + s0 + " ants  " + a1 + ": " + s1 + " ants  " + a2 + ": " + s2 + " ants";
		}
	}

	else {
		if (getWinnerName() == a0) {
			return "Ticks:" + justifiedTicks + " - " + a0 + "*: " + s0 + " ants  " + a1 + ": " + s1 + " ants  " + a2 + ": " + s2 + " ants  " + a3 + ": " + s3 + " ants";
		}
		else if (getWinnerName() == a1) {
			return "Ticks:" + justifiedTicks + " - " + a0 + ": " + s0 + " ants  " + a1 + "*: " + s1 + " ants  " + a2 + ": " + s2 + " ants  " + a3 + ": " + s3 + " ants";
		}
		else if (getWinnerName() == a2) {
			return "Ticks:" + justifiedTicks + " - " + a0 + ": " + s0 + " ants  " + a1 + ": " + s1 + " ants  " + a2 + "*: " + s2 + " ants  " + a3 + ": " + s3 + " ants";
		}
		else if (getWinnerName() == a3) {
			return "Ticks:" + justifiedTicks + " - " + a0 + ": " + s0 + " ants  " + a1 + ": " + s1 + " ants  " + a2 + ": " + s2 + " ants  " + a3 + "*: " + s3 + " ants";
		}
		else {
			return "Ticks:" + justifiedTicks + " - " + a0 + ": " + s0 + " ants  " + a1 + ": " + s1 + " ants  " + a2 + ": " + s2 + " ants  " + a3 + ": " + s3 + " ants";
		}
	}
}

void StudentWorld::newWinner(string s) {
	weHaveAWinner = true;
	setWinner(s);
}