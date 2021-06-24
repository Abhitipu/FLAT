/*
Name : Abhinandan De
Roll No : 19CS10069

****INSTRUCTIONS****
    To compile : g++ 19CS10069_Assign1.cpp
    To run : ./a.exe inputFileName.txt
    If this doesnt work, try: a.exe inputFileName.txt
*/

#include<iostream>
#include<fstream>
#include<cstdio>

using namespace std;

#define tab "   "
typedef unsigned int uint;

int bitcount(uint x) {              // utility function for counting the set bits in an unsigned integer
    int ans = 0;
    for(int i = 0; i < 32; i++) {
        if((x >> i)&1)
            ans++;
    }
    return ans;
}

uint** readNFA(const char* filename, int& n, int& m, uint& sstates, uint& estates, uint** adj) {        // function to read NFA from input file
    freopen(filename, "r", stdin);
    cin >> n >> m;

    sstates = estates = 0u;          // start states

    adj = new uint*[n];        // the adjacency list of the nfa

    for(int i = 0; i < n; ++i) {
        adj[i] = new uint[m];
        for(int j = 0; j < m; ++j)
            adj[i][j] = 0u;
    }

    int x, y, z;
    for(; cin >> x && x != -1; )
        sstates |= (1u << x);

    for(; cin >> x && x != -1; )
        estates |= (1u << x);

    for(; cin >> x && x != -1; ) {
        cin >> y >> z;
        adj[x][y] |= (1u << z);      // a digraph is created as stated in question.
    }

    int nStartStates = bitcount(sstates);       // According to construction
    int nEndStates = bitcount(estates);

    cout << "+++ Input NFA\n";
    cout << tab << "Number of states: " << n << '\n';
    cout << tab << "Input alphabet: " << "{";
    for(int i = 0; i < m; i++) {
        cout << i;
        if(i != m-1)
            cout << ",";
    }
    cout << "}" << '\n';

    cout << tab << "Start states: " << "{";
    for(int i = 0; i < 32; i++) {
        if((sstates>>i)&1) {
            cout << i;
            nStartStates--;
            if(nStartStates)
                cout << ",";
        }
    }
    cout << "}" << '\n';

    cout << tab << "Final states: " << "{";
    for(int i = 0; i < 32; i++) {
        if((estates>>i)&1) {
            cout << i;
            nEndStates--;
            if(nEndStates)
                cout << ",";
        }
    }
    cout << "}" << '\n';

    cout << tab << "Transition function\n";

    for(int i = 0; i < n; i++) {
        for(int j = 0; j < m; j++) {
            int check = bitcount(adj[i][j]);
            cout << tab  << tab << "Delta (" << i << ", " << j << ") = {";
            for(int k = 0; k < n; k++) {
                if((adj[i][j]>>k)&1) {
                    cout << k;
                    check--;
                    if(check)
                        cout << ",";
                }
            }
            cout << "}\n";
        }
    }
    cout << '\n';

    return adj;
}

