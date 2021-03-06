/*
    FAT32.h
    FAT32 filesystem Routines in the PETdisk storage device
    Copyright (C) 2011 Michael Hill

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
    Contact the author at bitfixer@bitfixer.com
    http://bitfixer.com
    
    FAT32 code inspired by CC Dharmani's microcontroller blog
    http://www.dharmanitech.com
    
*/

#ifndef _FAT32_H_
#define _FAT32_H_

//Structure to access Master Boot Record for getting info about partitions
struct MBRinfo_Structure{
unsigned char	nothing[446];		//ignore, placed here to fill the gap in the structure
unsigned char	partitionData[64];	//partition records (16x4)
unsigned int	signature;		//0xaa55
};

//Structure to access info of the first partioion of the disk 
struct partitionInfo_Structure{ 				
unsigned char	status;				//0x80 - active partition
unsigned char 	headStart;			//starting head
unsigned int	cylSectStart;		//starting cylinder and sector
unsigned char	type;				//partition type 
unsigned char	headEnd;			//ending head of the partition
unsigned int	cylSectEnd;			//ending cylinder and sector
unsigned long	firstSector;		//total sectors between MBR & the first sector of the partition
unsigned long	sectorsTotal;		//size of this partition in sectors
};

//Structure to access boot sector data
struct BS_Structure{
unsigned char jumpBoot[3]; //default: 0x009000EB   //3
unsigned char OEMName[8];                          //11
unsigned int bytesPerSector; //deafault: 512       //13
unsigned char sectorPerCluster;                    //14
unsigned int reservedSectorCount;                  //16
unsigned char numberofFATs;                        //17
unsigned int rootEntryCount;                       //19
unsigned int totalSectors_F16; //must be 0 for FAT32    //21
unsigned char mediaType;                           //22
unsigned int FATsize_F16; //must be 0 for FAT32    //24
unsigned int sectorsPerTrack;                      //26
unsigned int numberofHeads;                        //28
unsigned long hiddenSectors;                       //32
unsigned long totalSectors_F32;                    //36
unsigned long FATsize_F32; //count of sectors occupied by one FAT   //40
unsigned int extFlags;                              //42
unsigned int FSversion; //0x0000 (defines version 0.0)  //44
unsigned long rootCluster; //first cluster of root directory (=2) //48
unsigned int FSinfo; //sector number of FSinfo structure (=1)   //50
unsigned int BackupBootSector;                      //52
unsigned char reserved[12];                         //64
unsigned char driveNumber;                          //65
unsigned char reserved1;                            //66
unsigned char bootSignature;                        //67
unsigned long volumeID;                             //71
unsigned char volumeLabel[11]; //"NO NAME "         //82
unsigned char fileSystemType[8]; //"FAT32"          //90
unsigned char bootData[420];                        //510
unsigned int bootEndSignature; //0xaa55             //512
};


//Structure to access FSinfo sector data
struct FSInfo_Structure
{
unsigned long leadSignature; //0x41615252
unsigned char reserved1[480];
unsigned long structureSignature; //0x61417272
unsigned long freeClusterCount; //initial: 0xffffffff
unsigned long nextFreeCluster; //initial: 0xffffffff
unsigned char reserved2[12];
unsigned long trailSignature; //0xaa550000
};

//Structure to access Directory Entry in the FAT
struct dir_Structure{
unsigned char name[11];     //0
unsigned char attrib;       //11 //file attributes
unsigned char NTreserved;   //12 //always 0
unsigned char timeTenth;    //13 //tenths of seconds, set to 0 here
unsigned int createTime;    //14 //time file was created
unsigned int createDate;    //16 //date file was created
unsigned int lastAccessDate;//18
unsigned int firstClusterHI;//20 //higher word of the first cluster number
unsigned int writeTime;     //22 //time of last write
unsigned int writeDate;     //24 //date of last write
unsigned int firstClusterLO;//26 //lower word of the first cluster number
unsigned long fileSize;     //28 //size of file in bytes
    //32
};

