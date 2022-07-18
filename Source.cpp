#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <iomanip>
#include <cstdlib>
#include "time.h"
#include <set> 
using namespace std;


const int n = 1;
const int line_size = 16; //16, 32, 64, 128  --> 4, 5, 6, 7
const int CACHE_SIZE = 64 * 1024 ;
const int ADDRESS_SIZE = 32;

int byte_offset_bits = log2(line_size);
int index_bits = log2(CACHE_SIZE / (n * line_size));
int tag_bits = ADDRESS_SIZE - index_bits - byte_offset_bits;
int lineNum = CACHE_SIZE / (n * line_size);
#define		DRAM_SIZE		(64*1024*1024)

unsigned int NumMaskBits()
{
    unsigned int mask;
    switch (index_bits)
    {
    case 3: mask = 0x7; break; //validation purposes
    case 6: mask = 0x3f; break;
    case 7: mask = 0x7f; break;
    case 8: mask = 0xff; break;
    case 9: mask = 0x1ff; break;
    case 10: mask = 0x3ff; break;
    case 11: mask = 0x7ff; break;
    case 12: mask = 0xfff; break;
    default: mask = 0x0;
    }
    return mask;
}
unsigned int memGenValidate()
{
    static unsigned int addr = 0;
    addr = rand() % 127;
    //cout << addr << endl;
    return addr;
};


unsigned int m_w = 0xABABAB55;    /* must not be zero, nor 0x464fffff */
unsigned int m_z = 0x05080902;    /* must not be zero, nor 0x9068ffff */
unsigned int rand_()
{
    m_z = 36969 * (m_z & 65535) + (m_z >> 16);
    m_w = 18000 * (m_w & 65535) + (m_w >> 16);
    return (m_z << 16) + m_w;  /* 32-bit result */
}

unsigned int memGen1()
{
    static unsigned int addr = 0;
    return (addr++) % (DRAM_SIZE);
}

unsigned int memGen2()
{
    static unsigned int addr = 0;
    return  rand_() % (24 * 1024);
}

unsigned int memGen3()
{
    return rand_() % (DRAM_SIZE);
}

unsigned int memGen4()
{
    static unsigned int addr = 0;
    return (addr++) % (4 * 1024);
}

unsigned int memGen5()
{
    static unsigned int addr = 0;
    return (addr++) % (1024 * 64);
}

unsigned int memGen6()
{
    static unsigned int addr = 0;
    return (addr += 32) % (64 * 4 * 1024);
}
// Creating the struct which acts as a line in the cache
struct cacheLine
{
    char validBit;
    string tag = "";
    string data = ""; // stored data
};

// Reading fike to test the input data
char* readFile(string fileName, int& size)
{
    char* file = nullptr;
    ifstream out(fileName);
    if (out.is_open())
    {
        out.seekg(0, out.end); // pointing to the end of the file
        size = out.tellg();    // getting the size of the file
        out.seekg(0, out.beg); // pointing to the beginning of the file

        file = new char[size];
        out.read(file, size);
    }
    else
        cout << "Error reading the text file \n";

    return file; // return the data
}
string DecToBin(unsigned int decimal)
{
    string binary = "";
    string remainder;
    int product = 1;
    if (decimal == 0)
        return "0";
    else
    {
        while (decimal != 0) {
            remainder = to_string(decimal % 2);
            binary = remainder + binary;
            decimal = decimal / 2;
        }
        return binary;
    }
}
// Creating the cache and filling it with data
cacheLine** cacheInfo(char* file, int dataSize, int tagSize, int indexSize, int fileSize)
{
    string fileString, tagTemp, dataTemp, fullLine;
    char validBit;
    int loopLimit = tagSize + dataSize + 4;
    int index = 0;
    for (int i = 0; i < fileSize; i++)
    {
        fileString += file[i];
    }
    cacheLine** cache = new cacheLine * [indexSize];
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
            // cout << "WORD: " << word << endl;
            word = "";
            if (j == n)
            {
                //read the index from the text file (address and then compute it)
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
    /*for (int i = 0; i < lineNum; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cout << cache[i][j].validBit << " " << cache[i][j].tag << endl;
        }
    }*/
    return cache;
}

// A function to read from the cache and set hit and miss values
void read_from_cache(uint32_t address, double& HIT, double& MISS, cacheLine** cache) {
    // getting index(It will act as the set number)
    // shifting to the left to get rid of the offset
    //unsigned int index = ExtractBitsFromUint(address, byte_offset_bits, byte_offset_bits + index_bits);
    unsigned int index = (address >> byte_offset_bits) & NumMaskBits();
    

    // getting TAG

    // shifting to the left by 32 - #bits of index - #bits of offset to get rid of the offset and index
    int shifted_bits = index_bits + byte_offset_bits;
    unsigned int TAG = address >> shifted_bits;
    string tagInString = DecToBin(TAG);
    int size = tagInString.size();
    for (int i = 0; i < (tag_bits - size); i++)
    {

        tagInString = "0" + tagInString;
    }
    // A boolean flag to check if we found the data inside the cache or not
    bool flag = false;
    unsigned int tag_replaced;
    int counter=0;
    // Looping on the lines of the selected set to search the desired location
    for (int i = 0; i < n; i++)
    {
        if (cache[index][i].tag == tagInString && cache[index][i].validBit == '1') // Data Found
        {
            //cout << "Line Found!\n";
            HIT++;
            flag = true;
            break;
        }
    }
    bool flag2 = false;
    if (!flag) // Data isn't found
    {
        // writing the new tag inside the cache and valid_bit to 1
        // Note: We are doing it random as proved to be good
        // Generating a random number within the range of lines per set
        for (int i = 0; i < n; i++) {
            if (cache[index][i].validBit == '0')
            {
                cache[index][i].tag = tagInString;
                cache[index][i].validBit = '1';
                //cout << "Data stored!\n";
                flag2 = true;
                break;
            }
        }
        if (!flag2)
        {
            tag_replaced = (rand() % n);
            cache[index][tag_replaced].tag = tagInString;
            cache[index][tag_replaced].validBit = '1';
            //cout << "Data stored after capacity miss (random insertion)!\n";
        }
        MISS++;
    }
}

int main()
{
    int fileSize;
    //string fileName = "fifty_set_2.txt";
    string fileName = "Test_Case(1,16).txt";

    char* file = readFile(fileName, fileSize);
    auto cache = cacheInfo(file, line_size * 8, tag_bits, lineNum, fileSize);
  

    double HIT = 0;
    double MISS = 0;
    ofstream out;
    out.open(("res1(1,16).csv"), ios::app);
    if (out.fail())
        cout << "file failed to open \n";
    srand(time(0));
    uint32_t address;
    int no_of_iterations = 1000000;
    for (int inst = 0; inst < no_of_iterations; inst++)
    {
        address = memGen6();
        read_from_cache(address, HIT, MISS, cache);
        cout << "0x" << setfill('0') << setw(8) << hex << address;
        cout<< " (" << HIT << ")" << " (" << MISS << ") \n";
    }
    cout << "Hit ratio = " << double(100* double(HIT / no_of_iterations)) << endl;
    cout << "HITS : " << HIT << "\n";
    cout << "MISS : " << MISS << "\n";
    out << line_size << "," << double (100 * double(HIT / no_of_iterations)) << endl;

    /*for (int i = 0; i < lineNum; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cout << cache[i][j].validBit << " " << cache[i][j].tag << endl;
        }
    }*/

    system("pause");
    return 0;
}