#include "Actor.h"
#include "StudentWorld.h"
using namespace std;
Actor::Actor(int imageID, int x, int y, Direction dir, int depth, int hitpoints, int food, StudentWorld* world, int type)
	: GraphObject(imageID, x, y, dir, depth)
{
	m_hitpoints = hitpoints;
	m_food = food;
	m_world = world;
	m_stun = false;
	m_block = false;
	m_bit = false;
	m_alive = true;
	m_type = type;
	m_done = false;
	m_asleep = 0;
}



int Actor::returnType()
{
	return m_type;
}
StudentWorld* Actor::returnWorld()
{
	return m_world;
}

bool Actor::returnDone() {
	return m_done;
}

int Actor::returnFood() {
	return m_food;
}

bool Actor::returnAlive() {
	return m_alive;
}

void Actor::setAlive(bool alive) {
	m_alive = alive;
}

void Actor::setFood(int food) {
	m_food = food;
}

void Actor::setDone(bool value) {
	m_done = value;
}

int Actor::returnHealth() {
	return m_hitpoints;
}

void Actor::decreaseHealth(int value) {
	m_hitpoints -= value;
}

void Actor::increaseHealth(int value) {
	m_hitpoints += value;
}

void Actor::setHealth(int value) {
	m_hitpoints = value;
}

int Actor::returnAsleep() {
	return m_asleep;
}

void Actor::addAsleep(int value) {
	m_asleep += value;
}

void Actor::decreaseAsleep(int value) {
	m_asleep -= value;
}

bool Actor::returnStunned() {
	return m_stun;
}

void Actor::setStunned(bool value) {
	m_stun = value;
}

bool Actor::canBeBit() {
	if (m_type == 1 || m_type == 4 || m_type == 8) {
		return true;
	}
	return false;
}

int Actor::removeFood(int value) {
	if (value >= returnFood()) {
		int stuff = returnFood();
		setFood(0);
		setAlive(false);
		return stuff;
	}
	else {
		int difference = returnFood() - value;
		setFood(difference);
		return value;
	}
}


Pebble::Pebble(int x, int y, StudentWorld* world)
	:Actor(IID_ROCK, x, y, right, 1, 999, 0, world, 0) {}


void Pebble::doSomething() {}

Food::Food(int x, int y, StudentWorld* world, int startAmount)
	: Actor(IID_FOOD, x, y, right, 2, 9999, startAmount, world, 2) {}

void Food::doSomething() {}



void Food::addFood(int value) {
	setFood(returnFood() + value);
}

Poison::Poison(int x, int y, StudentWorld* world)
	: Actor(IID_POISON, x, y, right, 2, 9999, 0, world, 3) {}

void Poison::doSomething() {
	vector<Actor*> position = returnWorld()->returnActorsHere(getX(), getY());
	for (int i = 0; i < position.size(); i++) {
		if (position[i]->returnType() == 1 || position[i]->returnType() == 8 || position[i]->returnType() == 4) {
			position[i]->decreaseHealth(150);
		}
	}
}

BabyGrasshopper::BabyGrasshopper(int x, int y, StudentWorld* world)
	:Insect(IID_BABY_GRASSHOPPER, x, y, 500, world, 1)
{
	m_distance = randInt(2, 10);
}

