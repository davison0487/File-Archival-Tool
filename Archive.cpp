//
//  Archive.cpp
//  RGAssignment2
//
//  Created by Yunhsiu Wu on 2/2/21.
//

#include "Archive.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace ECE141;

// Header for archived files
struct arcHeader {
	size_t validSize;
	char name[30];

	arcHeader() {
		validSize = 0;
		name[0] = '\0';
	}
};

static const size_t blockSize = 1024;
static const size_t headerSize = sizeof(arcHeader);
static const size_t dataSize = blockSize - headerSize;

Archive::~Archive() {
	//close file stream
	arcFile.close();
}

Archive* Archive::createArchive(const std::string& anArchiveName) {
	std::string fileName = anArchiveName + ".arc";
	Archive* newArchive = new Archive(fileName);

	//create a new archive file
	std::ofstream newFile(newArchive->archivePath, std::ios::binary | std::ios::out);
	if (!newFile) {
		std::cout << "Failed to create new archive\n";
		return nullptr;
	}
	newFile.close();

	//read the newly created archive file
	newArchive->arcFile.open(newArchive->archivePath, std::ios::binary | std::ios::out | std::ios::in);

	//add an empty block header
	arcHeader newBlock;
	newArchive->arcFile.write(reinterpret_cast<char*>(&newBlock), headerSize);
	
	return newArchive;
}

Archive* Archive::openArchive(const std::string& anArchiveName) {
	std::string fileName(anArchiveName);
	Archive* newArchive = new Archive(fileName);

	//read existed archive file
	newArchive->arcFile.open(newArchive->archivePath, std::ios::binary | std::ios::out | std::ios::in);
	if (!newArchive->arcFile) {
		std::cout << "Failed to read archive\n";
		return nullptr;
	}

	return newArchive;
}

bool Archive::add(const std::string& aFullPath) {
	std::fstream inFile(aFullPath, std::ios::binary | std::ios::in);
	if (!inFile) {
		std::cout << "Failed to open file!\n";
		return false;
	}

	//get file name
	std::string inFileName = aFullPath.substr(aFullPath.find_last_of("/\\") + 1);

	//get archive size
	arcFile.seekg(0, arcFile.end);
	size_t arcSize = arcFile.tellg();

	//refuse to add file if  already exists
	for (size_t i = 0; i * blockSize < arcSize; ++i) {
		arcHeader blockHeader;
		int blockPos = i * blockSize;
		arcFile.seekg(blockPos, arcFile.beg);
		arcFile.read(reinterpret_cast<char*>(&blockHeader), headerSize);
		std::string blockFileName(blockHeader.name);
		if (inFileName == blockFileName) {
			std::cout << "File already exist in archive!\n";
			return false;
		}
	}

	//get in file size
	inFile.seekg(0, inFile.end);
	size_t inFileSize = inFile.tellg();

	//reset position
	inFile.seekg(0);
	arcFile.seekp(0);

	//go through and find available block
	for (size_t i = 0; i * blockSize < arcSize; ++i) {
		arcHeader blockHeader;
		int blockPos = i * blockSize;
		arcFile.seekg(blockPos, arcFile.beg);
		arcFile.read(reinterpret_cast<char*>(&blockHeader), headerSize);
		
		if (blockHeader.validSize == 0 && inFile.tellg() != inFileSize) { // empty block!
			//set up header
			size_t validDataSize = inFileSize - inFile.tellg() > dataSize ? dataSize : inFileSize - inFile.tellg();
			arcHeader inFileHeader;
			inFileHeader.validSize = validDataSize;
			for (int i = 0; i < inFileName.size(); ++i)
				inFileHeader.name[i] = inFileName[i];
			inFileHeader.name[inFileName.size()] = '\0';

			//write to archive
			char dataCache[dataSize];
			inFile.read(dataCache, validDataSize);
			arcFile.seekp(blockPos);
			arcFile.write(reinterpret_cast<char*>(&inFileHeader), headerSize);
			arcFile.write(dataCache, validDataSize);
		}
	}
	
	//if there are still more data to write
	arcFile.seekp(0, arcFile.end);
	while (inFile.tellg() != inFileSize) {
		//set up header
		size_t validDataSize = inFileSize - inFile.tellg() > dataSize ? dataSize : inFileSize - inFile.tellg();
		arcHeader inFileHeader;
		inFileHeader.validSize = validDataSize;
		for (int i = 0; i < inFileName.size(); ++i)
			inFileHeader.name[i] = inFileName[i];
		inFileHeader.name[inFileName.size()] = '\0';
		
		//read file and write to archive
		char dataCache[dataSize];
		inFile.read(dataCache, validDataSize);
		arcFile.write(reinterpret_cast<char*>(&inFileHeader), headerSize);
		arcFile.write(dataCache, validDataSize);
	}

	inFile.close();
	return true;
}

bool Archive::extract(const std::string& aFilename, const std::string& aFullPath) {
	//get archive size
	arcFile.seekg(0, arcFile.end);
	size_t arcSize = arcFile.tellg();

	//search if file exists
	bool fileExist = false;
	for (size_t i = 0; i * blockSize < arcSize; ++i) {
		arcHeader blockHeader;
		int blockPos = i * blockSize;
		arcFile.seekg(blockPos, arcFile.beg);
		arcFile.read(reinterpret_cast<char*>(&blockHeader), headerSize);
		std::string blockFileName(blockHeader.name);
		if (aFilename == blockFileName) {
			fileExist = true;
			break;
		}
	}

	if (!fileExist) {
		std::cout << "File does not exists!\n";
		return false;
	}

	//out file stream
	std::fstream outFile(aFullPath, std::ios::binary, std::ios::out);
	outFile.seekp(0);

	//search for match file
	for (size_t i = 0; i * blockSize < arcSize; ++i) {
		arcHeader blockHeader;
		int blockPos = i * blockSize;
		arcFile.seekg(blockPos, arcFile.beg);
		arcFile.read(reinterpret_cast<char*>(&blockHeader), headerSize);
		std::string blockFileName(blockHeader.name);
		//file found, write to out file stream
		if (aFilename == blockFileName) {
			char dataCache[dataSize];
			arcFile.read(dataCache, blockHeader.validSize);
			outFile.write(dataCache, blockHeader.validSize);
		}
	}

	outFile.close();
	return true;
}

