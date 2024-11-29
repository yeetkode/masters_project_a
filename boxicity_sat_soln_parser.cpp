#include <bits/stdc++.h>

using namespace std;

int begin(int n, int d, int v, int dim, int i){
	return (2*n*d)*v + n*dim + i;
}
int end(int n, int d, int v, int dim, int i){
	return (2*n*d)*v + n*(dim+d) + i;
}

int main() {
	int n, m, d;
	cout << "Enter integers n, m, d: ";
	cin >> n >> m >> d;
    string fileName;
    cout << "Enter the file name: ";
    cin >> fileName;

    ifstream infile(fileName);
    if(!infile){
        cerr << "Error: Could not open file " << fileName << endl;
        return 1;
    }

    string line;
    getline(infile, line);
    infile.close();

    istringstream iss(line);
    string word;
    iss >> word;

    if(word != "SAT"){
        cerr << "Error: The file must start with the word 'SAT'" << endl;
        return 1;
    }

    vector<int> numbers;
    int num;
    numbers.push_back(0);
    while (iss >> num) {
		numbers.push_back(num);
    }
    ofstream outfile("SAT_interpretation.txt");
    for(int v = 0; v < n; ++v){
    	outfile << "Vertex " << v << ": ";
    	for(int dim = 0; dim < d; ++dim){
    		outfile << '[';
    		for(int i = 1; i <= n; ++i){
    			if(numbers[begin(n, d, v, dim, i)] == begin(n, d, v, dim, i)){
    				outfile << i << ',' << ' ';
    				break;
    			}
    		}
    		for(int i = 1; i <= n; ++i){
    			if(numbers[end(n, d, v, dim, i)] == end(n, d, v, dim, i)){
    				outfile << i << ']';
    				break;
    			}
    		}
    		if(dim != d-1){
    			outfile << ',';
    		}
    		outfile << ' ';
    	}
    	outfile << '\n';
    }
}
