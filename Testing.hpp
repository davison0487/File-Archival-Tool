//
//  Testing.hpp
//  RGAssignment2
//
//  Created by rick gessner on 1/24/21.
//

#ifndef Testing_h
#define Testing_h
#include "Archive.hpp"
#include <fstream>
#include <sstream>
#include <vector>

namespace ECE141 {
  
struct Testing : public ArchiveObserver {
  
  //-------------------------------------------
  std::string folder;
  
  std::string getRandomWord() {
    static std::vector<std::string> theWords = {
      std::string("class"),   std::string("happy"),
      std::string("coding"),  std::string("pattern"),
      std::string("design"),  std::string("method"),
      std::string("dyad"),    std::string("story"),
      std::string("monad"),   std::string("data"),
      std::string("compile"), std::string("debug"),
    };
    return theWords[rand() % theWords.size()];
  }
  
  void makeFile(const std::string &aFullPath, size_t aMaxSize) {
    const char* thePrefix="";
    std::ofstream theFile(aFullPath.c_str(),std::ios::trunc | std::ios::out);
    size_t theSize=0;
    size_t theCount=0;
    while(theSize<aMaxSize) {
      std::string theWord=getRandomWord();
      theFile << thePrefix;
      if(0==theCount++ % 10) theFile << "\n";
      theFile << theWord;
      thePrefix=", ";
      theSize+=theWord.size()+2;
    }
    theFile << std::endl;
    theFile.close();
  }

  void buildTestFiles() {
    makeFile(folder+"/small.txt",900);
    makeFile(folder+"/medium.txt",1900);
    makeFile(folder+"/large.txt",2900);
  }
  
  Testing(const std::string &aFolder) : folder(aFolder) {
    buildTestFiles();
  }
  
  bool doCompileTest() {
    return true;
  }
  
  size_t addTestFiles(Archive &anArchive) {
    anArchive.add(folder+"/small.txt");
    anArchive.add(folder+"/medium.txt");
    anArchive.add(folder+"/large.txt");
    return 3;
  }
  
  bool doAddTest() {
    size_t theCount=0;
    size_t theAddCount=0;
    Archive *theArchive = Archive::createArchive(folder+"/test");
    if(theArchive) {
      theArchive->addObserver(*this);
      theAddCount=addTestFiles(*theArchive);
      std::stringstream theStream;
      theArchive->list(theStream);
      theCount=theArchive->list(theStream);
      delete theArchive;
    }
    return theCount==theAddCount;
  }
  
  //-------------------------------------------
  
  bool verifyDump(const std::string &aString) {
    //coming soon...
    return false;
  }
  
  bool doDumpTest() {
    bool   theResult=false;
    Archive *theArchive = Archive::createArchive(folder+"/test");
    if(theArchive) {
      theArchive->addObserver(*this);
      addTestFiles(*theArchive);
      addTestFiles(*theArchive);
      std::stringstream theStream;
      if(12==theArchive->debugDump(theStream)) {
        theResult=verifyDump(theStream.str());
      }
      //std::cout << "\n" << theStream.str() << "\n";
      delete theArchive;
    }
    return theResult;
  }

  //-------------------------------------------
  
  bool filesMatch(const std::string &aFilename, const std::string &aFullPath) {
    std::string theFilePath("/tmp/");
    theFilePath+=aFilename;
    std::ifstream theFile1(theFilePath);
    std::ifstream theFile2(aFullPath);

    std::string theLine1;
    std::string theLine2;

    if(theFile1 && theFile2){
      while(std::getline(theFile1,theLine1)) {
        std::getline(theFile2,theLine2);
        if(theLine1!=theLine2) return false;
      }
    }
    return true;
  }
  
  //-------------------------------------------
  
  const char* pickRandomFile() {
    static const char* theFiles[]={"small.txt","medium.txt","large.text"};
    size_t theCount=sizeof(theFiles)/sizeof(char*);
    return theFiles[rand() % theCount];
  }
  
  bool doExtractTest() {
    bool theResult=false;
    Archive *theArchive = Archive::createArchive(folder+"/test");
    if(theArchive) {
      theArchive->addObserver(*this);
      addTestFiles(*theArchive);
      std::stringstream theStream;
      std::string theFileName=pickRandomFile();
      theArchive->extract(theFileName,"/tmp/test.txt");
      theResult=filesMatch(theFileName,"/tmp/test.txt");
      delete theArchive;
    }
  
    return theResult;
  }

  //-------------------------------------------
  
  bool verifyAddList(const std::string &aString) {
    //coming soon...
    return true;
  }
  
  bool doListTest() {
    bool theResult=false;
    Archive *theArchive = Archive::createArchive(folder+"/test");
    if(theArchive) {
      theArchive->addObserver(*this);
      addTestFiles(*theArchive);
      std::stringstream theStream;
      theArchive->list(theStream);
      std::string theOutput=theStream.str();
      theResult=verifyAddList(theOutput);
      delete theArchive;
    }
    return theResult;
  }

  //-------------------------------------------
  
  bool verifyRemove(const std::string &aName, std::string &anOutput) {
    //coming soon...
    return false;
  }
  
  bool doRemoveTest() {
    bool theResult=false;
    Archive *theArchive = Archive::createArchive(folder+"/test");
    if(theArchive) {
      theArchive->addObserver(*this);
      addTestFiles(*theArchive);
      std::stringstream theStream;
      std::string theFileName=pickRandomFile();
      theArchive->remove(theFileName);
      theArchive->list(theStream);
      std::string theOutput=theStream.str();
      theResult=verifyRemove(theFileName, theOutput);
      delete theArchive;
    }
    return theResult;
  }
  
  //-------------------------------------------

  bool doCompactTest() {
    //coming soon...
    return false;
  }  
  
  //-------------------------------------------

  bool doStressTest() {
    //coming soon...
    return false;
  }
  
  void operator()(ActionType anAction, const std::string &aName, bool status) {
    std::cerr << "observed ";
    switch(anAction) {
      case ActionType::added: std::cerr << "add "; break;
      case ActionType::extracted: std::cerr << "extract "; break;
      case ActionType::removed: std::cerr << "remove "; break;
      case ActionType::listed: std::cerr << "list "; break;
      case ActionType::dumped: std::cerr << "dump "; break;
    }
    std::cerr << aName << "\n";
  }
  
};



}

#endif /* Testing_h */
