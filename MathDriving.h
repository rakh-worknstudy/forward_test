#pragma once

class Movement {
protected:
	Movement(void);
	virtual ~Movement(void);
private:
	float speed;
	float speedAngle;
	float accel;
	float accelAngle;
};

class Car : protected Movement{
public:
	Car(void);
	virtual ~Car(void);
protected:
private:
	float position;
	float angle;
};

class Road {
public:
	Road(void);
	virtual ~Road(void);
private:

};

