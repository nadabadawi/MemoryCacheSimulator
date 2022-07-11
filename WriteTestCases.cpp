#include <iostream>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <string>
using namespace std;

const int n = 1;
const int line_size = 32;
const int CACHE_SIZE = 64 * 1024;
const int ADDRESS_SIZE = 32;

int byte_offset_bits = log2(line_size);
int index_bits = log2(CACHE_SIZE / (n * line_size));
int tag_bits = ADDRESS_SIZE - index_bits - byte_offset_bits;

string DecToBin(int decimal);


int main()
{
    ofstream binfile, decfile;
    binfile.open("BinaryTest.txt");
    decfile.open("DecimalTest.txt");
    srand(time(0));
    int valid, tag, data;
    int max_tag = pow(2, tag_bits);
    int max_data = pow(2, line_size * 8);
    for (int i = 0; i < (CACHE_SIZE / line_size); i++) //Each iteration is a line
    {
        valid = rand() % 2;
        tag = rand() % max_tag;
        data = rand() % max_data;
        binfile << DecToBin(valid);
        decfile << valid;
        if (valid)
        {
            binfile << ' ' << DecToBin(tag) << ' ' << DecToBin(data);
            decfile << ' ' << tag << ' ' << data;
        }
        binfile << '\n';
        decfile << '\n';
    }

    //decfile << (rand() % max_tag) << ' ' << (pow(2, line_size * 8));
    //for (int i = 0; i < (CACHE_SIZE / line_size); i++) //Each iteration is a line
    //{
    //    valid = rand() % 2;
    //    binfile << DecToBin(valid) << ' ';
    //    if (valid)
    //        binfile << DecToBin(rand() % max_tag) << ' ' << DecToBin(pow(2, line_size * 8));
    //    binfile << '\n';
    //}

    binfile.close();
    decfile.close();
    return 0;
}


string DecToBin(int decimal) 
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