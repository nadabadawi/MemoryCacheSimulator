#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
using namespace std;

const int n = 4;
const int line_size = 32; //16, 32, 64, 128  --> 4, 5, 6, 7
const int CACHE_SIZE = 64 * 1024;
const int ADDRESS_SIZE = 32;

int byte_offset_bits = log2(line_size);
int index_bits = log2(CACHE_SIZE / (n * line_size));
int tag_bits = ADDRESS_SIZE - index_bits - byte_offset_bits;
int lineNum = CACHE_SIZE / (n * line_size);

unsigned int NumMaskBits()
{
    unsigned int mask;
    switch (index_bits)
    {
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
unsigned int m_w = 0xABABAB55;    /* must not be zero, nor 0x464fffff */
unsigned int m_z = 0x05080902;    /* must not be zero, nor 0x9068ffff */
unsigned int rand_()
{
    m_z = 36969 * (m_z & 65535) + (m_z >> 16);
    m_w = 18000 * (m_w & 65535) + (m_w >> 16);
    return (m_z << 16) + m_w;  /* 32-bit result */
}

unsigned int memGen2()
{
    static unsigned int addr = 0;
    return  rand_() % (24 * 1024);
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
    cout << "Flag: " << test_ctr << endl;
    return cache;
}

// A function to read from the cache and set hit and miss values
void read_from_cache(uint32_t address, int& HIT, int& MISS, cacheLine** cache) {
    // getting index(It will act as the set number)
    // shifting to the left to get rid of the offset
    //unsigned int index = ExtractBitsFromUint(address, byte_offset_bits, byte_offset_bits + index_bits);
    unsigned int index = (address >> byte_offset_bits) & NumMaskBits();
    //cout << "ReadFromCahce: index: " << index << endl;
    // getting TAG

    // shifting to the left by 32 - #bits of index - #bits of offset to get rid of the offset and index
    int shifted_bits = index_bits + byte_offset_bits;
    unsigned int TAG = address >> shifted_bits;
    
    //bitset<32> set = TAG;
    string tagInString = DecToBin(TAG);

    // A boolean flag to check if we found the data inside the cache or not
    bool flag = false;

    // Looping on the lines of the selected set to search the desired location
    //cout << "Current tag: " << tagInString << endl;
    for (int i = 0; i < n; i++)
    {
        //cout << "Cache elemnt Tag: " << cache[index][i].tag << endl;
        if (cache[index][i].tag == tagInString && cache[index][i].validBit == '1') // Data Found
        {
            HIT++;
            flag = true;
            break;
        }
    }
    if (!flag) // Data isn't found
    {
        // writing the new tag inside the cache and valid_bit to 1
        // Note: We are doing it random as proved to be good

        // Generating a random number within the range of lines per set
        unsigned int tag_replaced = 0 + (rand() % n);
        cache[index][tag_replaced].tag = tagInString;
        cache[index][tag_replaced].validBit = '1';
        MISS++;
    }
}

int main()
{
    int fileSize;
    string fileName = "Test_Case(4,32).txt";
    char* file = readFile(fileName, fileSize);
    // cacheLine** cache = new cacheLine * [lineNum];

    // for (int i = 0; i < lineNum; i++)
    //   cache[i] = new cacheLine[n];
    auto cache = cacheInfo(file, line_size * 8, tag_bits, lineNum, fileSize);    
    
    /*for (int i = 0; i < lineNum; i++)
    {
        for (int j = 0; j < n; j++)
            cout << cache[i][j].validBit << " " << cache[i][j].tag << " " << cache[i][j].data << endl;
    }*/
     
    int HIT = 0;
    int MISS = 0;
    // Testing
    //uint32_t address_1 = 0xB22A4000;//0x7345BFE1; // HIT
    //read_from_cache(address_1, HIT, MISS, cache);
    //address_1 = 0b101100100010101001; // HIT
    // read_from_cache(address_1, HIT, MISS, cache);
    uint32_t address_1 = 0xB22A4020;
    read_from_cache(address_1, HIT, MISS, cache);
    read_from_cache(address_1, HIT, MISS, cache);
    read_from_cache(address_1, HIT, MISS, cache);

     //uint32_t address_2 = 0b000011000011111110; // HIT
     //read_from_cache(address_2, HIT, MISS, cache);

     /*uint32_t address_3 = 0b111111000011111110; // MISS
     read_from_cache(address_3, HIT, MISS, cache);

     uint32_t address_4 = 0b101111000011111110; // MISS
     read_from_cache(address_4, HIT, MISS, cache);*/

    cout << "HITS : " << HIT << "\n";
    cout << "MISS : " << MISS << "\n";

    system("pause");
    return 0;
}
