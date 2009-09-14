#include "Config.h"

#include <fstream>
#include <iostream>
using namespace std;

Config::Config(string & configFile) :
    m_configFile(configFile),
    m_data()
{
    // read all the configuration data
    readData();
}

Config::~Config() {

}


void Config::readData() {
    // load all data from disk and store it in memory
    m_data.clear();

    ifstream in(m_configFile.c_str(), ifstream::in);
    po::store(po::parse_config_file(in, NULL), m_data);
}


void Config::writeData() {
    // write all data to disk

}

