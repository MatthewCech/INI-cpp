/*!***************************************************************************
@file    INIParser.hpp
@author  Matthew Cech
@date    8/19/2018

@brief
INI parser for C++.

@copyright See LICENSE.md
*****************************************************************************/
#pragma once
#include <string>
#include <map>
#include <vector>
#include <string>
#include <fstream>



struct INIPair
{
  INIPair() : Key(""), Value("") {  }
  INIPair(std::string key, std::string value) : Key(key), Value(value) {  }
  std::string Key;
  std::string Value;
};

class INIParser
{
public:
  // Constructor
  INIParser(std::fstream &file);

  std::vector<INIPair> GetSection(std::string section);
  INIPair GetKey(std::string key);

private:
  std::fstream &file_;
  std::map<std::string, std::vector<INIPair>> sectionLookup_;
  std::map<std::string, INIPair*> directLookup_;
};

INIParser::INIParser(std::fstream &file) : file_(file)
{
  
}

std::vector<INIPair> INIParser::GetSection(std::string section)
{
  return { };
}

INIPair INIParser::GetKey(std::string key)
{
  return { };
}