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

struct node {
    int num;
    char value;
    struct node* left;
    struct node* right;
};

vector<pair<char, string> > codes;

struct cmp {
    bool operator() (struct node* n1, struct node* n2)
    {
        return n1->num > n2->num;
    }
};

void get_codes(struct node* curr, string code)
{
    if(curr == NULL)
    {
        return;
    }
    if(curr->left == NULL && curr->right == NULL)
    {
        codes.push_back(make_pair(curr->value, code));
    }
    get_codes(curr->left, code + "0");
    get_codes(curr->right, code + "1");
}

void create_demand_unzip(char graph[], int size, string fname)
{
    // Store the binary tree --> Store -1 if not leaf char
    for(int i=0; i<size; i++)
    {
        graph[i] = -1;
    }

    for(auto c=codes.begin(); c != codes.end(); c++)
    {
        int loc = 1;
        for(int i=0; i != c->second.length(); i++)
        {
            int add = c->second[i] == '0' ? 0 : 1;
            loc = (2*loc) + add;
        }
        graph[loc-1] = c->first;
        graph[(2*loc)-1] = 0;
        graph[(2*loc+1)-1] = 0;
    }
    bitset<8> t(graph[295*2-1]);
    string base_fname = "";
    for(int i=0; i<fname.length() && fname[i] != '.'; i++)
    {
        base_fname += fname[i];
    }
    string dzip_fname = base_fname + ".dzip";
    ofstream compressed(dzip_fname, ios::binary);
    int actual_graph_size = size;
    for(int i=size-1; i>=0; i--)
    {
        if(graph[i] != -1)  // and graph[i] != 0 because don't need leaf nodes
        {
            break;
        }
        actual_graph_size = i;  // After because, example, imagine full graph --> want to include all --> actual_graph_size = size.
    }

    // BEGINNING OF HEADER

    char magic = 0x53;
    // Magic byte header
    compressed << magic;

    // Use size in order to shrink the graph / not just put a bunch of 0xFF's into the compressed file
    // Store binary size of graphs
    // Next actual_graph_size bytes contains the graph for decoding the dzip file 
    compressed << actual_graph_size;

    compressed.write(graph, actual_graph_size);
    // Store the bit breaking within the file in order to partially decompress

    // ENDING OF HEADER
    map<char, string> codes_map;
    for(auto a=codes.begin(); a != codes.end(); a++)
    {
        codes_map[a->first] = a->second;
    }

    ifstream uncompressed;
    char* buffer;
    uncompressed.open(fname, ios::binary);
    uncompressed.seekg(0, uncompressed.end);    // Seek to end of file
    int length = uncompressed.tellg();          // Tell to find current position (which is end of file)
    buffer = new char[length];
    uncompressed.seekg(0, uncompressed.beg);
    uncompressed.read(buffer, length);              // Read contents of file into buffer
    int x = 0;
    char curr = 0;
    int total = 0;
    for(int i=0; i < length; i++)   // Go through uncompressed file
    {
        string s = codes_map[buffer[i]];    // s is the code
        for(int j=0; j != s.length(); j++)
        {
            char b = s[j] == '0' ? 0 : 1;
            curr = curr | (b << (7-x));
            x += 1;
            if(x == 8)
            {
                bitset<8> b(curr);
                compressed << curr;
                curr = 0;
                x = 0;
                total += 1;
            }
        }
    }
    compressed.close();
}


void do_huffman(string fname)
{
    // Get bytes into "buffer"
    ifstream uncompressed;
    char* buffer;
    uncompressed.open(fname, ios::binary);
    uncompressed.seekg(0, uncompressed.end);    // Seek to end of file
    int length = uncompressed.tellg();          // Tell to find current position (which is end of file)
    buffer = new char[length];
    uncompressed.seekg(0, uncompressed.beg);
    uncompressed.read(buffer, length);              // Read contents of file into buffer

    // Find frequency of each byte
    map<char, int> freq;

    for(int i=0; i < length; i++)
    {
        if(freq.find(buffer[i]) == freq.end())  // Not in map
        {
            freq[buffer[i]] = 1;
        }
        else
        {
            freq[buffer[i]] += 1;
        }
    }

    priority_queue<struct node*,vector<struct node*>,cmp > q;
    
    for(auto it = freq.begin(); it != freq.end(); it++)
    {
        struct node* n = new node();
        n->value = it->first;
        n->num = it->second;
        n->left = NULL;
        n->right = NULL;
        q.push(n);
    }

    // Algorithm:
    // Until there are 0 or 1 elements left, pop off 2 and create new internal node
    while(q.size() > 1)
    {
        struct node* left = q.top();
        q.pop();
        struct node* right = q.top();
        q.pop();
        struct node* n = new node();
        n->left = left;
        n->right = right;
        n->num = left->num + right->num;
        n->value = -1;
        q.push(n);
    }

    // Get huffman codes
    struct node* root = q.top();

    get_codes(root, "");

    int m = 0;

    for(auto a = codes.begin(); a != codes.end(); a++)
    {
        // cout << a->first << " - " << a->second << endl;
        if((a->second).length() > m)
        {
            m = (a->second).length();
        }
    }
    m += 1;
    int arr_size = (int) (pow(2, m)-1);
    char graph[arr_size];
    create_demand_unzip(graph, arr_size, fname);
}

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        cout << "Need to supply at least 1 argument" << endl;
        exit(1);
    }
    do_huffman(argv[1]);
    return 0;
}