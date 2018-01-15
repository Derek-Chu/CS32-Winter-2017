#ifndef ACTOR_H_
#define ACTOR_H_

class StudentWorld;
#include "GraphObject.h"
#include <vector>
#include "Compiler.h"
#include <cmath>

class Actor : public GraphObject{
public:
	Actor(int imageID, int x, int y, Direction dir, int depth, int hitpoints, int food, StudentWorld* world, int type);
	virtual void doSomething() = 0;
	int returnType();
	int returnFood();
	bool returnAlive();
	void setAlive(bool alive);
	void setFood(int food);
	StudentWorld* returnWorld();
	bool returnDone();
	void setDone(bool value);
	int returnHealth();
	void decreaseHealth(int value);
	void increaseHealth(int value);
	void setHealth(int value);
	int returnAsleep();
	void addAsleep(int value);
	void decreaseAsleep(int value);
	int removeFood(int value);
	bool returnStunned();
	void setStunned(bool value);
	bool canBeBit();
	virtual bool bite() { return false; }

private: 
	int m_hitpoints;
	int m_food;
	int m_pheromone;
	int m_asleep;
	bool m_stun;
	bool m_block;
	bool m_bit;
	bool m_alive;
	int m_type;
	StudentWorld* m_world;
	bool m_done;
};

class Insect : public Actor {
public:
	Insect(int imageID, int x, int y, int hitpoints, StudentWorld* world, int type);
	bool move(Direction dir);
	bool bite();
	virtual void doSomething() = 0;

};


class Pebble : public Actor{
public:
	Pebble(int x, int y, StudentWorld* world);
	virtual void doSomething();
};

class Food : public Actor {
public:
	Food(int x, int y, StudentWorld* world, int startAmount);
	void addFood(int value);
	virtual void doSomething();
};


class Poison : public Actor {
public:
	Poison(int x, int y, StudentWorld* world);
	virtual void doSomething();
};

class BabyGrasshopper : public Insect{
public:
	BabyGrasshopper(int x, int y, StudentWorld* world);
	virtual void doSomething();
private:
	int m_distance;
};

class AdultGrasshopper : public Insect {
public:
	AdultGrasshopper(int x, int y, StudentWorld* world);
	virtual void doSomething();
private:
	int m_distance;
};

class Pool : public Actor {
public:
	Pool(int x, int y, StudentWorld* world);
	virtual void doSomething();
	void stun();
};

class Anthill : public Actor {
public:
	Anthill(int x, int y, StudentWorld* world, int colonyType, Compiler* c);
	int returnColonyType();
	virtual void doSomething();
private:
	Compiler* ant_c;
	int m_colonyType;
};

class Pheremone : public Actor {
public:
	Pheremone(int x, int y, StudentWorld* world, int colonyType);
	virtual void doSomething();
	int returnColonyType();
	int returnID(int colonyType);
private:
	int m_colonyType;
};

class Ant : public Insect {
public:
	Ant(int x, int y, StudentWorld* world, int colonyType, Compiler* c);
	virtual void doSomething();
	void setBitten(bool value);
	int returnColonyType();

private:
	Compiler* instructions;
	bool was_bitten;
	bool was_blocked;
	int m_colonyType;
	int lastRand;
	int instructionCount;
};
int returnID(int colonyType);



#endif // ACTOR_H_