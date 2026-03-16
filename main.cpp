#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <string>
#include <cctype>
#include <ctime>

using namespace std;

// ==============================
// Encapsulation + Abstraction
// ==============================
class FileReader {
private:
    string path;   // encapsulated variable

public:
    FileReader(const string &p) : path(p) {}

    string read() {
        ifstream file(path);
        if (!file) return "";

        stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }
};

// ===================================================
// Abstraction: Base Class (pure virtual functions)
// ===================================================
class Analyzer {
public:
    virtual double getMatch() = 0;
    virtual vector<string> getMissing() = 0;
    virtual ~Analyzer() {}
};

// =============================================
// Inheritance + Polymorphism
// =============================================
class ResumeAnalyzer : public Analyzer {
private:
    string resume;
    string job;

    string toLower(string s) {
        transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
    }

    vector<string> splitWords(const string &t) {
        vector<string> words;
        string temp;

        for (char c : t) {
            if (isalnum(c))
                temp += tolower(c);
            else if (!temp.empty()) {
                words.push_back(temp);
                temp.clear();
            }
        }

        if (!temp.empty())
            words.push_back(temp);

        return words;
    }

public:
    ResumeAnalyzer(const string &r, const string &j) {
        resume = toLower(r);
        job = toLower(j);
    }

    double getMatch() override {
        vector<string> jobWords = splitWords(job);

        int match = 0;

        for (auto &w : jobWords)
            if (w.size() > 2 && resume.find(w) != string::npos)
                match++;

        return (match * 100.0 / jobWords.size());
    }

    vector<string> getMissing() override {
        vector<string> jobWords = splitWords(job);
        vector<string> missing;

        for (auto &w : jobWords)
            if (w.size() > 2 && resume.find(w) == string::npos)
                missing.push_back(w);

        return missing;
    }
};

// ==========================
// Random Suggestions Engine
// ==========================
string generateSuggestion(double percent, vector<string> missing) {

    srand(time(NULL));

    vector<string> phrases = {
        "Try improving skills in",
        "Focus more on",
        "Boost your resume by adding",
        "Enhance your abilities in",
        "You can strengthen"
    };

    if (missing.empty())
        return "Excellent! Your resume covers all important keywords.";

    string output = phrases[rand() % phrases.size()] + ": ";

    for (int i = 0; i < missing.size(); i++) {
        output += missing[i];
        if (i != missing.size() - 1)
            output += ", ";
    }

    output += ".";
    return output;
}

// ========================
// MAIN
// ========================
int main() {

    string resumePath;
    string jobPath;

    cout << "Enter Resume File Path: ";
    getline(cin, resumePath);

    cout << "Enter Job Description File Path: ";
    getline(cin, jobPath);

    FileReader r(resumePath);
    FileReader j(jobPath);

    string resume = r.read();
    string job = j.read();

    if (resume.empty() || job.empty()) {
        cout << "\nError: Could not read files.\n";
        return 1;
    }

    Analyzer *analyzer = new ResumeAnalyzer(resume, job);

    double percent = analyzer->getMatch();
    vector<string> missing = analyzer->getMissing();

    cout << "\n===========================\n";
    cout << " Resume Match: " << percent << "%\n";
    cout << "===========================\n";

    cout << generateSuggestion(percent, missing) << "\n";

    cout << "\nMissing Keywords:\n";
    for (auto &w : missing)
        cout << "• " << w << endl;

    delete analyzer;

    return 0;
}