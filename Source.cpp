#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

struct cacheLine
{
    string offset;
    string tag;
    string data;
};

char* readFile(string, int& fileSize );
cacheLine* cacheInfo( char* file, int dataSize, int tagSize, int indexSize, int offsetSize, int fileSize);

int main()
{
    //Testing the functions
    int fileSize;
    string fileName = "Text.txt";
    char* file = readFile(fileName, fileSize);

    cacheLine* cache = new cacheLine [5]; 
    cache = cacheInfo(file, 4, 3, 8, 2, fileSize);

    for (int i = 0; i <8 ; i++)
    {
        cout << cache[i].offset << " " << cache[i].tag << " " << cache[i].data << endl;
    }

	system("pause");
	return 0;
}
char* readFile(string fileName, int& size)
{
    char* file=nullptr;
    ifstream out(fileName);
    if (out.is_open()) {
        out.seekg(0, out.end);
        size = out.tellg();
        out.seekg(0, out.beg);

        file = new char[size];
        out.read(file, size);
    }
    else
        cout << "Error reading the text file \n";

    return file;

}
cacheLine* cacheInfo(char* file, int dataSize, int tagSize, int indexSize, int offsetSize, int fileSize)
{
    string fileString, offsetTemp, tagTemp, dataTemp, fullLine;
    int loopLimit = offsetSize + tagSize + dataSize + 3;
    int index = 0;

    for (int i = 0; i < fileSize; i++)
    {
        fileString += file[i];
    }

    cacheLine *cache = new cacheLine[indexSize];
    
    for (int i = 0; i <= loopLimit*(indexSize-1); i=i+loopLimit)
    {
        fullLine= fileString.substr(i, offsetSize + tagSize + dataSize + 2);
        tagTemp=fullLine.substr(offsetSize+1, tagSize);
        dataTemp= fullLine.substr(offsetSize + tagSize + 2, dataSize);
        offsetTemp = fullLine.substr(0, offsetSize);

        cache[index].offset = offsetTemp;
        cache[index].tag = tagTemp;
        cache[index].data = dataTemp;
        index++;
    }

    /*for (int i = 0; i < 5; i++)
    {
        cout << cache[i].offset << "\t" << cache[i].tag << "\t" << cache[i].data << endl;
    }*/
   
    return cache;
}
