#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <utility>
#include <queue>
using namespace std;

struct node {
    int num;
    char value;
    struct node* left;
    struct node* right;
};

vector<pair<char, string> > v;


// template<class T>
// bool bigger(T t1, T t2)
// {
//     return t1.value > t2.value;
// }

// bool operator>(const struct node n1, const struct node n2) 
// {
//     return n1.num > n2.num;
// }

// bool cmp(pair<char, int> p1, pair<char, int> p2)
// {
//     return p1.second > p2.second;
// }

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
        v.push_back(make_pair(curr->value, code));
    }
    get_codes(curr->left, code + "0");
    get_codes(curr->right, code + "1");
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
    // 1. Sort elements by frequency

    // vector<pair<char, int> > t;

    // for(auto it = freq.begin(); it != freq.end(); it++)
    // {
    //     t.push_back(make_pair(it->first, it->second));
    // }

    // for(auto it = t.begin(); it != t.end(); it++)
    // {
    //     cout << hex << (int) it->first << " - " << it->second << endl;
    // }

    // cout << "______________" << endl;

    // sort(t.begin(), t.end(), cmp);

    // for(auto it = t.begin(); it != t.end(); it++)
    // {
    //     cout << hex << (int) it->first << " - " << it->second << endl;
    // }

    // 2. Create huffman tree --> Branch with 0 and 1 becomes the subtree
    /*

    */

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

    for(auto a = v.begin(); a != v.end(); a++)
    {
        cout << a->first << " - " << a->second << endl;
    }



}

int main()
{
    do_huffman("trial.txt");
    return 0;
}
