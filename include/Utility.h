#include<string>
#include<vector>

#ifndef UTILITY_H
#define UTILITY_H
using namespace std;
class Utility
{
    public:
        Utility();
        static vector<string> split(const string& s, const string& delim);

};

#endif // UTILITY_H
