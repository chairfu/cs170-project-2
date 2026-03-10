#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <limits>
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

        vector<double> currObject = data[i];
        int nnDist = DBL_MAX;
        int nnIndex = DBL_MAX;

        for (int j = 0; j < data.size(); j++) {

            double dist = 0;

            if (!featureSet.empty()){
                for (int feature : featureSet) {
                    cout << "current feature: " << feature << endl;
                    double diff = data[j][feature] - data[i][feature];
                    dist += diff * diff;
                }
            }

            int diff = data[j][currFeature] - data[i][currFeature];
            dist += diff * diff;

            dist = sqrt(dist);

            if (nnDist > dist) {
                nnDist = dist;
                nnIndex = j;
            }

        }

        if (data[nnIndex][0] == data[i][0]) {
            correct++;
        }

    }

    cout << correct / data.size() << endl;

    return (correct / data.size());

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

        int maxAcc = 0;
        int maxAccIndex = INT_MAX;
        for (int j = 1; j < data[0].size(); j++) {
            if (j == i) {continue;}

            cout << "Testing accuracy with feature: " << j << endl;

            int currAcc = accuracy(data, j, featureSet);
            cout << currAcc << endl;

            if (currAcc > maxAcc) {
                maxAcc = currAcc;
                maxAccIndex = j;
            }

        }

        cout << "Adding feature " << maxAccIndex << " , which gave an accuracy of " << maxAcc << "%" << endl;
        featureSet.insert(maxAccIndex);

    }

    return featureSet;

}


int main () {

    int fileNum;
    cout << "Which dataset would you like to use?" << endl;
    cin >> fileNum;

    string fileName;
    if (fileNum == 1) {
        fileName = "SanityCheck_DataSet__1.txt";
    }

    vector<vector<double>> data;
    data = readDataset(fileName);

    set<int> finalFeatures = forward_search(data);


}
