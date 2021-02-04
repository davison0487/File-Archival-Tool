//
//  Archive.hpp
//  RGAssignment2
//
//  Created by Yunhsiu Wu on 2/2/21.
//

#ifndef Archive_hpp
#define Archive_hpp

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace ECE141 {
  
  enum class ActionType {added, extracted, removed, listed, dumped, compacted};

  struct ArchiveObserver {
	  virtual void operator() (ActionType anAction,
		                       const std::string& aName,
		                       bool status) = 0;
  };
  
  //---------------------------------------------------

  class Archive {
  protected:
	  //file stream
	  std::fstream arcFile;

	  //full archive path
	  const std::string archivePath;

	  //observer list
	  std::vector<ArchiveObserver*> observerList;

	  Archive(std::string& aFullPath) :archivePath(aFullPath) {};  //protected on purpose!

  public:

              ~Archive();  //

    static    Archive* createArchive(const std::string &anArchiveName);
    static    Archive* openArchive(const std::string &anArchiveName);

    Archive&  addObserver(ArchiveObserver &anObserver);
	void notifyObserver(ActionType anAction, const std::string& aName, bool status);
    
    bool      add(const std::string& aFullPath);
    bool      extract(const std::string &aFilename, const std::string &aFullPath);
    bool      remove(const std::string &aFilename);
    
    size_t    list(std::ostream &aStream);
    size_t    debugDump(std::ostream &aStream);

    size_t    compact();

              //STUDENT: add anything else you want here...
    
  };

}

#endif /* Archive_hpp */
