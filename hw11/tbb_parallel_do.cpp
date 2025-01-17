#include <iostream>
#include <fstream>
#include <string>
#include <tbb/parallel_for_each.h>
#include <tbb/blocked_range.h>

using namespace tbb;
using namespace std;

void process_line(string& line) {
    for (auto& c : line) {
        c = toupper(c);
    }
}

void process_file(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Couldn open file: " << filename << endl;
        return;
    }

    vector<string> lines;
    string line;
    while (getline(file, line)) {
        lines.push_back(line);
    }
    tbb::parallel_for_each(lines.begin(), lines.end(), process_line);

    for (const auto& processed_line : lines) {
        std::cout << processed_line << std::endl;
    }
}

int main() {
    // WITH BIG LOVE TO TWIN PEAKS
    //R.I.P. David Lynch

    string filename = "twinpeaks.txt"; 
    process_file(filename);
    return 0;
}
