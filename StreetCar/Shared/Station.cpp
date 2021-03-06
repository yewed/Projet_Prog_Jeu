#include "Station.h"
#include <iostream>

using namespace std;

Station::Station(){}

Station::Station(idTile t, int x, int y) : Square(t, x, y){
	if (!isStation()) {
		cout << "ERROR: Constructor Station: Bad idTile " << t << endl;
	}
	linked = false;
}

Station::~Station()
{

}

bool Station::isLinked(){
	return linked;
}

void Station::setOrientation(Orientation o)
{
	linked = true;
	orientation = o;
}

Orientation Station::getOrientation()
{
	return orientation;
}
bool Station::getAccess(Orientation o) {
	return IMPOSSIBLE;
}


bool Station::canChange(Square*s) {
	return false;
}

void Station::print() {
	Square::print();
}

void Station::change(Square *s)
{
	cout << "Error, can't change a station " << endl;
}


ostream& operator << (ostream &f, Station &t){
    f << t.linked << " ";
    f << (int)t.orientation;
    return f;
}
istream& operator >> (istream &f, Station &t){
    f >> t.linked;
    int i;
    f >> i;
    t.orientation = (Orientation)i;
    return f;
}
