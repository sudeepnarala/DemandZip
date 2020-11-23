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
    int graph_size;
    compressed >> graph_size;
    char graph[graph_size];
    compressed.read(graph, graph_size);
    // Traverse graph and create new file with file data
    ofstream output_file("trial.csv", ios::binary);
    char buf;
    int x = 0;
    int location = 1;
    int count = 0;

    
    while(!compressed.eof())
    {
        count += 1;
        compressed >> buf;
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
    cout << count << endl;
    return 0;
}

int main()
{
    int ret = expand("trial.dzip");
    if(ret == -1)
    {
        cout << "Magic signature doesn't match" << endl;
    }
}