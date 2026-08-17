#ifndef DEVHUB_CONFIG_HPP
#define DEVHUB_CONFIG_HPP

#include<string>
#include<map>

class Config{
    private:
    std::map<std::string,std::map<std::string,std::string>>data;

    public:
    bool load(const std::string& filename);

    std::string get(const std::string& section,const std::string& key)const;
    int getInt(const std::string& section,const std::string& key) const;
};

#endif