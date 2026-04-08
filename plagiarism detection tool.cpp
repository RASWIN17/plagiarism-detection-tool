#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <cmath>
#include <algorithm>

using namespace std;

// Stopwords
set<string> STOPWORDS = {
    "the", "is", "in", "and", "to", "of", "a", "an", "on", "for", "with",
    "that", "this", "it", "as", "are", "was", "were"
};

int N_GRAM = 2;

// Convert text to lowercase and remove punctuation
string clean_text(string text) {
    for (char &c : text) {
        if (ispunct(c)) c = ' ';
        else c = tolower(c);
    }
    return text;
}

// Preprocess text
vector<string> preprocess(string text) {
    text = clean_text(text);
    stringstream ss(text);
    vector<string> words;
    string word;

    while (ss >> word) {
        if (STOPWORDS.find(word) == STOPWORDS.end()) {
            words.push_back(word);
        }
    }
    return words;
}

// Generate n-grams
vector<string> generate_ngrams(vector<string> words, int n = N_GRAM) {
    vector<string> ngrams;
    for (int i = 0; i <= (int)words.size() - n; i++) {
        string gram = words[i];
        for (int j = 1; j < n; j++) {
            gram += " " + words[i + j];
        }
        ngrams.push_back(gram);
    }
    return ngrams;
}

// Jaccard similarity
double jaccard_similarity(string t1, string t2) {
    vector<string> w1 = preprocess(t1);
    vector<string> w2 = preprocess(t2);

    vector<string> n1 = generate_ngrams(w1);
    vector<string> n2 = generate_ngrams(w2);

    set<string> s1(n1.begin(), n1.end());
    set<string> s2(n2.begin(), n2.end());

    int intersection = 0;
    for (auto &x : s1) {
        if (s2.count(x)) intersection++;
    }

    int uni = s1.size() + s2.size() - intersection;
    if (uni == 0) return 0;

    return (double)intersection / uni * 100;
}

// Cosine similarity
double cosine_similarity(string t1, string t2) {
    vector<string> w1 = preprocess(t1);
    vector<string> w2 = preprocess(t2);

    vector<string> n1 = generate_ngrams(w1);
    vector<string> n2 = generate_ngrams(w2);

    map<string, int> f1, f2;

    for (auto &x : n1) f1[x]++;
    for (auto &x : n2) f2[x]++;

    set<string> all_keys;
    for (auto &p : f1) all_keys.insert(p.first);
    for (auto &p : f2) all_keys.insert(p.first);

    double dot = 0, mag1 = 0, mag2 = 0;

    for (auto &key : all_keys) {
        int v1 = f1[key];
        int v2 = f2[key];
        dot += v1 * v2;
        mag1 += v1 * v1;
        mag2 += v2 * v2;
    }

    if (mag1 == 0 || mag2 == 0) return 0;

    return (dot / (sqrt(mag1) * sqrt(mag2))) * 100;
}

// Read file
string read_file(string path) {
    ifstream file(path);
    if (!file.is_open()) {
        cout << "Error reading " << path << endl;
        return "";
    }

    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Compare files
void compare_files(vector<string> file_paths) {
    vector<string> texts;

    for (auto &path : file_paths) {
        texts.push_back(read_file(path));
    }

    cout << "\n=== Similarity Results ===\n\n";

    for (int i = 0; i < file_paths.size(); i++) {
        for (int j = i + 1; j < file_paths.size(); j++) {

            double jac = jaccard_similarity(texts[i], texts[j]);
            double cos = cosine_similarity(texts[i], texts[j]);

            cout << file_paths[i] << "  <-->  " << file_paths[j] << endl;
            cout << "  Jaccard: " << jac << "%" << endl;
            cout << "  Cosine : " << cos << "%" << endl;

            if (cos > 80)
                cout << "  Result : HIGH plagiarism ⚠️\n";
            else if (cos > 50)
                cout << "  Result : MODERATE similarity ⚠️\n";
            else
                cout << "  Result : LOW similarity ✅\n";

            cout << "----------------------------------------\n";
        }
    }
}

// Main function
int main() {
    cout << "=== Advanced Plagiarism Detection Tool ===\n";

    int n;
    cout << "Enter number of files to compare: ";
    cin >> n;
    cin.ignore();

    vector<string> file_paths;
    for (int i = 0; i < n; i++) {
        string path;
        cout << "Enter path for file " << i + 1 << ": ";
        getline(cin, path);
        file_paths.push_back(path);
    }

    compare_files(file_paths);

    return 0;
}
