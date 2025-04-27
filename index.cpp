#include "lexer.cpp"
#include "parsingtable.cpp"
#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

void mapTokenToTerminal(Token &t) {
    switch (t.type) {

    case TokenType::IDENTIFIER:
        t.value = "id";
        break;
    case TokenType::INTEGER_LITERAL:
    case TokenType::FLOAT_LITERAL:
        t.value = "num";
        break;
    }
}

bool parseSequence(
    const vector<Token> &tokens,
    const unordered_map<string, unordered_map<string, vector<string>>> &table,
    const string &startSymbol) {

    stack<string> st;
    st.push("$");
    st.push(startSymbol);

    int errorCount = 0;
    int idx = 0;
    string previous = "";

    while (!st.empty()) {
        string top = st.top();
        string curr = tokens[idx].value;

        // Accept
        if (top == "$" && curr == "$") {
            cout << "Parsing successful!\n";
          
            if (errorCount){
                cerr << "Parsing complet with " << errorCount << " Errors\n";
                return false;
            }
            return true;
        }

        // If top is a terminal
        if (table.find(top) == table.end()) {
            if (top == curr) {
                st.pop();
                previous = curr;
                idx++;
            } else {
                if(previous == "error"){
                    st.pop();
                    continue;;
                } 
                cerr << "Syntax error: at line " << tokens[idx].lineNO << " expected '" << top
                     << "' but got '" << curr << "'\n";
                errorCount++;

                st.pop();
                // Skip until delimiter
                while (idx < tokens.size() && tokens[idx].value != ";" && tokens[idx].value != "{" && tokens[idx].value != "}") {
                    idx++;
                }
                if (idx == tokens.size()) {
                    cerr << "Parsing failed: reached end of input during recovery.\n";
                    return false;
                }
                // Check if current token is delimiter and adjust idx
                if (tokens[idx].value == "}" || tokens[idx].value == ";") {
                    idx++;
                    if (idx >= tokens.size()) {
                        idx = tokens.size() - 1;
                    }
                }
                previous = "error"; // Reset previous token
            }
        }
        // top is a non-terminal
        else {
            auto row = table.at(top);
            if (row.count(curr)) {
                vector<string> prod = row.at(curr);

                cout << "Top of Stack = " << top << "\t\tInput_buffer " << curr << endl;
                cout << "Applying " << top << "-> ";
                for (auto &x : prod)
                    cout << x;

                cout << endl
                     << endl;

                st.pop();
                if (!(prod.size() == 1 && prod[0] == "ε")) {
                    for (auto it = prod.rbegin(); it != prod.rend(); ++it)
                        st.push(*it);
                }

            } else {
                if(previous == "error"){
                    st.pop();
                    continue;;
                } 
                cerr << "Syntax error at line " << tokens[idx].lineNO << ": unexpected token '" << curr << "' after '" << previous << "'\n";
                errorCount++;

                st.pop();
                // Skip until delimiter
                while (idx < tokens.size() && tokens[idx].value != ";" && tokens[idx].value != "{" && tokens[idx].value != "}") {
                    idx++;
                }
                if (idx == tokens.size()) {
                    cerr << "Parsing failed: reached end of input during recovery.\n";
                    return false;
                }
                // Check if current token is delimiter and adjust idx
                if (tokens[idx].value == "}" || tokens[idx].value == ";") {
                    idx++;
                    if (idx >= tokens.size()) {
                        idx = tokens.size() - 1;
                    }
                }
                previous = "error"; // Reset previous token
            }
        }
    }

    cerr << "Parsing failed: stack emptied early.\n";
    return false;
}

int main() {
    auto parsingTable = getParcingTable();

    // 1) Read source
    ifstream file("code.txt");
    if (!file.is_open()) {
        cerr << "Error opening file!\n";
        return 1;
    }
    stringstream buffer;
    buffer << file.rdbuf();
    string sourceCode = buffer.str();

    LexicalAnalyzer lexer(sourceCode);
    vector<Token> lexed = lexer.tokenize();

    for (auto &tk : lexed)
        mapTokenToTerminal(tk);
    lexed.push_back(Token(TokenType::UNKNOWN, "$", -1));

    ofstream logFile("parsinglogs.txt");
    if (!logFile.is_open()) {
        cerr << "Error opening parsinglogs.txt\n";
        return 1;
    }
    auto oldCoutBuf = cout.rdbuf();
    cout.rdbuf(logFile.rdbuf());

    cout << "Token sequence: ";
    for (auto &s : lexed)
        cout << "'" << s.value << "' ";
    cout << "\n\n";

    bool ok = parseSequence(lexed, parsingTable, "S");
    cout.rdbuf(oldCoutBuf);
    cout << (ok ? "ACCEPTED\n" : "REJECTED\n");
    return 0;
}
