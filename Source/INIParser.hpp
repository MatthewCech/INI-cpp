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
#include <cctype>
#include <locale>
#include <algorithm>



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

  // Public methods
  std::vector<INIPair> GetSection(std::string section);
  INIPair GetPair(std::string key);
  std::string Get(std::string key);

private:
  // Private methods
  void trimStringStart(std::string &str);
  void trimStringEnd(std::string &str);
  void trimString(std::string &str);
  bool INIParser::startsWith(const std::string &line, const std::string &chars);

  // Internal data and refs
  std::fstream &file_;
  std::map<std::string, std::vector<INIPair>> sectionLookup_;
  std::map<std::string, INIPair> directLookup_;
  //std::vector<INILine
};

  ////////////////////////////////////////
  // Private utility-oriented functions //
////////////////////////////////////////
// Determines if the specified characters lead the string
bool INIParser::startsWith(const std::string &line, const std::string &chars)
{
  for (std::size_t i = 0; i < chars.size(); ++i)
  {
    if (line[0] == chars[i])
      return true;
  }

  return false;
}

// Removes whitespace characters at the beginning of a string
void INIParser::trimStringStart(std::string &str)
{
  str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int character) {
    return !std::isspace(character);
  }));
}

// Removes whitespace characters at the end of a string
void INIParser::trimStringEnd(std::string &str)
{
  str.erase(std::find_if(str.rbegin(), str.rend(), [](int character) {
    return !std::isspace(character);
  }).base(), str.end());
}

// Removes whitespace characters from the beginning and end of a string
void INIParser::trimString(std::string &str)
{
  trimStringStart(str);
  trimStringEnd(str);
}


  ////////////////////////////////////////////////////
  // Publicly accessable constructors and functions //
////////////////////////////////////////////////////
INIParser::INIParser(std::fstream &file) : file_(file)
{
  if (file.is_open())
  {
    std::string currentLine = "";
    std::string currentSection = "";

    while (std::getline(file, currentLine))
    {
      trimString(currentLine);

      // Case: Empty line
      // Appearance:
      // Notes: Whitespace trimmed.
      if (currentLine.size() <= 0)
      {
        continue;
      }

      // Case: Comment
      // Appearance: ; comment
      // Appearance: # comment
      // Notes: Comments are single-line.
      if (startsWith(currentLine, ";#"))
      {
        continue;
      }

      // Case: Section
      // Appearance: [Section]
      // Notes: [] clears the section to default.
      if (startsWith(currentLine, "["))
      {
        std::size_t loc = currentLine.find_first_of("]");

        if (loc != currentLine.npos)
          currentSection = currentLine.substr(1, loc - 1); // subtract 1 for offset

        continue;
      }

      // Case: Key-value pair
      // Appearance: key = value
      // Appearance: key=value
      // Appearance: key =
      // Appearance: = value
      // Notes: Trailing and leading whitespace ignored.
      std::size_t loc = currentLine.find_first_of("=");
      if (loc != currentLine.npos)
      {
        std::string key = currentLine.substr(0, loc);
        std::string value = currentLine.substr(loc + 1);
        trimStringEnd(key);
        trimStringStart(value);
        sectionLookup_[currentSection].push_back(INIPair(key, value));
        directLookup_[key] = INIPair(key, value);
      }
    }
  }
}

std::vector<INIPair> INIParser::GetSection(std::string section)
{
  auto iter = sectionLookup_.find(section);

  if (iter != sectionLookup_.end())
    return iter->second;
  else
    return {  };
}

INIPair INIParser::GetPair(std::string key)
{
  auto iter = directLookup_.find(key);

  if (iter != directLookup_.end())
    return iter->second;
  else
    return {  };
}

// Alternately, if allocating memory, we can return a reference. 
std::string INIParser::Get(std::string key)
{
  auto iter = directLookup_.find(key);

  if (iter != directLookup_.end())
    return iter->second.Value;
  else
    return {};
}
