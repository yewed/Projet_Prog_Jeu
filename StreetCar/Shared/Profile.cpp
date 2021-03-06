#include "Profile.h"

using namespace std;

Profile::Profile(){
	avatar = 0;
	name = "";
	type = 0;
}

Profile::Profile(const Profile& p){
    this->avatar = p.avatar;
    this->name = p.name;
    this->type = p.type;
}

Profile::Profile(std::string n, int a, int t){
    avatar = a;
    name = n;
	type = t;
}

Profile Profile::operator = (Profile p){
    avatar = p.avatar;
    name = p.name;
	type = p.type;
    return *this;
}

ostream& operator << (ostream &f, Profile &p){
    f << p.name << " ";
    f << p.avatar << " ";
    f << p.type;
    return f;
}
istream& operator >> (istream &f, Profile &p){
    f >> p.name;
    f >> p.avatar;
    f >> p.type;
    return f;
}
