// #include <algorithm>
// #include <fstream>
// #include <iostream>
// #include <map>
// #include <set>
// #include <sstream>
// #include <string>
// #include <unordered_map>
// #include <vector>
#include <fstream>
#include <bits/stdc++.h>
using namespace std;

unordered_map<string, vector<string>> cfgs;
std::vector<std::string> order;
int terminalAdded = 0;

void followHelper2(
    string x,
    unordered_map<string, set<string>> &followSet,
    unordered_map<string, set<string>> firstsets);

string longestCommonPrefix(vector<string> &arr) {

    sort(arr.begin(), arr.end());

    string first = arr.front();
    string last = arr.back();
    int minLength = min(first.size(), last.size());

    int i = 0;

    while (i < minLength && first[i] == last[i]) {
        i++;
    }

    return first.substr(0, i);
}

void addNewRule(const string &prefix, const string &nt, vector<string> &production) {

    cout << "Breaking down " << nt << " production with prefix " << prefix << "\n";

    string rule = prefix + "T" + to_string(terminalAdded);
    vector<string> newProudction;

    for (int j = 0; j < cfgs[nt].size(); j++) {
        string str = cfgs[nt][j];
        auto it = find(production.begin(), production.end(), str);
        if (it != production.end()) {
            string newStr = str.substr(prefix.length());
            if (newStr != "\"\"" && newStr != "")
                newProudction.push_back(newStr);
            cfgs[nt].erase(cfgs[nt].begin() + j);
            j--;
        }
    }
    cfgs[nt].push_back(rule);
    cfgs["T" + to_string(terminalAdded)] = newProudction;
    order.push_back("T" + to_string(terminalAdded));
    cout << "Added new Non Terminal " << "T" + to_string(terminalAdded) << endl;
    terminalAdded++;
}

void leftFactoring() {

    // RUNNING THE LEFT FACTORING UNTIL THE GRAMMER CHANGED
    while (true) {
        cout << "\nStarting left Factoring algorithm again\n";
        bool change = false;

        // RUNNING FOR THE PRODUCTION FOR EACH NON TERMINAL
        for (auto &x : cfgs) {
            string nt = x.first;
            cout << "Runnig for " << nt << endl;
            sort(cfgs[nt].begin(), cfgs[nt].end());

            vector<string> production = x.second;
            int numberOfProduction = production.size();
            // HAVE TO RUN THE ALGORITHM FOR ALL THE PARIS OF ALL THE LENGTH
            // I.E IF NO COMMON PREFIX IN ALL PRODUCTION CHECK ON THEIR SUBSETS
            bool isBrokenDown = false;
            for (; numberOfProduction > 1; numberOfProduction--) {
                for (int itr = 0; itr + numberOfProduction <= x.second.size(); itr++) { //  CREATING ALL THE PAIRS
                    vector<string> currentProduction(production.begin() + itr, production.begin() + itr + numberOfProduction);
                    if (currentProduction.size() > 1) {
                        string prefix = longestCommonPrefix(currentProduction);
                        cout << "Prefix for " << nt << " " << prefix << endl;
                        if (prefix != "") {
                            change = true;
                            addNewRule(prefix, nt, currentProduction);
                            isBrokenDown = true;
                            break;
                            ;
                        }
                    }
                }
                if (isBrokenDown)
                    break;
            }
        }

        if (!change)
            break;
    }
}

void readCFG(const string &filename) {
    ifstream file(filename);
    string line;

    int itr = 0;
    while (getline(file, line)) {
        istringstream iss(line);
        string nt;
        string arrow, prod;

        if (!(iss >> nt >> arrow))
            continue;

        if (itr == 0) {
            itr++;
            if (nt != "S") {
                cout << nt << endl;
                cout << "Starting symbol must be S\n";
                exit(1);
            }
        }

        string str = "";
        while (iss >> prod) {
            if (prod == "|") {
                cfgs[nt].push_back(str);
                str = "";
                continue;
            } else {
                str += prod;
            }
        }
        cfgs[nt].push_back(prod);
        if (std::find(order.begin(), order.end(), nt) == order.end()) {
            order.push_back(nt);
        }
        sort(cfgs[nt].begin(), cfgs[nt].end());
    }
}