struct dir_Longentry_Structure{
    unsigned char LDIR_Ord;
    unsigned int LDIR_Name1[5];
    unsigned char LDIR_Attr;
    unsigned char LDIR_Type;
    unsigned char LDIR_Chksum;
    unsigned int LDIR_Name2[6];
    unsigned int LDIR_FstClusLO;
    unsigned int LDIR_Name3[2];
};

// structure for file read information
typedef struct _file_stat{
    unsigned long currentCluster;
    unsigned long fileSize;
    unsigned long currentSector;
    unsigned long byteCounter;
    int sectorIndex;
} file_stat;

typedef struct _file_position {
    unsigned char isLongFilename;
    unsigned char *fileName;
    unsigned long startCluster;
    unsigned long cluster;
    unsigned long dirStartCluster;
    unsigned char sectorIndex;
    unsigned long sector;
    unsigned long fileSize;
    unsigned long byteCounter;
    unsigned int byte;
    unsigned char shortFilename[11];
} file_position;

//Attribute definitions for file/directory
#define ATTR_READ_ONLY     0x01
#define ATTR_HIDDEN        0x02
#define ATTR_SYSTEM        0x04
#define ATTR_VOLUME_ID     0x08
#define ATTR_DIRECTORY     0x10
#define ATTR_ARCHIVE       0x20
#define ATTR_LONG_NAME     0x0f


#define DIR_ENTRY_SIZE     0x32
#define EMPTY              0x00
#define DELETED            0xe5
#define GET     0
#define SET     1
#define READ	0
#define VERIFY  1
#define ADD		0
#define REMOVE	1
#define LOW		0
#define HIGH	1	
#define TOTAL_FREE   1
#define NEXT_FREE    2
#define GET_LIST     0
#define GET_FILE     1
#define DELETE		 2
#define EOF		0x0fffffff

#define MAX_FILENAME 32

//************* external variables *************
volatile unsigned long _firstDataSector,     _rootCluster,        _totalClusters;
volatile unsigned int  _bytesPerSector,      _sectorPerCluster,   _reservedSectorCount;
volatile unsigned long _unusedSectors, _appendFileSector, _appendFileLocation, _fileSize, _appendStartCluster;

//global flag to keep track of free cluster count updating in FSinfo sector
unsigned char _freeClusterCountUpdated;
volatile unsigned long _fileStartCluster;

volatile unsigned char _longEntryString[MAX_FILENAME];
//volatile unsigned long _fileNameLong[MAX_FILENAME];
volatile file_position _filePosition;


//************* functions *************
unsigned char getBootSectorData (void);
unsigned long getFirstSector(unsigned long clusterNumber);
unsigned long getSetFreeCluster(unsigned char totOrNext, unsigned char get_set, unsigned long FSEntry);
struct dir_Structure* findFile (unsigned char *fileName, unsigned long firstCluster);
unsigned long getSetNextCluster (unsigned long clusterNumber,unsigned char get_set,unsigned long clusterEntry);
unsigned char readFile (unsigned char flag, unsigned char *fileName);

void convertToShortFilename(unsigned char *input, unsigned char *output);
void writeFile (unsigned char *fileName);
void appendFile (void);
unsigned long searchNextFreeCluster (unsigned long startCluster);
void displayMemory (unsigned char flag, unsigned long memory);
void deleteFile();
void freeMemoryUpdate (unsigned char flag, unsigned long size);
unsigned char ChkSum (unsigned char *pFcbName);

void startFileRead(struct dir_Structure *dirEntry, file_stat *thisFileStat);
void getCurrentFileBlock(file_stat *thisFileStat);
unsigned long getNextBlockAddress(file_stat *thisFileStat);

unsigned long getFirstCluster(struct dir_Structure *dir);
void openFileForWriting(unsigned char *fileName, unsigned long dirCluster);
unsigned char openFileForReading(unsigned char *fileName, unsigned long dirCluster);
unsigned int getNextFileBlock();
void writeBufferToFile(unsigned int bytesToWrite);
void closeFile();
void makeShortFilename(unsigned char *longFilename, unsigned char *shortFilename);

void openDirectory(unsigned long firstCluster);

struct dir_Structure *getNextDirectoryEntry();
unsigned char ChkSum (unsigned char *pFcbName);

#endif
