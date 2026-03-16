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

double accuracy(const vector<vector<double>>& data, int currFeature, const set<int>& featureSet) {

    double correct = 0.0;

    for (int i = 0; i < data.size(); i++) {

        double nnDist = DBL_MAX;
        int nnIndex = -1;

        for (int j = 0; j < data.size(); j++) {

            if (j == i) {continue;}

            double dist = 0;

            if (!featureSet.empty()){
                for (int feature : featureSet) {
                    double diff = data[j][feature] - data[i][feature];
                    dist += diff * diff;
                }
            }

            if (j > 0) {
                double diff = data[j][currFeature] - data[i][currFeature];
                dist += diff * diff;
            }

            if (nnDist > dist) {
                nnDist = dist;
                nnIndex = j;
            }

        }

        if (data[nnIndex][0] == data[i][0]) {
            correct++;
        }

    }

    //cout << double(correct / data.size()) * 100 << "%" << endl;
    return double(correct / data.size());

}

void forwardSearch(const vector<vector<double>>& data) {

    set<int> featureSet;

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

            //cout << "Testing accuracy with feature: " << j << endl;

            double currAcc = accuracy(data, j, featureSet);

            if (currAcc > maxAcc) {
                //cout << "here" << endl;
                maxAcc = currAcc;
                maxAccIndex = j;
            }

        }

        cout << "Adding feature " << maxAccIndex << ", which gave an accuracy of " << maxAcc  * 100.0 << "%" << endl;
        featureSet.insert(maxAccIndex);

    }

}

void backwardElimination(const vector<vector<double>>& data) {

    set<int> featureSet;

    for (int i = 1; i < data[0].size(); i++) {
        featureSet.insert(i);
    }

    for (int i = 1; i < data[0].size() - 1; i++) {

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
            if (!featureSet.count(j)) continue;

            //cout << "Testing accuracy without feature: " << j << endl;

            set<int> tempSet = featureSet;
            tempSet.erase(j);

            double tempSetAcc = accuracy(data, -1, tempSet);

            if (tempSetAcc > maxAcc) {
                maxAcc = tempSetAcc;
                maxAccIndex = j;
            }

        }

        featureSet.erase(maxAccIndex);
        double newAcc = accuracy(data, -1, featureSet);

        cout << "Removing feature " << maxAccIndex << ", accuracy of set is now " << newAcc  * 100.0 << "%" << endl;

    }

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
    else if (fileNum == 3) {
        fileName = "CS170_Small_DataSet__110.txt";
    }
    else if (fileNum == 4) {
        fileName = "CS170_Large_DataSet__62.txt";
    }

    vector<vector<double>> data;
    data = readDataset(fileName);

    cout << "Which method would you like to use? 1 for forward search, 2 for backward elimination" << endl;

    int method;
    cin >> method;

    if (method == 1) {
        forwardSearch(data);
    }
    else if (method == 2) {
        backwardElimination(data);
    }

}
