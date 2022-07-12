#include <iostream>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <string>
using namespace std;

const int n = 1;
const int line_size = 64;
const int CACHE_SIZE = 64 * 1024;
const int ADDRESS_SIZE = 32;

int byte_offset_bits = log2(line_size);
int index_bits = log2(CACHE_SIZE / (n * line_size));
int tag_bits = ADDRESS_SIZE - index_bits - byte_offset_bits;

int main()
{
    ofstream binfile;
    binfile.open("BinaryTest2.txt");
    srand(time(0));
    int valid;
    string tag;
    string data = "";
    if (binfile.fail())
        cout << "Error!";
    else
    {
        int number_of_lines = CACHE_SIZE / line_size;
        for (int i = 0; i < number_of_lines; i++) //Each iteration is a line
        {
            tag = "";
            data = "";
            valid = rand() % 2;
            for (int i = 0; i < tag_bits; i++)
                tag = tag + to_string(rand() % 2);
            for (int i = 0; i < (line_size * 8); i++)
                data = data + to_string(rand() % 2);
            binfile << to_string(valid);
            if (valid)
                binfile << ' ' << tag << ' ' << data;
            binfile << '\n';
        }
    }

    binfile.close();
    return 0;
}
