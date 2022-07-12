//  Created by Nermien Elassy on 10/07/2022.

#include<iostream>
using namespace std;


//Creating the struct which acts as a line in the cache
struct line
{
    int valid_bit; //valid bit
    int TAG; //tag
    //We could store the data as well, but it isn't needed
};

//A function to build the cache and initialize it
line* cache_builder(int sets_num, int n_lines_per_set)
{
    line* Cache = new line[sets_num * n_lines_per_set];
    return Cache
}

//A function to read from the cache and set hit and miss values
//It takes a 32-bit address, HIT, MISS, Cache
void read_from_cache(unsigned int address, &int HIT, &int MISS, line* Cache, int n_lines_per_set)
{
    //getting index(It will act as the set number)
    
    //shifting to the left 4 bits to get rid of the offset
    unsigned int index = address << 4;
    index = index & 0b111111; //Anding with 6 ones to get rid of TAG
    
    //getting TAG
    
    //shifting to the left by 10 bits to get rid of the offset and index
    unsigned int TAG = address << 10;
    
    //A boolean flag to check if we found the data inside the cache or not
    bool flag = false;
    
    //Looping on the lines of the selected set to search the desired location
    for(int i=0; i<n_lines_per_set; i++)
    {
        if (Cache[index][i].TAG == TAG && Cache[index][i].valid_bit == 1) //Data Found
        {
            HIT++;
            flag = true;
            break
        }
    }
    if(!flag) // Data isn't found
    {
        //writing the new tag inside the cache and valid_bit to 1
        //Note: We are doing it random as proved to be good
        
        //Generating a random number within the range of lines per set
       unsigned int tag_replaced = 0 + (rand() % ( n_lines_per_set - 0 + 1 ) );
        Cache[index][tag_replaced].TAG = TAG;
        Cache[index][tag_replaced].valid_bit = 1;
        MISS++;
    }
}

