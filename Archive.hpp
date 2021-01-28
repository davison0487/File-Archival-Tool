//
//  Archive.hpp
//  RGAssignment2
//
//  Created by rick gessner on 1/24/21.
//

#ifndef Archive_hpp
#define Archive_hpp

#include <stdio.h>
#include <iostream>

namespace ECE141 {
  
  enum class ActionType {added, extracted, removed, listed, dumped};

  struct ArchiveObserver {
    void operator()(ActionType anAction,
                    const std::string &aName, bool status);
  };
  
  //---------------------------------------------------

  class Archive {
  protected:
              Archive();  //protected on purpose!

  public:

              ~Archive();  //

    static    Archive* createArchive(const std::string &anArchiveName);
    static    Archive* openArchive(const std::string &anArchiveName);

    Archive&  addObserver(ArchiveObserver &anObserver);
    
    bool      add(const std::string &aFilename);
    bool      extract(const std::string &aFilename, const std::string &aFullPath);
    bool      remove(const std::string &aFilename);
    
    size_t    list(std::ostream &aStream);
    size_t    debugDump(std::ostream &aStream);

    size_t    compact();

              //STUDENT: add anything else you want here...
    
  };

}

#endif /* Archive_hpp */