void displayCFG(unordered_map<string, vector<string>> mp = cfgs) {
    for (const auto &nt : order) {
        auto prods = cfgs[nt];
        for (const auto &p : prods) {
            cout << nt << " -> ";
            for (char c : p) {
                if (c == 0x01)
                    cout << "ε";
                else
                    cout << c;
            }
            cout << endl;
        }
    }
}

void leftRecursion() {

    for (auto &x : cfgs) {
        string nt = x.first;
        vector<string> production = x.second;

        vector<string> newProduction;
        string newNonTerminal = "T" + to_string(terminalAdded);
        set<int> nonRecursiveProd;
        int removed = 0;
        for (int i = 0; i < production.size(); i++) {
            string prod = production[i];
            if (prod.length() < nt.length()) {
                nonRecursiveProd.insert(i);
                continue;
            }
            string left = prod.substr(0, nt.length());
            if (left == nt) {
                string right = prod.substr(nt.length());
                cout << "Left Recurrion occuring at " << nt << " -> " << prod << "\n";
                newProduction.push_back(right + newNonTerminal);
                x.second.erase(x.second.begin() + i - removed);
                removed++;

            } else
                nonRecursiveProd.insert(i);
        }
        if (newProduction.size()) {
            order.push_back("T" + to_string(terminalAdded));
            terminalAdded++;
            newProduction.push_back("\"\""); // pushing the episilon
            cfgs[newNonTerminal] = newProduction;
            cout << " size = " << x.second.size() << endl;
            x.second.push_back(newNonTerminal);
            int itr = 0;
            for (auto &y : x.second) {
                if (y != newNonTerminal) {
                    if (itr == 0) {
                        x.second.erase(x.second.end() - 1);
                        itr++;
                    }
                    if (y != "\"\"")
                        y = y + newNonTerminal;
                    else
                        y = newNonTerminal;
                } else {
                    break;
                }
            }
        }
    }
}

void helper(const string &current, const string &starting) {
    if (current == starting) {
        cout << "INDIRECT LEFT REC\n";
    }

    for (auto &prod : cfgs[current]) {
        // helper()
    }
}

void indirectRecursion() {
    // CHECKING WHICH RULES ARE CAUSING LEFT RECURSION
    // for (auto &x : cfgs) {
    //     string nt = x.firt();
    //     string production = x.second();

    //     for (auto &prod : production) {
    //     }
    // }
}

set<string> calculatingFirstOfOneProd(
    string prod,
    unordered_map<string, set<string>> firstsets) {

    set<string> st;
    string str;
    bool isEpisilon = true;
    auto isnonTerminal = false;
    while (isEpisilon) {
        isEpisilon = false;
        isnonTerminal = false;
        for (auto ch : prod) {
            str += ch;
            auto it = find(order.begin(), order.end(), str);
            if (it != order.end()) {
                isnonTerminal = true;

                for (auto &y : firstsets[str]) {
                    // IF THE FIRST OF NON TERMINAL IS EPSILON THEN WE HAVE TO LOOK FOR FIRST OF NEXT NON-TERMINAL
                    if (y == "\"\"") {
                        isEpisilon = true;
                    } else
                        st.insert(y);
                }
                prod = prod.substr(str.length());
                if (prod.length() == 0 && isEpisilon) {
                    st.insert("\"\"");
                }
                str = "";
                break;
            }
        }
    }
    if (!isnonTerminal) {
        if (prod[0] != '\"') {
            string s(1, prod[0]);
            st.insert(s);
        } else {
            st.insert(prod.substr(0, 2));
        }
    }
    return st;
}

