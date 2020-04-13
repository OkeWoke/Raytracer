#include<string>
#include<vector>

#ifndef UTILITY_H
#define UTILITY_H
using namespace std;

extern uint64_t numRayTrianglesTests;
extern uint64_t numRayTrianglesIsect;
extern uint64_t numPrimaryRays;

class Utility
{
    public:
        Utility();
        static vector<string> split(const string& s, const string& delim);

};

#endif // UTILITY_H