uint** subsetcons(int n, int m, uint sStates, uint eStates, uint** adj, uint** adj2) {      // utility function for subset construction

    uint nSubsets = (1u << n);          // total no of subsets is 2^n
    adj2 = new uint*[nSubsets];

    for(int i = 0; i < nSubsets; ++i) {
        adj2[i] = new uint[m];
        for(int j = 0; j < m; ++j)
            adj2[i][j] = 0u;
    }


    for(uint i = 0; i < nSubsets; i++) {
        for(int j = 0; j < n; j++) {
            if((i>>j)&1) {
                for(int k = 0; k < m; k++) {
                    adj2[i][k] = adj2[i][k] | adj[j][k];
                }
            }
        }
    }

    cout << "+++ Converted DFA\n";
    cout << tab << "Number of states: " << nSubsets << '\n';
    cout << tab << "Input alphabet: " << "{";
    for(int i = 0; i < m; i++) {
        cout << i;
        if(i != m-1)
            cout << ",";
    }
    cout << "}" << '\n';

    cout << tab << "Start state: " << sStates << '\n';

    int check = bitcount(eStates);
    uint fStates = nSubsets - (1u << (n-check));        // simply subtracting all the sets where we dont chose any final state.

    cout << tab << fStates << " final states." << '\n';

    cout << tab << "Transition function: ";

    if(nSubsets < 33) {                             // threshold set
        cout << "\n";
        cout << tab << "a\\p|";
        for(int i = 0; i < nSubsets; i++)
            printf("%3d", i);
        cout << '\n';
        cout << tab << "---+";
        for(int i = 0; i < nSubsets; i++)
            cout << "---";
        cout << '\n';

        for(int i = 0; i < m; i++) {
            cout << tab << i << "  |";
            for(int j = 0; j < nSubsets; j++) {
                printf("%3d", adj2[j][i]);
            }
            cout << '\n';
        }
        cout << "\n";
    }
    else {                      // skipped for large transition functions
        cout << "Skipped\n\n";
    }

    return adj2;
}

void findreachable(int n, int m, int i, bool* vis, uint** adj2, bool start = false) {       // utility function to find the reachable states using dfs traversal
    // start is defaulted with false it is set to true only for the root node

    vis[i] = true;
    for(int j = 0; j < m; j++) {
        if(!vis[adj2[i][j]]) {
            findreachable(n, m, adj2[i][j], vis, adj2);     // just like a normal dfs algorithm
        }
    }

    if(start) {         // only when it is the root(start) node ie when we complete our dfs traversal
        cout << "+++ Reachable states:\n";
        int last = -1;

        for(int j = (1 << n)-1; j >= 0; j--) {
            if(vis[j]) {
                last = j;
                break;
            }
        }
        cout << tab << "{";
        for(int j = 0; j < last; j++) {
            if(vis[j])
                cout << j << ",";
        }
        cout << last << "}\n\n";
    }
}

uint** rmunreachable(int n, int m, int startState, int eState, bool* vis, uint** adj2, uint** adj3, bool** finStates, int& newStart) {
    // utility function to get the dfa after removing unreachable states.
    // array finStates has to store changes... hence its address has to be passed.

    int newStates = 0;
    newStart = -1;

    for(int i = 0 ; i < (1 << n); i++)
        newStates += ((vis[i]) ? 1 : 0);

    int* prevState = new int[newStates];        // a kind of hash which maps from current index to prev index
    int nFinStates = 0;
    int cur = 0;

    for(uint i = 0; i < (1u << n); i++) {
        if(vis[i]) {
            prevState[cur] = i;
            for(int j = 0; j < n; j++) {
                if((eState>>j)&1 && (i>>j)&1) {
                    (*finStates)[cur] = true;
                    nFinStates++;
                    break;
                }
            }
            if(i == startState)     // this would the index of our new Start state
                newStart = cur;
            cur++;
        }
    }

    adj3 = new uint*[newStates];        // the new adjacency list
    for(int i = 0; i < newStates; i++) {
        adj3[i] = new uint[m];
        for(int k = 0; k < m; k++) {
            int checkval = adj2[prevState[i]][k];
            for(int x = 0; x < newStates; x++) {
                if(prevState[x] == checkval)  {
                    adj3[i][k] = x;         // assigning the new index
                    break;
                }
            }
        }
    }


    cout << "+++ Reduced DFA after removing unreachable states\n";
    cout << tab << "Number of states: " << newStates << '\n';
    cout << tab << "Input alphabet: " << "{";
    for(int i = 0; i < m; i++) {
        cout << i;
        if(i != m-1)
            cout << ",";
    }
    cout << "}" << '\n';
    cout << tab << "Start state: " << newStart << '\n';
    cout << tab << "Final states: {";

    for(int i = 0; i < newStates; i++) {
        if((*finStates)[i]) {
            cout << i;
            nFinStates--;
            if(nFinStates)
                cout << ",";
        }
    }
    cout << "}\n";

    cout << tab << "Transition function:\n";
    cout << tab << "a\\p|";
    for(int i = 0; i < newStates; i++)
        printf("%3d", i);
    cout << '\n';
    cout << tab << "---+";
    for(int i = 0; i < newStates; i++)
        cout << "---";
    cout << '\n';

    for(int i = 0; i < m; i++) {
        cout << tab << i << "  |";
        for(int j = 0; j < newStates; j++) {
            printf("%3d", adj3[j][i]);
        }
        cout << '\n';
    }
    cout << "\n";
    return adj3;
}

