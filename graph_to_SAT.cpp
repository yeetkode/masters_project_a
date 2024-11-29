// The following code reduces a query for testing a graph for boxicity at most d 
// to CNF-SAT, written in DIMACS form. We have n*d variables 'begin' and n*d 
// variables 'end' for each vertex v. Each vertex box is represented by the
// 'begin' (smallest coordinates) and 'end' (largest coordinates). Each coordinate
// is stored as a bunch of zeroes followed by a bunch of ones, where the position 
// of the first 1 encodes the integer.

// Constraints:
// 1) Begin, End look like 0^a1^b
// 2) Begin <= End coordinate-wise
// 3) a) Vertex intersect clauses
// 3) b) Vertex non-intersect clauses
// 4) Write into DIMACS

#include <bits/stdc++.h>

using namespace std;

typedef pair<int,int> pii;
#define pb push_back

int begin(int n, int d, int v, int dim, int i){
	return (2*n*d)*v + n*dim + i;
}
int end(int n, int d, int v, int dim, int i){
	return (2*n*d)*v + n*(dim+d) + i;
}

// Function to read input from the Terminal
void readFromTerminal(int& n, int& m, int& d, set<pii>& edges) {
    cout << "Enter n, m, and d:\n";
    cin >> n >> m >> d;

    cout << "Enter " << m << " edges (pairs of integers):\n";
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        edges.insert({u, v});
        edges.insert({v, u});
    }
}

// Function to read input from a file
void readFromFile(const string& filePath, int& n, int& m, set<pii>& edges) {
    ifstream file(filePath);
    if(!file){
        throw runtime_error("File not found: " + filePath);
    }

    file >> n >> m; // Read n and m from the file

    for (int i = 0; i < m; ++i) {
        int u, v;
        file >> u >> v;
        edges.insert({u, v});
        edges.insert({v, u});
    }
    file.close();
}
int main(int argc, char* argv[]){
	
	// n = number of vertices, m = number of edges, d = boxicity to be tested

	int n, m, d;

	vector<vector<int>> clauses;

	// Take input and store the set of edges as pairs of vertices on [0, n-1]

	set<pii> edges_set;

	bool use_file = false;
    string input_source;
	
    if(argc > 1) {
        if(string(argv[1]) == "--file"){
            use_file = true;
            if(argc > 2) {
                input_source = argv[2];
                if(argc > 3){
                    d = stoi(argv[3]);
                }
                else{
                    cerr << "Error: 'd' must be provided in file mode." << '\n';
                    return 1;
                }
            }
            else{
                cerr << "Error: No file path provided for --file flag." << '\n';
                return 1;
            }
        }
        else if(string(argv[1]) == "--input"){
            use_file = false;
        }
        else{
            cerr << "Error: Unknown argument. Use --file or --input." << '\n';
            return 1;
        }
    }
    else{
        cerr << "Error: No arguments provided. Use --file or --input." << '\n';
        return 1;
    }

    // Read the input based on the source
    try{
        if(use_file){
            readFromFile(input_source, m, n, edges_set);
            cout << "Reading from file '" << input_source << "':\n";
        }
        else{
            readFromTerminal(m, n, d, edges_set);
            cout << "Reading from terminal input:\n";
        }
    }
    catch(const exception& e){
        cerr << e.what() << '\n';
        return 1;
    }

	// Global counter for number of variables, counts begin and end by default
	int variables_total = 2*n*n*d;

	// Vertex-boxes are modelled by: coordinate k is represented by k-1 zeroes
	// followed by n-k+1 ones. Store smallest and largest vertices to store 
	// the entire box.

	// 1) Make begin, end work as above

	for(int v = 0; v < n; ++v){
		for(int dim = 0; dim < d; ++dim){
			for(int i = 1; i < n; ++i){
				clauses.pb({begin(n, d, v, dim, i+1), -begin(n, d, v, dim, i)});
				clauses.pb({end(n, d, v, dim, i+1), -end(n, d, v, dim, i)});
			}
			clauses.pb({begin(n, d, v, dim, n)});
			clauses.pb({end(n, d, v, dim, n)});
		}
	}

	// 2) Begin's coordinates never exceed end's

	for(int v = 0; v < n; ++v){
		for(int dim = 0; dim < d; ++dim){
			for(int i = 1; i <= n; ++i){
				clauses.pb({begin(n, d, v, dim, i), -end(n, d, v, dim, i)});
			}
		}
	}

	// 3) Interval conditions

	for(int u = 0; u < n; ++u){
		for(int v = u+1; v < n; ++v){
			if(edges_set.find({u,v}) != edges_set.end()){ // a) If edge exists
				for(int dim = 0; dim < d; ++dim){
					for(int i = 1; i <= n; ++i){
						clauses.pb({begin(n, d, u, dim, i), -end(n, d, v, dim, i)});
						clauses.pb({begin(n, d, v, dim, i), -end(n, d, u, dim, i)});
					}
				}
			}
			else{ // b) If edge doesn't exist
				vector<int>mega_clause(2*n*d, 0);
				for(int i = 0; i < mega_clause.size(); ++i){
					mega_clause[i] = variables_total + i + 1;
				}
				clauses.pb(mega_clause);
				for(int dim = 0; dim < d; ++dim){
					for(int i = 1; i <= n; ++i){
						++variables_total;
						clauses.pb({-variables_total, -begin(n, d, u, dim, i)});
						clauses.pb({-variables_total, end(n, d, v, dim, i)});
						clauses.pb({variables_total, begin(n, d, u, dim, i), -end(n, d, v, dim, i)});
					}
					for(int i = 1; i <= n; ++i){
						++variables_total;
						clauses.pb({-variables_total, -begin(n, d, v, dim, i)});
						clauses.pb({-variables_total, end(n, d, u, dim, i)});
						clauses.pb({variables_total, begin(n, d, v, dim, i), -end(n, d, u, dim, i)});
					}
				}
			}
		}
	}
	
	// 4) Write into DIMACS

	ofstream outfile("SAT_reduction.cnf");
	if (!outfile.is_open()){
        cerr << "Error: Could not open the output file." << '\n';
        return 1;
    }
	outfile.seekp(0, ios::beg);  // Set the file pointer to the beginning of the file
	outfile << "p cnf " << variables_total << ' ' << clauses.size() << '\n';
	for(int i = 0; i < clauses.size(); ++i){
		for(int j = 0; j < clauses[i].size(); ++j){
			outfile << clauses[i][j] << ' ';
		}
		outfile << 0 << "\n";
	}
	outfile.close();
}