void BabyGrasshopper::doSomething()
{
	vector<Actor*> temp = returnWorld()->returnActorsHere(getX(), getY());
	decreaseHealth(1);
	if (returnHealth() <= 0) {
		bool foodHere = false;
		int index = -1;
		for (int i = 0; i < temp.size(); i++) {
			if (temp[i]->returnType() == 2) {
				foodHere = true;
				index = i;
			}
		}
		if (foodHere) {
			temp[index]->setFood(temp[index]->returnFood() + 100);
		}
		else {
			returnWorld()->makeActor(getX(), getY(), 2, 100, -1);
		}
		setAlive(false);
		return;
	}
	if (returnAsleep() > 0) {
		decreaseAsleep(1);
		return;
	}

	if (returnHealth() >= 1600) {
		bool foodHere = false;
		int index = -1;
		for (int i = 0; i < temp.size(); i++) {
			if (temp[i]->returnType() == 2) {
				foodHere = true;
				index = i;
			}
		}
		if (foodHere) {
			temp[index]->setFood(temp[index]->returnFood() + 100);
		}
		else {
			returnWorld()->makeActor(getX(), getY(), 2, 100, -1);
		}
		returnWorld()->makeActor(getX(), getY(), 4, 0, -1);
		setAlive(false);
		return;
	}


	int rand = 0;
	for (int i = 0; i < temp.size(); i++) {
		if (temp[i]->returnType() == 2) {
			int healthIncrease = temp[i]->removeFood(200);
			increaseHealth(healthIncrease);
			rand = randInt(1, 2);
		}
	}
	if (rand == 1) {
		addAsleep(2);
		return;
	}



	Direction dir = getDirection();
	if (m_distance == 0) {
		setDirection((Direction)(randInt(up, left)));
		m_distance = randInt(2, 10);
	}

	if (move(dir)) {
		m_distance--;
	}
	else {
		m_distance = 0;
		addAsleep(2);
		return;
	}
	addAsleep(2);
}

AdultGrasshopper::AdultGrasshopper(int x, int y, StudentWorld* world)
	:Insect(IID_ADULT_GRASSHOPPER, x, y, 1600, world, 4)
{
	m_distance = randInt(2, 10);
}

bool Insect::bite() {
	vector<Actor*> temp = returnWorld()->returnActorsHere(getX(), getY());
	bool bitten = false;
	bool canBiteSomething = false;
	for (int i = 0; i < temp.size(); i++) {
		if (temp[i]->canBeBit() && temp[i] != this) {
			canBiteSomething = true;
		}
	}
	while (!bitten && canBiteSomething) {
		int rand = randInt(0, temp.size() - 1);
		if (temp[rand]->canBeBit()) {
			bitten = true;
			if (this->returnType() == 4) {
				temp[rand]->decreaseHealth(50);
			}
			else if (this->returnType() == 8) {
				temp[rand]->decreaseHealth(15);
			}
			if (temp[rand]->returnType() == 4) {
				int retaliate = randInt(1, 2);
				if (retaliate == 1) {
					temp[rand]->bite();
				}
			}
			else if (temp[rand]->returnType() == 8) {
				Ant* ant1 = dynamic_cast<Ant*>(temp[rand]);
				if (this->returnType() == 8) {
					Ant* ant2 = dynamic_cast<Ant*>(this);
					if (ant1->returnColonyType() == ant2->returnColonyType()) {
						ant1->increaseHealth(15);
						return false;
					}
				}
				ant1->setBitten(true);
			}
		}
	}
	return canBiteSomething;
}