bool** findequiv(int n, int m, uint** adj3, bool* newFinStates, bool** matrix) {        // a utility function to group all equivalent states together

    for(int i = 0; i < n; i++) {            // initialization
        for(int j = i+1; j < n; j++) {
            if(newFinStates[i] != newFinStates[j])
                matrix[i][j] = true;    // i and j are equivalent
        }
    }

    int change;
    int it = 1;
    do {
        change = 0; // keeping track of the changes made.
        for(int i = 0; i < n; i++) {
            for(int j = i+1; j < n; j++) {
                if(matrix[i][j])            // NOT equivalent
                    continue;

                for(int k = 0; k < m && !matrix[i][j]; k++) {
                    int x = adj3[i][k];
                    int y = adj3[j][k];
                    if(x > y) {
                        int temp = x;
                        x = y;
                        y = temp;
                    }
                    if(matrix[x][y]) {      // condition for non equivalence of i and j
                        change++;
                        matrix[i][j] = true;
                    }
                }
            }
        }
        ++it;
    } while(change != 0); // stop if there are no more changes.

    bool* marked = new bool[n];     // detects if some state has been chosen in a group
    for(int i = 0; i < n; i++)
        marked[i] = false;

    cout << "+++ Equivalent states\n";
    for(int i = 0, z = 0; i < n; i++) {
        if(marked[i])
            continue;

        int last = -1;
        for(int j = n-1; j >= i && last == -1; j--) {
            if(matrix[i][j])
                continue;
            last = j;
        }

        cout << tab << "Group " << z << ": {";

        for(int j = i; j <= last; j++) {
            if(!matrix[i][j] && !marked[j]) {
                cout << j;
                if(j != last)
                    cout << ",";
                marked[j] = true;
            }
        }
        cout << "}\n";
        z++;
    }
    cout << '\n';
    return matrix;
}