bool Archive::remove(const std::string& aFilename) {
	//get archive size
	arcFile.seekg(0, arcFile.end);
	size_t arcSize = arcFile.tellg();

	//search for file 
	bool fileExist = false;
	for (size_t i = 0; i * blockSize < arcSize; ++i) {
		arcHeader blockHeader;
		int blockPos = i * blockSize;
		arcFile.seekg(blockPos, arcFile.beg);
		arcFile.read(reinterpret_cast<char*>(&blockHeader), headerSize);
		std::string blockFileName(blockHeader.name);

		//file found, mark as empty
		if (aFilename == blockFileName) {
			fileExist = true;
			blockHeader.validSize = 0;
			blockHeader.name[0] = '\0';
			arcFile.seekp(blockPos, arcFile.beg);
			arcFile.write(reinterpret_cast<char*>(&blockHeader), headerSize);
		}
	}
	if (!fileExist)
		std::cout << "File does not exist in archive!\n";
	return fileExist;
};

size_t Archive::list(std::ostream& aStream) {
	std::unordered_map<std::string, int> fileList;

	//get acrhive size
	arcFile.seekg(0, arcFile.end);
	size_t arcSize = arcFile.tellg();

	for (size_t i = 0; i * blockSize < arcSize; ++i) {
		//read header
		arcHeader blockHeader;
		int blockPos = i * blockSize;
		arcFile.seekg(blockPos, arcFile.beg);
		arcFile.read(reinterpret_cast<char*>(&blockHeader), headerSize);
		std::string blockFileName(blockHeader.name);
		
		if (blockHeader.validSize != 0)
			fileList[blockFileName] += blockHeader.validSize;
	}

	aStream << "###  name         size       \n";
	aStream << "-----------------------------\n";

	int i = 1;
	for (auto cur : fileList)
		aStream << i++ << ".\t" << cur.first << "\t" << cur.second << std::endl;

	return fileList.size();
};

size_t Archive::debugDump(std::ostream& aStream) {
	aStream << "###  status   name     \n";
	aStream << "-----------------------\n";

	size_t blockCounts = 0;

	//get acrhive size
	arcFile.seekg(0, arcFile.end);
	size_t arcSize = arcFile.tellg();

	for (size_t i = 0; i * blockSize < arcSize; ++i) {
		//read header
		arcHeader blockHeader;
		int blockPos = i * blockSize;
		arcFile.seekg(blockPos, arcFile.beg);
		arcFile.read(reinterpret_cast<char*>(&blockHeader), headerSize);
		std::string blockFileName(blockHeader.name);
		
		//number
		aStream << i << ".\t";

		//status and file name
		if (blockHeader.validSize == 0)
			aStream << "empty\n";
		else
			aStream << "used\t" << blockFileName << std::endl;

		++blockCounts;
	}
	return blockCounts;
};

size_t Archive::compact() {
	//close current archive and rename to temp.arc
	arcFile.close();

	char oldName[256];
	for (int i = 0; i < archivePath.size(); ++i)
		oldName[i] = archivePath[i];
	oldName[archivePath.size()] = '\0';

	std::string tempPath(archivePath.substr(0, archivePath.find_last_of("/\\") + 1));
	tempPath += "temp.arc";
	char tmpName[256];
	for (int i = 0; i < tempPath.size(); ++i)
		tmpName[i] = tempPath[i];
	tmpName[tempPath.size()] = '\0';

	rename(oldName, tmpName);

	//create a new archive and open with class file stream
	std::ofstream tempStream(archivePath, std::ios::binary);
	tempStream.close();
	arcFile.open(archivePath, std::ios::binary | std::ios::out | std::ios::in);

	//open old archive (renamed to .../temp.arc)
	std::fstream oldArcFile(tempPath, std::ios::binary | std::ios::in);

	//get old acrhive size
	oldArcFile.seekg(0, oldArcFile.end);
	size_t oldArcSize = oldArcFile.tellg();

	//reset position to start
	arcFile.seekp(0);
	oldArcFile.seekg(0);

	//new block counter
	size_t newblockCount = 0;

	//write old data to new archive
	for (size_t i = 0; i * blockSize < oldArcSize; ++i) {
		arcHeader blockHeader;
		int blockPos = i * blockSize;
		oldArcFile.seekg(blockPos, oldArcFile.beg);
		oldArcFile.read(reinterpret_cast<char*>(&blockHeader), headerSize);
		
		//write non-empty block to new archive
		if (blockHeader.validSize != 0) {
			char dataCache[dataSize];
			oldArcFile.read(dataCache, blockHeader.validSize);
			arcFile.write(reinterpret_cast<char*>(&blockHeader), headerSize);
			arcFile.write(dataCache, blockHeader.validSize);
			++newblockCount;
		}
	}

	//delete old archive (.../temp.arc)
	remove(tmpName);

	return newblockCount;
}