#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
using namespace std;

struct cacheLine
{
    char validBit;
    string tag;
    string data;
};

char* readFile(string, int& fileSize );
cacheLine** cacheInfo( char* file, int dataSize, int tagSize, int indexSize,int fileSize);

const int n = 4;
const int line_size = 32;
const int CACHE_SIZE = 64 * 1024;
const int ADDRESS_SIZE = 32;

int byte_offset_bits = log2(line_size);
int index_bits = log2(CACHE_SIZE / (n * line_size));
int tag_bits = ADDRESS_SIZE - index_bits - byte_offset_bits;
int lineNum = CACHE_SIZE / (n * line_size);

int main()
{
    int fileSize;
    string fileName = "Test_Case(4,32).txt";
    char* file = readFile(fileName, fileSize);
    cacheLine** cache = new cacheLine * [lineNum];

    for (int i = 0; i < lineNum; i++)
        cache[i] = new cacheLine[n];
    cache = cacheInfo(file, line_size*8, tag_bits, lineNum, fileSize);

    for (int i = 0; i < lineNum; i++)
    {
        for (int j = 0; j < n; j++)
            cout << cache[i][j].validBit << " " << cache[i][j].tag << " " << cache[i][j].data << endl;
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
cacheLine** cacheInfo(char* file, int dataSize, int tagSize, int indexSize,int fileSize)
{
    string fileString, tagTemp, dataTemp, fullLine;
    char validBit;
    int loopLimit = tagSize + dataSize + 4;
    int index = 0;

    for (int i = 0; i < fileSize; i++)
    {
        fileString += file[i];
    }
    cacheLine **cache = new cacheLine*[indexSize]; 
    for (int i = 0; i < indexSize; i++)
        cache[i] = new cacheLine[n];
    int counter = 0;
    string word = "";
    int newline_ctr = 0;
    int j = 0;
    int test_ctr = 0;
    for (int i = 0; i < fileString.size(); i++)
    {
            if (fileString[i] == '\n')
            {
                if (counter == 1)
                    cache[index][j].tag = word;
                else if (counter == 2)
                    cache[index][j].data = word;
                else if (!counter)
                    cache[index][j].validBit = word[0];
                j++;
                test_ctr++;
                counter = 0;
                word = "";
                if (j == n)
                {
                    index++;
                    j = 0;
                }
            }
            else if (fileString[i] == ' ')
            {
                if (counter == 1)
                    cache[index][j].tag = word;
                else if (counter == 2)
                    cache[index][j].data = word;
                else if (!counter)
                {
                    cache[index][j].validBit = word[0];
                }
                word = "";
                counter++;
            }
            else
                word = word + fileString[i];
        
    }
    cout << "Flag: " << test_ctr << endl;
    return cache;
}
