/*!***************************************************************************
@file    main.cpp
@author  Matthew Cech
@date    8/19/2018

@brief 
Testing file for the INI parser

@copyright See LICENSE.md
*****************************************************************************/
#include <iostream>               // std::cout
#include <utility>                // std::pair
#include <string>                 // std::string
#include <fstream>                // std::fstream object
#include <GUtils/GTimekeeper.hpp> // Rutils::RException
#include "INIParser.hpp"          // The INI parser we're testing

#define TEST_SUCCESS std::make_pair(true, __FUNCTION__)
#define TEST_FAIL std::make_pair(false, __FUNCTION__)
#define TEST_RETURN std::pair<bool, std::string>
#define DUMP_REREAD(str) std::fstream("test.ini", std::ios::out); testFile << str; testFile.close(); testFile.open("test.ini", std::fstream::in | std::ios_base::out)

TEST_RETURN TestPairSplit()
{
  std::string testString = "testKey=testValue";
  std::fstream testFile = DUMP_REREAD(testString);
  INIParser parser = INIParser(testFile);
  return TEST_SUCCESS;
}

TEST_RETURN TestPairCategory()
{
  std::string test = "[section]\ntestKey=testValue";
  return TEST_SUCCESS;
}

TEST_RETURN (*Tests[])(void) = 
{ 
    TestPairSplit
  , TestPairCategory
  , 
};

int main(int argc, char** argv)
{
  // Timing start
  GUtils::Timekeeper t;
  t.StartFrame();   
  
  // Run tests
  for (int i = 0; i < sizeof(Tests) / sizeof(Tests[0]); ++i)
  {
    TEST_RETURN test = Tests[i]();
    std::cout << test.second << ": " << (test.first ? " Success! " : " FAIL! ") << std::endl;
  }

  // Timing end
  t.EndFrame(); 
  std::cout << "Tests took: " << t.GetLastTimeMS() << "ms" << std::endl;
  std::cin.ignore();
  return 0;
}
