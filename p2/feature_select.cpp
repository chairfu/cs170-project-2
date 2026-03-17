#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <limits>
#include <cmath>
#include <chrono>
using namespace std::chrono;
using namespace std;

//Code here is adapted from this thread:
//https://stackoverflow.com/questions/64571919/c-readfile-with-varying-element-amounts-per-line#:~:text=%3E%20filename;%20readFile.-,open(filename);%20if%20(!readFile)%20%7B%20std::cout,28%2C%202020%20at%2011:45
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

void printSet (const set<int>& featureSet) {

    vector<int> toPrint(featureSet.begin(), featureSet.end());

    cout << "{";
    for (int i = 0; i < toPrint.size(); i++) {
        cout << toPrint[i];
        if (i != toPrint.size() - 1) {cout << ", ";}
    }
    cout << "}";

}

vector<vector<vector<double>>> cacheInit (const vector<vector<double>>& data) {

    //for each feature (data[0].size), store distance from one point i to every other point j 
    //layers of cache: feature, point i, point j
    vector<vector<vector<double>>> cache(data[0].size(), vector<vector<double>>(data.size(), vector<double>(data.size())));

    for (int feature = 1; feature < data[0].size(); ++feature) {
        for (int i = 0; i < data.size(); i++) {
            for (int j = 0; j < data.size(); j++) {
                
                double diff = data[i][feature] - data[j][feature];
                double dist = diff * diff;

                cache[feature][i][j] = dist;

            }
        }
    }

    return cache;

}

double defaultRate(const vector<vector<double>>& data) {

    //implementation for the default rate as given on Machine Learning 002 slides
    double c1 = 0.0;
    double c2 = 0.0;

    for (int i = 0; i < data.size(); i++) {

        if (data[i][0] == 1.0000000e+00) {c1++;}
        if (data[i][0] == 2.0000000e+00) {c2++;}

    }

    double defaultRate = (c1 > c2) ? c1 / double(data.size()) : c2 / double(data.size());

    return defaultRate;

}

double accuracy(const vector<vector<double>>& data, const vector<vector<vector<double>>>& cache, int currFeature, const set<int>& featureSet, const double bestSoFar) {

    int rows = data.size();

    double correct = 0.0;
    double incorrect = 0.0;
    double incorrectThresh = rows - (rows * bestSoFar);

    //iterating through a vector is faster. I was trying anything at this point :,)
    vector<int> featureVec (featureSet.begin(), featureSet.end());

    for (int i = 0; i < rows; i++) {

        double nnDist = DBL_MAX;
        int nnIndex = -1;

        for (int j = 0; j < rows; j++) {

            if (j == i) {continue;}

            double dist = 0;

            for (int feature : featureVec) {
                dist += cache[feature][i][j];
                //if distance already exceeds the nearest neighbor's distance, there's no need to continue
                if (dist > nnDist) {
                    break; 
                }
            }

            if (dist <= nnDist && currFeature > -1) {
                dist += cache[currFeature][i][j];
            }

            if (nnDist > dist) {
                nnDist = dist;
                nnIndex = j;
            }

        }
        
        if (data[nnIndex][0] == data[i][0]) {
            correct++;
        }
        else {
            incorrect++;
        }

        //if this block gets entered that means that the accuracy is already lower than our best so far, no need to continue computing this feature
        if (incorrect >= incorrectThresh) {
            return double(correct / rows);
        }

    }

    return double(correct / rows);

}

set<int> forwardSearch(const vector<vector<double>>& data, const vector<vector<vector<double>>>& cache) {

    set<int> featureSet;
    set<int> bestSet;
    double bestSetAcc = 0.0;

    for (int i = 1; i < data[0].size(); i++) {

        double maxAcc = 0.0;
        int maxAccIndex = -1;
        for (int j = 1; j < data[0].size(); j++) {
            if (featureSet.count(j)) continue;

            cout << "\tUsing feature(s) {";
            for (int feature : featureSet) {
                cout << feature << ", ";
            }

            cout << j << "}";

            double currAcc = accuracy(data, cache, j, featureSet, maxAcc);

            cout << " accuracy is " << currAcc * 100 << "%" << endl;

            if (currAcc > maxAcc) {
                maxAcc = currAcc;
                maxAccIndex = j;
            }

        }

        featureSet.insert(maxAccIndex);

        cout << "Feature set " ;
        printSet(featureSet);

        cout << " was best, accuracy is " << maxAcc * 100 << "%" << endl;

        if (maxAcc > bestSetAcc) {
            bestSet = featureSet;
            bestSetAcc = maxAcc;
        }

    }

     cout << "Search complete! The best set was: "; 
     printSet(bestSet);
     cout << " which has an accuracy of " << bestSetAcc * 100 << "%" << endl;

    return bestSet;

}

void backwardElimination(const vector<vector<double>>& data, const vector<vector<vector<double>>>& cache) {

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

        vector<int> potential(featureSet.begin(), featureSet.end());

        for (int j : potential) {

            featureSet.erase(j);
            double tempSetAcc = accuracy(data, cache, -1, featureSet, maxAcc);
            featureSet.insert(j);

            if (tempSetAcc > maxAcc) {
                maxAcc = tempSetAcc;
                maxAccIndex = j;
            }

        }

        featureSet.erase(maxAccIndex);

        cout << "Removing feature " << maxAccIndex << ", accuracy of set is now " << maxAcc  * 100.0 << "%" << endl;

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
    cout << "Which method would you like to use? 1 for forward search, 2 for backward elimination" << endl;

    int method;
    cin >> method;

    vector<vector<double>> data;
    data = readDataset(fileName);

    cout << "This dataset has " << data[0].size() - 1 << " features (not including the class attribute), with " << data.size() << " instances." << endl;

    vector<vector<vector<double>>> cache = cacheInit(data);

    set<int> featureSet;

    for (int i = 1; i < data[0].size(); i++) {
        featureSet.insert(i);
    }

    cout << "Running nearest neighbor with all 4 features, using 'leave-one-out' cross evaluation, I get an accuracy of: " << accuracy(data, cache, -1, featureSet, 0.0) * 100 << "%" << endl;
    cout << "Beginning search:" << endl;

    //all timing logic came from this website:
    //https://www.geeksforgeeks.org/cpp/measure-execution-time-function-cpp/
    auto start = high_resolution_clock::now();
    if (method == 1) {
        set<int> bestSet = forwardSearch(data, cache);
    }
    else if (method == 2) {
        backwardElimination(data, cache);
    }
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(stop - start);
    cout << duration.count() << endl;


}