void helper(string nt, unordered_map<string, set<string>> &firstsets, set<string> nonTerminal) {

    vector<string> production = cfgs[nt];
    set<string> st;

    // LOOPING ALL THE RULES FOR ONE NON TERMINAL
    for (auto &prod : production) {

        // ENSURING IT IS TERMINAL OR NON TERMINAL
        string str;
        auto isnonTerminal = false;
        bool isEpisilon = true;
        while (isEpisilon) {
            isnonTerminal = false;
            isEpisilon = false;
            for (auto ch : prod) {
                str += ch;
                auto it = nonTerminal.find(str);
                if (it != nonTerminal.end()) {
                    // cout << nt << " -> " << prod << " Non Terminal\n";
                    isnonTerminal = true;
                    // FIRST SET OF THAT NON TERMINAL IS ALREADY THERE
                    if (firstsets.find(str) == firstsets.end()) {
                        helper(str, firstsets, nonTerminal);
                    }

                    for (auto &y : firstsets[str]) {
                        // IF THE FIRST OF NON TERMINAL IS EPSILON THEN WE HAVE TO LOOK FOR FIRST OF NEXT NON-TERMINAL
                        if (y == "\"\"") {
                            isEpisilon = true;
                        } else
                            st.insert(y);
                    }
                    prod = prod.substr(str.length());
                    if (prod.length() == 0 && isEpisilon) {
                        st.insert("\"\"");
                    }
                    str = "";
                    break;
                }
            }
        }
        if (!isnonTerminal) {
            if (prod[0] != '\"') {
                string s(1, prod[0]);
                st.insert(s);
            } else {
                st.insert(prod.substr(0, 2));
            }
        }
    }
    firstsets[nt] = st;
}

unordered_map<string, set<string>> calculateFirst() {

    set<string> nonTerminal;
    for (auto &x : cfgs) {
        nonTerminal.insert(x.first);
    }

    unordered_map<string, set<string>> firstsets;
    for (auto &x : cfgs) {
        helper(x.first, firstsets, nonTerminal);
    }
    return firstsets;
}

void followHelper(
    string prod, string nonTtoCalculate,
    string nonterminal,
    unordered_map<string, set<string>> &followsets,
    unordered_map<string, set<string>> firstsets) {
    // cout <<nonterminal << "-> "<< prod << " " << nonTtoCalculate<<endl;
    auto isnonTerminal = false;
    set<string> st;
    bool isEpisilon = true;

    if (prod == "") {
        if (followsets.find(nonterminal) == followsets.end()) {
            followHelper2(nonterminal, followsets, firstsets);
        }
        followsets[nonTtoCalculate].insert(followsets[nonterminal].begin(), followsets[nonterminal].end());
        return;
    }

    while (isEpisilon) {

        isnonTerminal = false;
        isEpisilon = false;
        string str;
        for (auto ch : prod) {
            str += ch;
            auto it = find(order.begin(), order.end(), str); // checking if it is non-terminal or not

            if (it != order.end()) {
                isnonTerminal = true;
                for (auto &y : firstsets[str]) {
                    // IF THE FIRST OF NON TERMINAL IS EPSILON THEN WE HAVE TO LOOK FOR FIRST OF NEXT NON-TERMINAL
                    if (y == "\"\"") {
                        isEpisilon = true;
                    } else
                        st.insert(y);
                }

                prod = prod.substr(str.length());
                if (prod.length() == 0 && isEpisilon) {
                    if (followsets.find(nonterminal) == followsets.end()) {
                        followHelper2(nonterminal, followsets, firstsets);
                    }
                    st.insert(followsets[nonterminal].begin(), followsets[nonterminal].end());
                }
                str = "";
                break;
            }
        }
    }
    if (!isnonTerminal) {
        string s(1, prod[0]);
        st.insert(s);
        // cout << " non terminal case here " << nonTtoCalculate << " " << s << "\n";
    }
    followsets[nonTtoCalculate].insert(st.begin(), st.end());
}

