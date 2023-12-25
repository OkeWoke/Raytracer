#include "Utility.h"

Utility::Utility()
{
    //ctor
}

std::vector<std::string> Utility::split(const std::string& s, const std::string& delim)
{
    std::vector<std::string> splitted;
    std::string temp = s;

    while(temp.find(delim) != temp.npos)
    {
        int pos = temp.find(delim);
        splitted.push_back(temp.substr(0,pos));
        temp = temp.erase(0,pos+delim.length());
    }

    splitted.push_back(temp);
    return splitted;
}
