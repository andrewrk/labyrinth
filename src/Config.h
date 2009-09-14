// Config class - abstracts loading/saving configuration

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <string>
#include <map>
using namespace std;

#include <boost/program_options.hpp>
namespace po = boost::program_options;

class Config {
    public:
        // file name of the config data
        Config(string & filename);
        ~Config();

        template <class T>
        T & getItem(string & itemName);

        template <class T>
        void setItem(string & itemName, T & itemValue);

    private:
        string & m_configFile;       
        po::variables_map m_data;

        void readData();
        void writeData();
};

template <class T>
T & Config::getItem(string & itemName){
    // return the item from memory
}

template <class T>
void Config::setItem(string & itemName, T & itemValue){
    // set the item in memory

    // write the data to disk
    writeData();
}

#endif