void AdultGrasshopper::doSomething() {
	decreaseHealth(1);
	vector<Actor*> temp = returnWorld()->returnActorsHere(getX(), getY());
	if (returnHealth() <= 0) {
		bool foodHere = false;
		int index = -1;
		for (int i = 0; i < temp.size(); i++) {
			if (temp[i]->returnType() == 2) {
				foodHere = true;
				index = i;
			}
		}
		if (foodHere) {
			temp[index]->setFood(temp[index]->returnFood() + 100);
		}
		else {
			returnWorld()->makeActor(getX(), getY(), 2, 100, -1);
		}
		setAlive(false);
		return;
	}
	if (returnAsleep() > 0) {
		decreaseAsleep(1);
		return;
	}
	int randBite = randInt(1, 3);
	if (randBite == 1) {
		if (bite()) {
			addAsleep(2);
			return;
		}
	}

	int randJump = randInt(1, 10);
	if (randJump == 1) {
		int theta = randInt(0, 360);
		int x = (10.0 * cos(theta));
		int y = (10.0 * sin(theta));
		for (;;) {
			if (returnWorld()->canIMove(getX() + x, getY() + y)) {
				moveTo(getX() + x, getY() + y);
				addAsleep(2);
				return;
			}
			theta = randInt(0, 360);
			x = cos(theta);
			y = sin(theta);
		}
	}


	int rand = 0;
	for (int i = 0; i < temp.size(); i++) {
		if (temp[i]->returnType() == 2) {
			int healthIncrease = temp[i]->removeFood(200);
			increaseHealth(healthIncrease);
			rand = randInt(1, 2);
		}
	}
	if (rand == 1) {
		addAsleep(2);
		return;
	}



	Direction dir = getDirection();
	if (m_distance == 0) {
		setDirection((Direction)(randInt(up, left)));
		m_distance = randInt(2, 10);
	}
	if (move(dir)) {
		m_distance--;
	}
	else {
		m_distance = 0;
		addAsleep(2);
		return;
	}

	addAsleep(2);
}

Pool::Pool(int x, int y, StudentWorld* world)
	:Actor(IID_WATER_POOL, x, y, right, 2, 9999, 0, world, 5) {}

void Pool::doSomething() {
	stun();
}

void Pool::stun() {
	vector<Actor*> temp = returnWorld()->returnActorsHere(getX(), getY());
	for (int i = 0; i < temp.size(); i++) {
		if (temp[i]->canBeBit() && !(temp[i]->returnStunned()) && temp[i]->returnType() != 4) {
			temp[i]->addAsleep(2);
			temp[i]->setStunned(true);
		}
	}
}

Anthill::Anthill(int x, int y, StudentWorld* world, int colonyType, Compiler* c)
	:Actor(IID_ANT_HILL, x, y, right, 2, 8999, 0, world, 7) {
	ant_c = c;
	m_colonyType = colonyType;
}

int Anthill::returnColonyType() {
	return m_colonyType;
}

void Anthill::doSomething() {
	decreaseHealth(1);
	if (returnHealth() <= 0) {
		setAlive(false);
		return;
	}
	vector<Actor*> temp = returnWorld()->returnActorsHere(getX(), getY());
	for (int i = 0; i < temp.size(); i++) {
		if (temp[i]->returnType() == 2) {
			int healthIncrease = temp[i]->removeFood(10000);
			increaseHealth(healthIncrease);
		}
	}
	if (returnHealth() >= 2000) {
		returnWorld()->makeAnt(getX(), getY(), m_colonyType, ant_c);
		decreaseHealth(1500);
		if (m_colonyType == 0) {
			returnWorld()->addAnt0(1);
		}
		else if (m_colonyType == 1) {
			returnWorld()->addAnt1(1);
		}
		else if (m_colonyType == 2) {
			returnWorld()->addAnt2(1);
		}
		else if (m_colonyType == 3) {
			returnWorld()->addAnt3(1);
		}
	}
}

Pheremone::Pheremone(int x, int y, StudentWorld* world, int colonyType)
	: Actor(returnID(colonyType), x, y, right, 2, 256, 0, world, 6) {
	m_colonyType = colonyType;
}

int Pheremone::returnID(int colonyType) {
	if (colonyType == 0) {
		return IID_PHEROMONE_TYPE0;
	}
	else if (colonyType == 1) {
		return IID_PHEROMONE_TYPE1;
	}
	else if (colonyType == 2) {
		return IID_PHEROMONE_TYPE2;
	}
	else if (colonyType == 3) {
		return IID_PHEROMONE_TYPE3;
	}
	else {
		return -1;
	}
}

void Pheremone::doSomething() {
	decreaseHealth(1);
	if (returnHealth() <= 0) {
		setAlive(false);
	}
}

int Pheremone::returnColonyType() {
	return m_colonyType;
}