void collapse(int n, int m, int start, bool* finStates, bool** matrix, uint** adj3) {       // utility function to print the details of collapsed dfa

    bool* marked = new bool[n];
    for(int i=0; i < n; i++)
        marked[i] = false;

    int nGroups = 0;
    int newStart;
    for(int i = 0; i < n; i++) {
        if(marked[i])
            continue;

        int last = -1;
        for(int j = n-1; j >= i && last == -1; j--) {
            if(matrix[i][j])
                continue;
            last = j;
        }

        for(int j = i; j <= last; j++) {
            if(!matrix[i][j] && !marked[j]) {
                marked[j] = true;
            }
        }
        nGroups++;
    }
    // getting the number of disjoint sets first

    int** adj4 = new int*[nGroups];
    bool* newFinStates = new bool[nGroups];
    int nFinStates = 0;

    for(int i = 0; i < nGroups; i++) {
        newFinStates[i] = false;
        adj4[i] = new int[m];
    }

    int* par = new int[n];      // stores the representative of the
    int* hash = new int[nGroups];

    int ctr = 0;
    for(int i=0; i < n; i++)
        marked[i] = false;

    for(int i = 0; i < n; i++) {
        if(marked[i])
            continue;

        int last = -1;
        for(int j = n-1; j >= i && last == -1; j--) {
            if(matrix[i][j])
                continue;
            last = j;
        }

        for(int j = i; j <= last; j++) {
            if(!matrix[i][j] && !marked[j]) {
                marked[j] = true;
                par[j] = ctr; //  stores the representative of a particular disjoint set.
                if(j == start)
                    newStart = ctr;
                if(finStates[j])
                    newFinStates[ctr] = true;
            }
        }
        hash[ctr] = i;
        ctr++;
    }

    for(int i = 0; i < nGroups; i++)
        nFinStates += ((newFinStates[i]) ? 1 : 0);

    for(int i = 0; i < nGroups; i++) {
        for(int k = 0; k < m; k++) {
            adj4[i][k] = par[adj3[hash[i]][k]];     // constructing the new adjacency list
        }
    }

    // Printing out details

    cout << "+++ Reduced DFA after collapsing equivalent states\n";
    cout << tab << "Number of states: " << nGroups << '\n';
    cout << tab << "Input alphabet: " << "{";
    for(int i = 0; i < m; i++) {
        cout << i;
        if(i != m-1)
            cout << ",";
    }
    cout << "}\n";
    cout << tab << "Start state: " << newStart << '\n';
    cout << tab << "Final states: {";
    for(int i = 0; i < nGroups; i++) {
        if(newFinStates[i]) {
            cout << i;
            nFinStates--;
            if(nFinStates)
                cout << ",";
        }
    }
    cout << "}\n";

    cout << tab << "Transition function:\n";
    cout << tab << "a\\p|";
    for(int i = 0; i < nGroups; i++)
        printf("%3d", i);
    cout << '\n';
    cout << tab << "---+";
    for(int i = 0; i < nGroups; i++)
        cout << "---";
    cout << '\n';

    for(int i = 0; i < m; i++) {
        cout << tab << i << "  |";
        for(int j = 0; j < nGroups; j++) {
            printf("%3d", adj4[j][i]);
        }
        cout << '\n';
    }
    cout << "\n";

}

int main(int argc, char* argv[]) {
    // freopen("output.txt", "w", stdout);

    int n, m;
    uint sStates;
    uint eStates;
    uint** adj;
    uint** adj2;
    uint** adj3;

    if(argc < 2) {
        cout << "Please specify input file!!\n";
        exit(-1);
    } else {
        ifstream infile;
        infile.open(argv[1]);

        if(infile.fail()) {
            cout << "File does not exist\n";
            exit(-1);
        }
        else
            infile.close();
    }

    adj = readNFA(argv[1], n, m, sStates, eStates, adj);        // Part 1

    adj2 = subsetcons(n, m, sStates, eStates, adj, adj2);       // Part 2

    bool* vis = new bool[1<<n];                                  // Part 3
    for(int i = 0; i < (1<<n); i++)
        vis[i] = false;

    findreachable(n, m, sStates, vis, adj2, true);

    int newStates = 0, newStart;                                // Part 4
    for(int i = 0; i < (1 << n); i++) {
        newStates += ((vis[i]) ? 1 : 0);
    }
    adj3 = new uint*[newStates];        // new adj list for new DFA
    bool* newFinStates = new bool[newStates];
    for(int i = 0; i < newStates; i++) {
        adj3[i] = new uint[m];
        for(int j = 0; j < m; j++)
            adj3[i][j] = 0;
        newFinStates[i] = false;
    }
    adj3 = rmunreachable(n, m, sStates, eStates, vis, adj2, adj3, &newFinStates, newStart);

    bool** matrix = new bool*[newStates];       // for implementing the minimization algorithm
    for(int i = 0; i < newStates; i++) {
        matrix[i] = new bool[newStates];
        for(int j = 0; j < newStates; j++)
            matrix[i][j] = false;
        matrix[i][i] = false;
    }

    matrix = findequiv(newStates, m, adj3, newFinStates, matrix);           // Part 5

    collapse(newStates, m, newStart, newFinStates, matrix, adj3);

    return 0;
}
