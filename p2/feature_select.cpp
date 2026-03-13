#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <limits>
#include <cmath>
using namespace std;

//did not write this
vector<vector<double>> readDataset(const string& filename) {
    ifstream file(filename);
    vector<vector<double>> data;

    string line;

    while (getline(file, line)) {
        vector<double> row;
        stringstream ss(line);
        double value;

        while (ss >> value) {
            row.push_back(value);
        }

        data.push_back(row);
    }

    return data;
}

double accuracy (const vector<vector<double>>& data, int currFeature, const set<int>& featureSet) {

    double correct = 0.0;

    for (int i = 0; i < data.size(); i++) {

        double nnDist = DBL_MAX;
        int nnIndex = -1;

        for (int j = 0; j < data.size(); j++) {

            if (j == i) {continue;}

            double dist = 0;

            if (!featureSet.empty()){
                for (int feature : featureSet) {
                    //cout << "current feature: " << feature << endl;
                    double diff = data[j][feature] - data[i][feature];
                    dist += diff * diff;
                }
            }

            double diff = data[j][currFeature] - data[i][currFeature];
            dist += diff * diff;

            if (nnDist > dist) {
                nnDist = dist;
                nnIndex = j;
            }

        }

        if (data[nnIndex][0] == data[i][0]) {
            correct++;
        }

    }

    return double(correct / data.size());

}

set<int> forward_search(const vector<vector<double>>& data) {

    set<int> featureSet;

    cout << data[0].size() << endl;

    for (int i = 1; i < data[0].size(); i++) {

        if (!featureSet.empty()) {
            cout << "Current feature set: ";
            for (int feature : featureSet) {
                cout << feature << " ";
            }

            cout << endl;
        }

        double maxAcc = 0.0;
        int maxAccIndex = -1;
        for (int j = 1; j < data[0].size(); j++) {
            if (featureSet.count(j)) continue;

            cout << "Testing accuracy with feature: " << j << endl;

            double currAcc = accuracy(data, j, featureSet);
            cout << currAcc << endl;

            if (currAcc > maxAcc) {
                cout << "here" << endl;
                maxAcc = currAcc;
                maxAccIndex = j;
            }

        }

        cout << "Adding feature " << maxAccIndex << " , which gave an accuracy of " << maxAcc  * 100.0 << "%" << endl;
        featureSet.insert(maxAccIndex);

    }

    return featureSet;

}

set<int> backward_elimination(const vector<vector<double>>& data) {

    //nothing here yet

}


int main () {

    int fileNum;
    cout << "Which dataset would you like to use?" << endl;
    cin >> fileNum;

    string fileName;
    if (fileNum == 1) {
        fileName = "SanityCheck_DataSet__1.txt";
    }
    else if (fileNum == 2) {
        fileName = "SanityCheck_DataSet__2.txt";
    }

    vector<vector<double>> data;
    data = readDataset(fileName);

    set<int> finalFeatures = forward_search(data);


}
