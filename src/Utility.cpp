#include "Utility.h"

Utility::Utility()
{
    //ctor
}

vector<string> Utility::split(const string& s, const string& delim)
{
    vector<string> splitted;
    string temp = s;

    while(temp.find(delim) != temp.npos)
    {
        int pos = temp.find(delim);
        splitted.push_back(temp.substr(0,pos));
        temp = temp.erase(0,pos+delim.length());
    }

    splitted.push_back(temp);
    return splitted;
}
