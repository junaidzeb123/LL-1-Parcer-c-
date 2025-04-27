#define EPSILON "ε"

#include <bits/stdc++.h>
#include <fstream>
using namespace std;
unordered_map<string, vector<string>> cfgs;
std::vector<std::string> order;
int terminalAdded = 0;

set<string> terminals = {
    "main",
    "id",
    "num",
    "int",
    "float",
    "double",
    "char",
    "bool",
    "void",
    "if",
    "else",
    "while",
    "for",
    "return",
    "break",
    "continue", "=", "+", "-", "*", "/","%",
    "<", ">", "@", "!=", "<=", ">=",
    "&", "|", 
    "(", ")",
    "{", "}", ";", ",", "orOpt", EPSILON};

void followHelper2(string x, unordered_map<string, set<string>> &followSet, unordered_map<string, set<string>> firstsets);

string longestCommonPrefix(vector<string> &arr) {

    sort(arr.begin(), arr.end());

    string first = arr.front();
    string last = arr.back();
    int minLength = min(first.size(), last.size());

    int i = 0;

    while (i < minLength && first[i] == last[i]) {
        i++;
    }

    string common = first.substr(0, i);
    return find(order.begin(), order.end(), common) != order.end() ? common : "";
}

vector<string> getTokensFromProd(string prod) {
    vector<string> tokens;
    int i = 0;
    while (i < prod.length()) {
        bool matched = false;
        // FOR ALL THE NON TERMINALS
        for (const auto &symbol : order) { // order = all non-terminals and terminals
            if (prod.substr(i, symbol.length()) == symbol) {
                tokens.push_back(symbol);
                i += symbol.length();
                matched = true;
                break;
            }
        }
        // FOR ALL THE TERMINALS
        if (!matched) {

            for (const auto &symbol : terminals) { // order = all non-terminals and terminals

                if (prod.substr(i, symbol.length()) == symbol) {
                    tokens.push_back(symbol);
                    i += symbol.length();
                    matched = true;
                    break;
                }
            }
        }
        if (!matched) {
            ++i;
        }
    }
    return tokens;
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
            if (newStr.empty()) {
                newProudction.push_back(EPSILON); // explicitly add epsilon
            } else if (newStr != EPSILON) {
                newProudction.push_back(newStr);
            }
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
        // cout << "\nStarting left Factoring algorithm again\n";
        bool change = false;

        // RUNNING FOR THE PRODUCTION FOR EACH NON TERMINAL
        for (auto &x : cfgs) {
            string nt = x.first;
            // cout << "Runnig for " << nt << endl;
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
                        // cout << "Prefix for " << nt << " " << prefix << endl;
                        if (prefix != "") {
                            change = true;
                            addNewRule(prefix, nt, currentProduction);
                            isBrokenDown = true;
                            break;
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
                // cout << str << endl;
                cfgs[nt].push_back(str);
                str = "";
                continue;
            } else {
                str += prod;
                if(str.length() == 5 && str == "orOpt"){ // converting orOpt in actual form
                    str = "|";
                }
            }
        }
        cfgs[nt].push_back(str);
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
                // if (c == 0x01)
                //     cout << "ε";
                // else
                cout << c;
            }
            cout << endl;
        }
    }
}

void solveNonImmediateLR(string nameA, string nameB) {
    // string nameA = A.getName();
    // string nameB = B.getName();
    vector<string> rulesA, rulesB, newRulesA;
    rulesA = cfgs[nameA];
    rulesB = cfgs[nameB];

    for (auto rule : rulesA) {
        if (rule.substr(0, nameB.size()) == nameB) {
            for (auto rule1 : rulesB) {
                newRulesA.push_back(rule1 + rule.substr(nameB.size()));
            }
        } else {
            newRulesA.push_back(rule);
        }
    }
    cfgs[nameA] = newRulesA;
}

