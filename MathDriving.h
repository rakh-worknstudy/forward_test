#pragma once

class Car {
public:
	Car(void);
	virtual ~Car(void);
protected:
private:
	int posX;
	int posY;
};

class Road : protected Car {
public:
	Road(void);
	virtual ~Road(void);
private:

};

