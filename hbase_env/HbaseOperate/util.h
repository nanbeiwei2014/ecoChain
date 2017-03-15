#ifndef __HBASE_OPERATE_UTIL_H 
#define __HBASE_OPERATE_UTIL_H 
#include <iostream>     // std::cout
#include <sstream>      // std::istringstream
#include <string>       // std::string

using namespace std;


long stol(std::string str);
std::string ltos(long l);
int stoi(std::string str);
float stof(std::string str);
double stod(std::string str);
std::string itos(int i);
std::string ftos(float f);
std::string dtos(double d);

#endif


