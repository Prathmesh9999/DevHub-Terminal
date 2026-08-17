#include"Config.hpp"

#include<string>
#include<fstream>
// #include<sstream>
// #include<stdexcept>
// #include<cctype>

bool Config::load(const std::string& filename){
    std::ifstream file(filename);
    if(!file.is_open())return false;

    std::string line;
    std::string currentSection;

    while(std::getline(file,line)){
        if(line.empty()||line[0]=='#')continue;

        if(line.front()=='['&& line.back()==']'){
            currentSection=line.substr(1,line.size()-2);
            continue;
        }
        std::size_t seperator =line.find('=');
        if(seperator==std::string::npos)continue;

        std::string key=line.substr(0,seperator);
        std::string value=line.substr(seperator+1);

        data[currentSection][key]=value;

    }
    return true;
}

std::string Config::get(const std::string& section,const std::string& key)const{
    auto sectionIterator=data.find(section);

    if(sectionIterator==data.end())return "";

    auto keyIterator=sectionIterator->second.find(key);
    if(keyIterator==sectionIterator->second.end())return "";

    return keyIterator->second;
}
int Config::getInt(const std::string& section,const std::string& key) const
{
    return std::stoi(get(section,key));
}