void followHelper2(
    string x,
    unordered_map<string, set<string>> &followSet,
    unordered_map<string, set<string>> firstsets) {
    // TRAVERSE OVER ALL THE PRODUCTION AND CHECK WHERE THE CURRENT NON TERMINAL IS USED
    // cout << "Calculating follow of " << x << endl;
    for (auto nt : order) {
        vector<string> prod = cfgs[nt];
        for (auto y : prod) {
            int index = y.find(x);
            if (index != string::npos) {
                string temp = y.substr(index + x.length());
                followHelper(temp, x, nt, followSet, firstsets);
                if (x == "B")
                    break;
            }
        }
    }
}

unordered_map<string, set<string>> calculateFollow(unordered_map<string, set<string>> firstsets) {

    unordered_map<string, set<string>> followSet;
    followSet["S"].insert("$");
    for (auto &x : order) {
        if (x != "S" || followSet.find(x) == followSet.end())
            followHelper2(x, followSet, firstsets);
    }
    return followSet;
}

void displayFirstFollow(unordered_map<string, set<string>> firstFollow) {
    for (auto &x : firstFollow) {
        cout << x.first << "-> ";
        for (auto &y : x.second) {
            cout << y << " ";
        }
        cout << endl;
    }
}

unordered_map<string, unordered_map<string, string>> createParsingTable(
    unordered_map<string, vector<string>> &cfgs,
    unordered_map<string, set<string>> &firstsets,
    unordered_map<string, set<string>> &follow) {
    unordered_map<string, unordered_map<string, string>> table;

    for (auto &nt : order) {
        for (auto &production : cfgs[nt]) {
            set<string> firstAlpha = calculatingFirstOfOneProd(production, firstsets);
            for (auto &terminal : firstAlpha) {
                if (terminal != "\"\"") {
                    table[nt][terminal] = production;
                }
            }
            if (firstAlpha.find("\"\"") != firstAlpha.end()) {
                for (auto &terminal : follow[nt]) {
                    table[nt][terminal] = production;
                }
            }
        }
    }
    return table;
}

void displayParsingTable(const unordered_map<string, unordered_map<string, string>> &table) {
    set<string> terminals;
    for (auto &entry : table) {
        for (auto &cell : entry.second) {
            terminals.insert(cell.first);
        }
    }

    terminals.insert("$");

    cout << "\nLL(1) Parsing Table:\n";
    cout << setw(15) << left << "";
    for (auto term : terminals) {
        cout << setw(15) << left << term;
    }
    cout << "\n"
         << string(15 * (terminals.size() + 1), '-') << "\n";

    // Print each non-terminal row.
    for (auto &nt : order) {
        cout << setw(15) << left << nt;
        for (auto term : terminals) {
            if (table.count(nt) && table.at(nt).count(term)) {
                cout << setw(15) << left << table.at(nt).at(term);
            } else {
                cout << setw(15) << left << "";
            }
        }
        cout << endl;
    }
}

int main() {

    readCFG("cfg.txt");

    freopen("output.txt", "w", stdout);

    cout << "Original CFG:\n";
    displayCFG();

    cout << endl
         << endl
         << endl;

    leftFactoring();
    cout << "\nLeft Factored CFG:\n";
    displayCFG();

    cout << endl
         << endl
         << endl;

    leftRecursion();
    cout << "\nLEFT RECURSION REMOVED CFG:\n";
    displayCFG();

    unordered_map<string, set<string>> firstsets = calculateFirst();
    cout << "\n\n FIRST SETS\n";
    displayFirstFollow(firstsets);
    cout << endl
         << endl
         << endl;

    unordered_map<string, set<string>> follow = calculateFollow(firstsets);
    cout << "\n\n FOLLOW SETS\n";
    displayFirstFollow(follow);

    unordered_map<string, unordered_map<string, string>> table = createParsingTable(cfgs, firstsets, follow);
    displayParsingTable(table);
}

/*

A -> aaA
A -> aabc | afg
e -> ad
B -> dd | ddb


S  -> Stmt S
Stmt -> ifStmt RelOp | ifStmt RelOpRelOp | ifStmt RelOp RelOp RelOp
RelOp  -> < | > | == | <= | >= | !=


S -> BD
B -> ""
D -> a | AB | ""
A -> AABd | Ba | a
B -> Be | Bb


*/