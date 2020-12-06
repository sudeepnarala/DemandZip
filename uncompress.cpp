#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <utility>
#include <queue>
#include <math.h>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

int expand(string fname)
{
    ifstream compressed(fname, ios::binary);

    char check_magic;
    compressed >> check_magic;
    if(check_magic != 0x53)
    {
        return -1;
    }

    int num_partitions;
    char temp2[4];
    compressed.read(temp2, 4);
    num_partitions += temp2[0];
    num_partitions += (temp2[1]<<8);
    num_partitions += (temp2[2]<<16);
    num_partitions += (temp2[3]<<24);
    printf("There are %d partitions\n", num_partitions);
    int partitions[num_partitions];
    int temp;
    for(int i=0; i<num_partitions; i++)
    {
        char temp[4];
        int a = 0;
        compressed.read(temp, 4);
        a = 0;
        a |= (temp[0] & 0x000000FF);
        a |= ((temp[1] << 8) & 0x0000FF00);
        a |= ((temp[2] << 16) & 0x00FF0000);
        a |= ((temp[3] << 24) & 0xFF000000);
        partitions[i] = a;
    }

    int graph_size;
    compressed >> graph_size;
    char graph[graph_size];
    compressed.read(graph, graph_size);
    // Traverse graph and create new file with file data
    ofstream output_file("test.csv", ios::binary);
    char buf;
    int x = 0;
    int location = 1;
    int count = 0;
    int curr = compressed.tellg();          // Tell to find current position (which is end of file)
    compressed.seekg(0, compressed.end);    // Seek to end of file
    int length = compressed.tellg();
    length -= curr;
    compressed.seekg(curr);
    char* buffer = new char[length]; 
    compressed.read(buffer, length);

    for(int i=0; i<length; i++)
    {
        buf = buffer[i];
        while(x <= 7)
        {
            if(((location*2-1) >= graph_size) || ((graph[location*2-1] == 0) && (graph[location*2+1-1] == 0)))
            {
                output_file << graph[location-1];
                location = 1;
            }
            if(buf & (1 << (7-x)))
            {
                location = (location*2)+1;
            }
            else
            {
                location = (location*2);
            }
            x += 1;
        }
        x = 0;
    }
    return 0;
}

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        cout << "Need to supply at least 1 argument" << endl;
        exit(1);
    }
    int ret = expand(argv[1]);
    if(ret == -1)
    {
        cout << "Magic signature doesn't match" << endl;
        exit(1);
    }
}