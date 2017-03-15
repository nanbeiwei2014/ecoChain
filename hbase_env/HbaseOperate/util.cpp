#ifndef __HBASE_OPERATE_UTIL_H 
#define __HBASE_OPERATE_UTIL_H 
  
#include <iostream>     // std::cout
#include <sstream>      // std::istringstream
#include <string>       // std::string
#include "util.h"
using namespace std;

long stol(std::string str)
{
    long result;
    std::istringstream is(str);
    is >> result;
    return result;
}

std::string ltos(long l)
{
    std::ostringstream os;
    os<<l;
    std::string result;
    std::istringstream is(os.str());
    is>>result;
    return result;

}

int stoi(std::string str)
{
    int result;
    std::istringstream is(str);
    is >> result;
    return result;
}

float stof(std::string str)
{
    float result;
    std::istringstream is(str);
    is >> result;
    return result;
}

double stod(std::string str)
{
    double result;
    std::istringstream is(str);
    is >> result;
    return result;
}

std::string itos(int i)
{
    std::ostringstream os;
    os<<i;
    std::string result;
    std::istringstream is(os.str());
    is>>result;
    return result;

}

std::string ftos(float f)
{
    std::ostringstream os;
    os<<f;
    std::string result;
    std::istringstream is(os.str());
    is>>result;
    return result;

}


std::string dtos(double d)
{
    std::ostringstream os;
    os<<d;
    std::string result;
    std::istringstream is(os.str());
    is>>result;
    return result;

}



#endif


