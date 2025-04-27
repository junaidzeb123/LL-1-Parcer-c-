#include "parsingtable.cpp"
#include "lexer.cpp"
#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

string mapTokenToTerminal(const Token &t) {
    switch (t.type) {
    case TokenType::IDENTIFIER:
        return "id";
    case TokenType::INTEGER_LITERAL:
    case TokenType::FLOAT_LITERAL:
        return "num";
    case TokenType::KEYWORD: 
        return t.value;
    case TokenType::OPERATOR:  
    case TokenType::PUNCTUATOR: 
        return t.value;
    default: 
        return t.value;
    }
}

string nonTerminalMeaning(const string &nt) {
    if (nt == "<program>")
        return "the entire program";
    if (nt == "<global_decl_list>")
        return "a list of global declarations";
    if (nt == "<global_decl>")
        return "a global declaration (variable or function)";
    if (nt == "<decl_stmt>")
        return "a variable declaration statement";
    if (nt == "<init_decl_list>")
        return "a list of initialized declarations";
    if (nt == "<init_decl_tail>")
        return "additional initialized declarations";
    if (nt == "<init_decl>")
        return "a variable initialization";
    if (nt == "<func_def>")
        return "a function definition";
    if (nt == "<param_list>")
        return "a list of function parameters";
    if (nt == "<param_list_tail>")
        return "additional parameters in a function";
    if (nt == "<param>")
        return "a single function parameter";
    if (nt == "<block>")
        return "a block of statements (curly braces {...})";
    if (nt == "<stmt_list>")
        return "a list of statements";
    if (nt == "<stmt>")
        return "a statement";
    if (nt == "<matched_stmt>")
        return "a fully matched if-else, loop, or statement";
    if (nt == "<unmatched_stmt>")
        return "an incomplete if-else statement";
    if (nt == "<assign_stmt>")
        return "an assignment statement";
    if (nt == "<return_stmt>")
        return "a return statement";
    if (nt == "<expr_or_empty>")
        return "an expression or empty return";
    if (nt == "<break_stmt>")
        return "a break statement";
    if (nt == "<continue_stmt>")
        return "a continue statement";
    if (nt == "<expr_stmt>")
        return "an expression statement";
    if (nt == "<for_init>")
        return "the initialization part of a for-loop";
    if (nt == "<for_iter>")
        return "the iteration part of a for-loop";
    if (nt == "<expr>")
        return "an expression";
    if (nt == "<logical_or_expr>")
        return "a logical OR expression";
    if (nt == "<logical_or_expr_tail>")
        return "additional logical OR operations";
    if (nt == "<logical_and_expr>")
        return "a logical AND expression";
    if (nt == "<logical_and_expr_tail>")
        return "additional logical AND operations";
    if (nt == "<equality_expr>")
        return "an equality expression (==, !=)";
    if (nt == "<equality_expr_tail>")
        return "additional equality checks";
    if (nt == "<relational_expr>")
        return "a relational expression (<, >, <=, >=)";
    if (nt == "<relational_expr_tail>")
        return "additional relational comparisons";
    if (nt == "<additive_expr>")
        return "an additive expression (+, -)";
    if (nt == "<additive_expr_tail>")
        return "additional additive operations";
    if (nt == "<term>")
        return "a term in an arithmetic expression";
    if (nt == "<term_tail>")
        return "additional multiplicative operations (*, /, %)";
    if (nt == "<factor>")
        return "a factor (operand or parenthesized expression)";
    if (nt == "<function_call>")
        return "a function call";
    if (nt == "<arg_list>")
        return "a list of function call arguments";
    if (nt == "<arg_list_tail>")
        return "additional arguments in a function call";
    if (nt == "<type>")
        return "a data type (int, float, double, etc.)";

    return "some structure"; 
}



bool parseSequence(
    const vector<string> &tokens,
    const unordered_map<string, unordered_map<string, vector<string>>> &table,
    const string &startSymbol) {
    stack<string> st;
    st.push("$");
    st.push(startSymbol);

    int idx = 0;
    while (!st.empty()) {
        string top = st.top();
        string curr = tokens[idx];

        // Accept
        if (top == "$" && curr == "$") {
            cout << "Parsing successful!\n";
            return true;
        }

        // If top is a terminal
        if (table.find(top) == table.end()) {
            if (top == curr) {
                st.pop();
                idx++;
            } else {
                cerr << "Syntax error: expected '" << top
                     << "' but got '" << curr << "'\n";
                return false;
            }
        }
        // top is a non-terminal
        else {
            auto row = table.at(top);
            if (row.count(curr)) {
                vector<string> prod = row.at(curr);
               

                cout  << "Top of Stack = " <<  top << "\t\tInput buffer " << curr << endl;
                cout  << "Applying " << top << "-> ";
                for (auto &x : prod)
                    cout << x;

                cout << endl<< endl;

                st.pop();
                // push RHS in reverse, skipping epsilon
                if (!(prod.size() == 1 && prod[0] == "ε")) {
                    for (auto it = prod.rbegin(); it != prod.rend(); ++it)
                        st.push(*it);
                }
            } else {
                cout  << " terminal -> " << curr << " top -> " << top ;
                cout << endl;
                cout << "error: unexpected token '" << curr << "' while parsing " << nonTerminalMeaning(top) << "\n";
                return false;
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

    // 2) Lex
    LexicalAnalyzer lexer(sourceCode);
    vector<Token> lexed = lexer.tokenize();

    vector<string> tokens;
    for (auto &tk : lexed)
        tokens.push_back(mapTokenToTerminal(tk));
    tokens.push_back("$");

    // 3) Open log and redirect cout
    ofstream logFile("parsinglogs.txt");
    if (!logFile.is_open()) {
        cerr << "Error opening parsinglogs.txt\n";
        return 1;
    }
    auto oldCoutBuf = cout.rdbuf();            // save console buffer
    cout.rdbuf(logFile.rdbuf());               // redirect cout → logFile

    // 4) Print tokens and run the parser (all cout here goes to the file)
    cout << "Token sequence: ";
    for (auto &s : tokens) cout << "'" << s << "' ";
    cout << "\n\n";

    bool ok = parseSequence(tokens, parsingTable, "S");

    // 5) Restore cout back to console
    cout.rdbuf(oldCoutBuf);

    // 6) Print final verdict on the terminal
    cout << (ok ? "ACCEPTED\n" : "REJECTED\n");
    return 0;
}
