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

class INIData
{
public:
  // Constructor
  INIData(std::fstream &file);

  // Public methods
  std::vector<INIPair> GetSection(std::string section);
  INIPair GetPair(std::string key);
  std::string Get(std::string key);
  std::string Export();
  std::string ExperimentalExport();

private:
  // Internal structures
  struct INILine
  {
    INILine(std::string section, std::string comment, std::string key, std::string value) : Section(section), Comment(comment), Key(key), Value(value) {  }
    std::string Section;
    std::string Comment;
    std::string Key;
    std::string Value;
  };

  // Private methods
  void trimStringStart(std::string &str);
  void trimStringEnd(std::string &str);
  void trimString(std::string &str);
  bool INIData::startsWith(const std::string &line, const std::string &chars);


  // Internal data and refs
  std::fstream &file_;
  std::map<std::string, std::vector<INIPair>> sectionLookup_;
  std::map<std::string, INIPair> directLookup_;
  std::vector<INILine> lines_;
};

  ////////////////////////////////////////
  // Private utility-oriented functions //
////////////////////////////////////////
// Determines if the specified characters lead the string
bool INIData::startsWith(const std::string &line, const std::string &chars)
{
  for (std::size_t i = 0; i < chars.size(); ++i)
  {
    if (line[0] == chars[i])
      return true;
  }

  return false;
}

// Removes whitespace characters at the beginning of a string
void INIData::trimStringStart(std::string &str)
{
  str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int character) {
    return !std::isspace(character);
  }));
}

// Removes whitespace characters at the end of a string
void INIData::trimStringEnd(std::string &str)
{
  str.erase(std::find_if(str.rbegin(), str.rend(), [](int character) {
    return !std::isspace(character);
  }).base(), str.end());
}

// Removes whitespace characters from the beginning and end of a string
void INIData::trimString(std::string &str)
{
  trimStringStart(str);
  trimStringEnd(str);
}


  ////////////////////////////////////////////////////
  // Publicly accessable constructors and functions //
////////////////////////////////////////////////////
INIData::INIData(std::fstream &file) : file_(file)
{
  if (file.is_open())
  {
    std::string lineComment = "";
    std::string lineKey = "";
    std::string lineValue = "";
    std::string lineSection = "";
    std::string currentLine = "";
    std::string currentSection = "";

    while (std::getline(file, currentLine))
    {
      trimString(currentLine);

      // Use loop for contained jumping.
      do
      {

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
          lineComment = "";
          continue;
        }

        // Case: Section
        // Appearance: [Section]
        // Notes: [] clears the section to default.
        if (startsWith(currentLine, "["))
        {
          std::size_t loc = currentLine.find_first_of("]");

          if (loc != currentLine.npos)
          {
            currentSection = currentLine.substr(1, loc - 1); // subtract 1 for offset
            lineSection = currentSection;
          }

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
          lineKey = currentLine.substr(0, loc);
          lineValue = currentLine.substr(loc + 1);
          trimStringEnd(lineKey);
          trimStringStart(lineValue);
          sectionLookup_[currentSection].push_back(INIPair(lineKey, lineValue));
          directLookup_[lineKey] = INIPair(lineKey, lineValue);
          continue;
        }

      } while (0);

      lines_.push_back({ lineSection, lineComment, lineKey, lineValue });
      lineSection = "";
      lineComment = "";
      lineKey = "";
      lineValue = "";
    }
  }
}


std::vector<INIPair> INIData::GetSection(std::string section)
{
  auto iter = sectionLookup_.find(section);

  if (iter != sectionLookup_.end())
    return iter->second;
  else
    return {  };
}

INIPair INIData::GetPair(std::string key)
{
  auto iter = directLookup_.find(key);

  if (iter != directLookup_.end())
    return iter->second;
  else
    return {  };
}

// Alternately, if allocating memory, we can return a reference. 
std::string INIData::Get(std::string key)
{
  auto iter = directLookup_.find(key);

  if (iter != directLookup_.end())
    return iter->second.Value;
  else
    return {};
}

// Bunt export. Goes through each section and dumps everything in it.
std::string INIData::Export()
{
  std::string accumulated = "";
  bool fencepost = true;

  for (auto &sectionPair : sectionLookup_)
  {
    accumulated += "[" + sectionPair.first + "]\n";

    for (auto &keyValuePair : sectionPair.second)
    {
      accumulated += keyValuePair.Key + " = " + keyValuePair.Value;
      accumulated += "\n";
    }

    // A single pass is a single printed line.
    accumulated += "\n";
  }

  return accumulated;
}

// Exports the data in a string formatted to be a valid INI file.
// This is attempting to export the values but maintain original
// comments from the first file. This... may not work. It'll most
// certainly be interated on several times.
//
// NOTE(mcech): Attempt storage w/ structure: std::vector<INILine*>, where INILine* is: { section, comment, INIPair* }
std::string INIData::ExperimentalExport()
{
  std::string accumulated = "";

  for (const INILine &line : lines_)
  {
    // If it's empty, skip. We just want it to be an empty line.
    if (line.Comment.size() == 0 && line.Key.size() == 0 && line.Value.size() == 0 && line.Section.size() == 0)
      continue;

    // If it was just a comment line, export it as such.
    if (line.Key.size() == 0 && line.Value.size() == 0 && line.Section.size() == 0)
      accumulated += "; " + line.Comment;

    // If at this point we have a section, print it.
    if (line.Section.size() > 0)
      accumulated += "[" + line.Section + "]";

    // By default, output the key and value. 'key = value', 'key = ', and ' = value' are all valid outputs.
    accumulated += line.Key + " = " + line.Value;

    // A single pass is a single printed line.
    accumulated += "\n";
  }

  return accumulated;
}
