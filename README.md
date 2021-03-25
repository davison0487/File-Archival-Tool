# File Archival Tool 
This porject includes a non-compressing **Archival** storage system.

### Overview
`Archive` system supports:
- 1. Adding a file to the archive
- 2. Extracting a copy of a file from the archive
- 3. Removing a file from the archive (permanently)
- 4. Listing the names of all files in the archive
- 5. Performing a diagnostic "dump" of all the blocks in the file

In addition, `Archival` tool will allow interested objects to register themselves as _observers_. _Observers_ will be notified of any primary action (add, extract, list, remove) performed by the `Archive` system by way of a callback.

### Archive Tool Commands 

#### __Add__ file to Archive
User can add a new document to the archive by calling the `Archive::add` method. The signature for this method is given below:

```
bool add(std::string &aFullPath);
```

The `aFullPath` will contain the complete path (path+name) of a file to be added to the stream.  For example: "/tmp/small.txt".   The `Archive::add` method will copy the contents of the given file into a series of 1..n blocks within the archive itself. The document will be associated in the archive with the filename portion of the given full-path, so that it may be retrieved (extracted) or removed in the future. If everything goes works, this function call will return `True`.  For example, if `aFullPath` is "/tmp/small.txt", the name you store for the document is "small.txt" (exclude the path portion). If the user tries to add a document to an archive that already contains a file with that name, this function returns `False`. 

#### __Extract__ file from Archive
User can extract a named resource inside the archive file. If the file exists in the archive, it will write the contents of the file from your storage blocks to a file at the given `fullPath`, and return `True`. If the named resource does not exist, `Archive::extract()` will stop and return `False`. 

```
bool extract(std::string &aName, const std::string &aFullOutputPath);
```

#### __Remove__ file from Archive
User can __remove__ a named resource inside the archive file. When the user calls this message and passes a document name, search the archive for that file. If the file exists in the archive, the file will be removed from the archive. If this succeeds, return `True`. If the document name is not found in the archive, return `False` to the caller.

```
bool remove(std::string &aName);
```

#### __List__ files in Archive
User can __list__ all the names of the documents stored inside the archive file. Print their name to the given `std::ostream` object. Return the number of files found in the archive. Sample output format documents in the archive is given below:

```
###  name         size       date added
------------------------------------------------
1.   document1    203,400    2021-01-15 10:15:00
2.   document2    2,150      2021-01-16 16:22:30
```

Here is the __list__ method call:
```
size_t list(std::ostream);
```

#### __Dump__ archive blocks (debug mode)
User can __dump__ information about the sequence of blocks stored inside the archive file. When the user calls this message, iterate the blocks and prints meta information about the block to the given `std::ostream` object. When the function is finished, return the number of blocks found in the archive. Sample output of the dump is given below:

```
###  status   name    
-----------------------
1.   empty    
2.   used     test.txt
3.   used     test.txt
4.   empty
```

Here is the __dump__ method call:
```
size_t debugDump(std::ostream anOutputStream);
```

#### __Compact__ archive file 
This method will remove empty blocks in the archive file. Once complete, the archive file will contain 0 empty blocks, and every block will contain data from an archived document. This method returns the total number of blocks in the given archive file.

#### Registering an Observer
A user may register one or more "observers" of `Archive` class by calling the `Archive::addObserver()` method. This method will accept a single argument (a reference to an observer). `Archive` will notify registered observer when the `Archive` class methods are called ('add','extract','list','remove','dump').

```
  bool addObserver(ArchiveObserver &anObserver);
```

<img src="/notify.jpg" width="350" alt="notifications">

