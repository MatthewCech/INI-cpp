/*!***************************************************************************
@file    main.cpp
@author  Matthew Cech
@date    8/21/2018

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


  ////////////////////////////////
 // General setup & test utils //
////////////////////////////////
#define TEST_SUCCESS std::make_pair(true, __FUNCTION__)
#define TEST_FAIL std::make_pair(false, __FUNCTION__)
#define TEST_RETURN std::pair<bool, std::string>

std::fstream CreateTempFile(const std::string str)
{
  std::fstream testFile = std::fstream("test.ini", std::ios::out);
  testFile << str; 
  testFile.close(); 
  testFile.open("test.ini", std::fstream::in | std::ios_base::out);
  return testFile;
}


  //////////////////
 // Tests to run //
//////////////////
TEST_RETURN TestPairSplit()
{
  std::string testString = "testKey=testValue";
  std::fstream testFile = CreateTempFile(testString);
  INIData parser = INIData(testFile);

  INIPair pair = parser.GetPair("testKey");
  if (pair.Key == "testKey" && pair.Value == "testValue")
    return TEST_SUCCESS;
  
  return TEST_FAIL;
}

TEST_RETURN TestPairSplitWhitespace()
{
  std::string testString = " testKey = testValue ";
  std::fstream testFile = CreateTempFile(testString);
  INIData parser = INIData(testFile);

  INIPair pair = parser.GetPair("testKey");
  if (pair.Key == "testKey" && pair.Value == "testValue")
    return TEST_SUCCESS;

  return TEST_FAIL;
}

TEST_RETURN TestDefaultCateogry()
{
  std::string testString = "testKey=testValue";
  std::fstream testFile = CreateTempFile(testString);
  INIData parser = INIData(testFile);

  if (parser.GetSection("").size() == 1 && parser.GetSection("")[0].Key == "testKey" && parser.GetSection("")[0].Value == "testValue")
    return TEST_SUCCESS;

  return TEST_FAIL;
}

TEST_RETURN TestPairCategory()
{
  std::string testString = "[section]\ntestKey=testValue";
  std::fstream testFile = CreateTempFile(testString);
  INIData parser = INIData(testFile);

  auto pairVector = parser.GetSection("section");
  if(pairVector.size() == 1 && pairVector[0].Key == "testKey" && pairVector[0].Value == "testValue")
    return TEST_SUCCESS;

  return TEST_FAIL;
}

TEST_RETURN (*Tests[])(void) = 
{ 
    TestPairSplit
  , TestPairSplitWhitespace
  , TestDefaultCateogry
  , TestPairCategory
};


  /////////////////////////////
 // Application entry point //
/////////////////////////////
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
