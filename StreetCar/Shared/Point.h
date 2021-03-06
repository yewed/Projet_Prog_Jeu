#ifndef POINT_H
#define POINT_H
#include <fstream>


struct Point{
	int x;
	int y;

	Point &operator = (Point const&p){
		x = p.x;
		y = p.y;
		return *this;
	}
};


typedef struct Point Point;

bool operator != (Point p1, Point p2);
bool operator == (Point &p1, Point &p2);

std::ostream& operator << (std::ostream &f, Point &s);
std::istream& operator >> (std::istream &f, Point &s);



#endif