// Algorithm for eliminating Immediate Left Recursion
pair<string, vector<string>> solveImmediateLR(string name) {
    string newName = "T" + to_string(terminalAdded);

    vector<string> alphas, betas, rules, newRulesA, newRulesA1;
    rules = cfgs[name];

    // Checks if there is left recursion or not
    for (auto rule : rules) {
        if (rule.substr(0, name.size()) == name) {
            alphas.push_back(rule.substr(name.size()));
        } else {
            betas.push_back(rule);
        }
    }

    // If no left recursion, exit
    if (!alphas.size())
        return {};

    if (!betas.size())
        newRulesA.push_back(newName);

    for (auto beta : betas)
        newRulesA.push_back(beta + newName);

    for (auto alpha : alphas)
        newRulesA1.push_back(alpha + newName);

    cfgs[name] = newRulesA;
    newRulesA1.push_back(EPSILON);

    terminalAdded++;
    return {newName, newRulesA1};
}

// Eliminates left recursion
void applyAlgorithm() {
    int size = cfgs.size();

    int i = 0;

    vector<pair<string, vector<string>>> vec;

    for (auto &x : cfgs) {
        // int j = 0;
        // for (auto &y : cfgs) {
        //     solveNonImmediateLR(x.first, y.first);
        //     j++;
        //     if (j >= i)
        //         break;
        // }
        pair<string, vector<string>> result = solveImmediateLR(x.first);
        i++;
        if (result.first.empty() && result.second.empty()) {
            continue;
        }
        vec.push_back(result);
    }

    for (auto &x : vec) {
        order.push_back(x.first);
        cfgs.insert(x);
    }
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
                    if (y == EPSILON) {
                        isEpisilon = true;
                    } else
                        st.insert(y);
                }
                prod = prod.substr(str.length());
                if (prod.length() == 0 && isEpisilon) {
                    st.insert(EPSILON);
                }
                str = "";
                break;
            }
        }
    }
    if (!isnonTerminal) {
        string matched_terminal = "";
        for (const auto &t : terminals) {
            if (prod.substr(0, t.size()) == t) {
                if (t.size() > matched_terminal.size()) {
                    matched_terminal = t;
                }
            }
        }
        if (!matched_terminal.empty()) {
            st.insert(matched_terminal);
            prod = prod.substr(matched_terminal.size());
        } else {
            // fallback if no terminal matches
            // string s(1, prod[0]);
            // st.insert(s);
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
            vector<string> tokens = getTokensFromProd(prod);
            for (auto ch : tokens) {
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
                        if (y == EPSILON) {
                            isEpisilon = true;
                        } else
                            st.insert(y);
                    }
                    prod = prod.substr(str.length());
                    if (prod.length() == 0 && isEpisilon) {
                        st.insert(EPSILON);
                    }
                    str = "";
                    break;
                }
            }
        }
        if (!isnonTerminal) {
            string matched_terminal = "";
            for (const auto &t : terminals) {
                if (prod.substr(0, t.size()) == t) {
                    if (t.size() > matched_terminal.size()) {
                        matched_terminal = t;
                    }
                }
            }
            if (!matched_terminal.empty()) {
                st.insert(matched_terminal);
                prod = prod.substr(matched_terminal.size());
            } else {
                // // fallback if no terminal matches (should not happen ideally)
                // string s(1, prod[0]);
                // st.insert(s);
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
                    if (y == EPSILON) {
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
        string matched_terminal = "";
        for (const auto &t : terminals) {
            if (prod.substr(0, t.size()) == t) {
                if (t.size() > matched_terminal.size()) {
                    matched_terminal = t;
                }
            }
        }
        if (!matched_terminal.empty()) {
            st.insert(matched_terminal);
            prod = prod.substr(matched_terminal.size());
        } else {
            // fallback if no terminal matches
            // string s(1, prod[0]);
            // st.insert(s);
        }
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
                if (terminal != EPSILON) {
                    table[nt][terminal] = production;
                }
            }
            if (firstAlpha.find(EPSILON) != firstAlpha.end()) {
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
    int width = 40;
    cout << "\nLL(1) Parsing Table:\n";
    cout << std::left;
    cout << setw(width) << "";

    for (auto term : terminals) {
        cout << setw(width) << std::left << term;
    }
    cout << "\n"
         << string(width * (terminals.size() + 1), '-') << "\n";

    // Print each non-terminal row.
    for (auto &nt : order) {
        cout << setw(width) << std::left << nt;
        for (auto term : terminals) {
            if (table.count(nt) && table.at(nt).count(term)) {
                cout << setw(width) << std::left << table.at(nt).at(term);
            } else {
                cout << setw(width) << std::left << "";
            }
        }
        cout << endl;
    }
}

void displayParsingTable(const unordered_map<string, unordered_map<string, vector<string>>> &table) {
    set<string> terminals;

    // Collect all terminals
    for (auto &entry : table) {
        for (auto &cell : entry.second) {
            terminals.insert(cell.first);
        }
    }
    terminals.insert("$"); // add end marker

    int width = 40;
    cout << "\nLL(1) Parsing Table:\n";
    cout << std::left;
    cout << setw(width) << "";

    // Print all terminal headers
    for (auto term : terminals) {
        cout << setw(width) << std::left << term;
    }
    cout << "\n"
         << string(width * (terminals.size() + 1), '-') << "\n";

    // Print each non-terminal row
    for (auto &nt : order) {
        cout << setw(width) << std::left << nt;
        for (auto term : terminals) {
            if (table.count(nt) && table.at(nt).count(term)) {
                string production;
                for (const auto &sym : table.at(nt).at(term)) {
                    production += sym + " ";
                }
                if (!production.empty() && production.back() == ' ') {
                    production.pop_back(); // remove trailing space
                }
                cout << setw(width) << std::left << production;
            } else {
                cout << setw(width) << std::left << "";
            }
        }
        cout << endl;
    }
}

unordered_map<string, unordered_map<string, vector<string>>> changeTableShape(const unordered_map<string, unordered_map<string, string>> tabletemp) {

    unordered_map<string, unordered_map<string, vector<string>>> table;

    for (auto &x : tabletemp) {
        string nonterminal = x.first;
        for (auto &y : x.second) {
            string terminal = y.first;
            string prod = y.second;
            vector<string> tokens = getTokensFromProd(prod);
            // if (prod == "<type>id(<param_list>)<block>") {
            //     cout << "\n\n\n<type>id(<param_list>)<block>  -->  ";
            //     for (auto &t : tokens) {
            //         cout << t << " ";
            //     }
            // }
            table[nonterminal][terminal] = tokens;
        }
    }
    return table;
}

unordered_map<string, unordered_map<string, vector<string>>> getParcingTable() {

    readCFG("cfg.txt");

    // freopen("output.txt", "w", stdout);

    leftFactoring();
    applyAlgorithm(); 

    unordered_map<string, set<string>> firstsets = calculateFirst();

    unordered_map<string, set<string>> follow = calculateFollow(firstsets);
    unordered_map<string, unordered_map<string, string>> tabletemp = createParsingTable(cfgs, firstsets, follow);

    return changeTableShape(tabletemp);
}

// int main() {

//     readCFG("cfg.txt");

//     freopen("output.txt", "w", stdout);

//     cout << "Original CFG:\n";
//     displayCFG();

//     cout << endl
//          << endl
//          << endl;

//     leftFactoring();
//     cout << "\nLeft Factored CFG:\n";
//     displayCFG();

//     cout << endl
//          << endl
//          << endl;

//     applyAlgorithm();
//     cout << "\nLEFT RECURSION REMOVED CFG:\n";
//     displayCFG();

//     unordered_map<string, set<string>> firstsets = calculateFirst();
//     cout << "\n\n FIRST SETS\n";
//     displayFirstFollow(firstsets);
//     cout << endl
//          << endl
//          << endl;

//     unordered_map<string, set<string>> follow = calculateFollow(firstsets);
//     cout << "\n\n FOLLOW SETS\n";
//     displayFirstFollow(follow);

//     unordered_map<string, unordered_map<string, string>> table = createParsingTable(cfgs, firstsets, follow);
//     displayParsingTable(table);

//     cout << "TABLE # 2 \n";
//     displayParsingTable(changeTableShape(table));

//     for (auto &x : terminals) {
//         cout << x << " ";
//     }
// }