Ant::Ant(int x, int y, StudentWorld* world, int colonyType, Compiler* c)
	:Insect(returnID(colonyType), x, y, 1500, world, 8) {
	m_colonyType = colonyType;
	instructions = c;
	was_bitten = false;
	was_blocked = false;
	lastRand = 0;
	instructionCount = 0;
}

void Ant::doSomething() {
	decreaseHealth(1);
	vector<Actor*> temp = returnWorld()->returnActorsHere(getX(), getY());
	if (returnHealth() <= 0) {
		bool foodHere = false;
		int index = -1;
		for (int i = 0; i < temp.size(); i++) {
			if (temp[i]->returnType() == 2) {
				foodHere = true;
				index = i;
			}
		}
		if (foodHere) {
			temp[index]->setFood(temp[index]->returnFood() + 100);
		}
		else {
			returnWorld()->makeActor(getX(), getY(), 2, 100, m_colonyType);
		}
		setAlive(false);
		return;
	}

	if (returnAsleep() > 0) {
		decreaseAsleep(1);
		return;
	}

	Compiler::Command cmd;
	for (int numCommands = 0; numCommands < 10; numCommands++) {
		if (!instructions->getCommand(instructionCount, cmd)) {
			setAlive(false);
			return;
		}

		switch (cmd.opcode) {
		case Compiler::moveForward: {
			Direction dir = getDirection();
			if (move(dir)) {
				was_bitten = false;
				was_blocked = false;
			}
			else {
				was_blocked = true;
			}
			instructionCount++;
			return;
			break;
		}

		case Compiler::eatFood: {
			if (100 > returnFood()) {
				increaseHealth(returnFood());
				setFood(0);
			}
			else {
				increaseHealth(returnFood() - 100);
				setFood(returnFood() - 100);
			}
			instructionCount++;
			return;
			break;
		}

		case Compiler::dropFood: {
			bool foodHere = false;
			int index = -1;
			for (int i = 0; i < temp.size(); i++) {
				if (temp[i]->returnType() == 2) {
					foodHere = true;
					index = i;
				}
			}
			if (foodHere) {
				temp[index]->setFood(temp[index]->returnFood() + returnFood());
			}
			else if (returnFood() > 0) {
				returnWorld()->makeActor(getX(), getY(), 2, returnFood(), m_colonyType);
			}
			setFood(0);
			instructionCount++;
			return;
			break;
		}

		case Compiler::bite: {
			bite();
			instructionCount++;
			return;
			break;
		}

		case Compiler::pickupFood: {
			bool canPick = false;
			int index = -1;
			for (int i = 0; i < temp.size(); i++) {
				if (temp[i]->returnType() == 2) {
					canPick = true;
					index = i;
				}
			}
			if (canPick) {
				int add = 0;
				if (returnFood() > 1200) {
					add = 1600 - returnFood();
					add = temp[index]->removeFood(add);
				}
				else if (returnFood() < 1600) {
					add = temp[index]->removeFood(400);
				}
				setFood(returnFood() + add);
			}
			instructionCount++;
			return;
			break;
		}

		case Compiler::emitPheromone: {
			bool pheremoneHere = false;
			int index = -1;
			for (int i = 0; i < temp.size(); i++) {
				if (temp[i]->returnType() == 6) {
					Pheremone* thing = dynamic_cast<Pheremone*>(temp[i]);
					if (thing->returnColonyType() == m_colonyType) {
						pheremoneHere = true;
						index = i;
					}
				}
			}
			if (pheremoneHere) {
				if (temp[index]->returnHealth() > 512) {
					temp[index]->setHealth(768);
				}
				else {
					temp[index]->increaseHealth(256);
				}
			}
			else {
				returnWorld()->makeActor(getX(), getY(), 6, returnFood(), m_colonyType);
			}
			instructionCount++;
			return;
			break;
		}

		case Compiler::faceRandomDirection: {
			setDirection((Direction)(randInt(up, left)));
			instructionCount++;
			return;
			break;
		}

		case Compiler::rotateClockwise: {
			Direction dir = getDirection();
			if (dir == up) {
				setDirection(right);
			}
			else if (dir == right) {
				setDirection(down);
			}
			else if (dir == down) {
				setDirection(left);
			}
			else if (dir == left) {
				setDirection(up);
			}
			instructionCount++;
			return;
			break;
		}

		case Compiler::rotateCounterClockwise: {
			Direction dir = getDirection();
			if (dir == up) {
				setDirection(left);
			}
			else if (dir == right) {
				setDirection(up);
			}
			else if (dir == down) {
				setDirection(right);
			}
			else if (dir == left) {
				setDirection(down);
			}
			instructionCount++;
			return;
			break;
		}

		case Compiler::generateRandomNumber: {
			int max = stoi(cmd.operand1);
			if (max == 0) {
				lastRand = 0;
			}
			else {
				lastRand = randInt(0, max - 1);
			}
			instructionCount++;
			break;
		}

		case Compiler::goto_command: {
			instructionCount = stoi(cmd.operand1);
			break;
		}

		case Compiler::if_command: {
			switch (stoi(cmd.operand1)) {
			case Compiler::last_random_number_was_zero: {
				if (lastRand == 0) {
					instructionCount = stoi(cmd.operand2);
				}
				else {
					instructionCount++;
				}
				break;
			}

			case Compiler::i_am_carrying_food: {
				if (returnFood() > 0) {
					instructionCount = stoi(cmd.operand2);
				}
				else {
					instructionCount++;
				}
				break;
			}

			case Compiler::i_am_hungry: {
				if (returnHealth() <= 25) {
					instructionCount = stoi(cmd.operand2);
				}
				else {
					instructionCount++;
				}
				break;
			}

			case Compiler::i_am_standing_with_an_enemy: {
				bool dangerHere = false;
				for (int i = 0; i < temp.size(); i++) {
					if (temp[i]->returnType() == 1 || temp[i]->returnType() == 4) {
						dangerHere = true;
					}
					else if (temp[i]->returnType() == 8) {
						Ant* enemy = dynamic_cast<Ant*>(temp[i]);
						if (enemy->m_colonyType != this->m_colonyType) {
							dangerHere = true;
						}
					}
				}
				if (dangerHere) {
					instructionCount = stoi(cmd.operand2);
				}
				else {
					instructionCount++;
				}
				break;
			}
			case Compiler::i_am_standing_on_food: {
				bool foodHere = false;
				for (int i = 0; i < temp.size(); i++) {
					if (temp[i]->returnType() == 2) {
						foodHere = true;
					}
				}
				if (foodHere) {
					instructionCount = stoi(cmd.operand2);
				}
				else {
					instructionCount++;
				}
				break;
			}
			case Compiler::i_am_standing_on_my_anthill: {
				bool onAnthill = false;
				for (int i = 0; i < temp.size(); i++) {
					if (temp[i]->returnType() == 7) {
						Anthill* on = dynamic_cast<Anthill*>(temp[i]);
						if (on->returnColonyType() == m_colonyType) {
							onAnthill = true;
						}
					}
				}
				if (onAnthill) {
					instructionCount = stoi(cmd.operand2);
				}
				else {
					instructionCount++;
				}
				break;
			}

			case Compiler::i_smell_pheromone_in_front_of_me: {
				bool phereHere = false;
				vector<Actor*> inFront;
				if (getDirection() == up) {
					inFront = returnWorld()->returnActorsHere(getX(), getY() + 1);
				}
				else if (getDirection() == down) {
					inFront = returnWorld()->returnActorsHere(getX(), getY() - 1);
				}
				else if (getDirection() == right) {
					inFront = returnWorld()->returnActorsHere(getX() + 1, getY());
				}
				else if (getDirection() == left) {
					inFront = returnWorld()->returnActorsHere(getX() - 1, getY());
				}
				else {
					instructionCount++;
					break;
				}
				for (int i = 0; i < inFront.size(); i++) {
					if (inFront[i]->returnType() == 6) {
						phereHere = true;
					}
				}
				if (phereHere) {
					instructionCount = stoi(cmd.operand2);
				}
				else {
					instructionCount++;
				}
				break;
			}

			case Compiler::i_smell_danger_in_front_of_me: {
				vector<Actor*> inFront;
				if (getDirection() == up) {
					inFront = returnWorld()->returnActorsHere(getX(), getY() + 1);
				}
				else if (getDirection() == down) {
					inFront = returnWorld()->returnActorsHere(getX(), getY() - 1);
				}
				else if (getDirection() == right) {
					inFront = returnWorld()->returnActorsHere(getX() + 1, getY());
				}
				else if (getDirection() == left) {
					inFront = returnWorld()->returnActorsHere(getX() - 1, getY());
				}
				else {
					instructionCount++;
					break;
				}
				bool dangerInFront = false;
				for (int i = 0; i < inFront.size(); i++) {
					if (inFront[i]->returnType() == 1 || inFront[i]->returnType() == 4 || inFront[i]->returnType() == 5 || inFront[i]->returnType() == 3) {
						dangerInFront = true;
					}
					else if (inFront[i]->returnType() == 8) {
						Ant* enemy = dynamic_cast<Ant*>(inFront[i]);
						if (enemy->m_colonyType != this->m_colonyType) {
							dangerInFront = true;
						}
					}
				}
				if (dangerInFront) {
					instructionCount = stoi(cmd.operand2);
				}
				else {
					instructionCount++;
				}
				break;
			}

			case Compiler::i_was_bit: {
				if (was_bitten) {
					instructionCount = stoi(cmd.operand2);
				}
				else {
					instructionCount++;
				}
				break;
			}
			case Compiler::i_was_blocked_from_moving: {
				if (was_blocked) {
					instructionCount = stoi(cmd.operand2);
				}
				else {
					instructionCount++;
				}
				break;
			}
			}
		}
		}
	}
}

int Ant::returnColonyType() {
	return m_colonyType;
}

int returnID(int colonyType) {
	if (colonyType == 0) {
		return IID_ANT_TYPE0;
	}
	else if (colonyType == 1) {
		return IID_ANT_TYPE1;
	}
	else if (colonyType == 2) {
		return IID_ANT_TYPE2;
	}
	else if (colonyType == 3) {
		return IID_ANT_TYPE3;
	}
}

void Ant::setBitten(bool value) {
	was_bitten = value;
}

Insect::Insect(int imageID, int x, int y, int hitpoints, StudentWorld* world, int type)
	:Actor(imageID, x, y, (Direction)(randInt(up, left)), 1, hitpoints, 0, world, type)
{}

bool Insect::move(Direction dir) {
	if (dir == up) {
		if (returnWorld()->canIMove(getX(), getY() + 1)) {
			moveTo(getX(), getY() + 1);
			setStunned(false);
		}
		else {
			return false;
		}
	}
	if (dir == down) {
		if (returnWorld()->canIMove(getX(), getY() - 1)) {
			moveTo(getX(), getY() - 1);
			setStunned(false);
		}
		else {
			return false;
		}
	}
	if (dir == left) {
		if (returnWorld()->canIMove(getX() - 1, getY())) {
			moveTo(getX() - 1, getY());
			setStunned(false);
		}
		else {
			return false;
		}
	}
	if (dir == right) {
		if (returnWorld()->canIMove(getX() + 1, getY())) {
			moveTo(getX() + 1, getY());
			setStunned(false);
		}
		else {
			return false;
		}
		return true;
	}
	return false;
}