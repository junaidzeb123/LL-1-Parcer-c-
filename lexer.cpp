// C++ Program to implement a lexical analyzer

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

// Enum class to define different types of tokens
enum class TokenType {
    KEYWORD,
    IDENTIFIER,
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    OPERATOR,
    PUNCTUATOR,
    UNKNOWN
};

// Struct to represent a token with its type and value
struct Token {
    TokenType type;
    string value;
    int lineNO;
    Token(TokenType t, const string &v,const int lineNO)
        : type(t), value(v),lineNO(lineNO) {
    }
};

// Class that implements the lexical analyzer
class LexicalAnalyzer {
private:
    string input;
    size_t position;
    unordered_map<string, TokenType> keywords;
    int line;
    // Function to initialize the keywords map
    void initKeywords() {
        keywords["int"] = TokenType::KEYWORD;
        keywords["float"] = TokenType::KEYWORD;
        keywords["double"] = TokenType::KEYWORD;
        keywords["char"] = TokenType::KEYWORD;
        keywords["if"] = TokenType::KEYWORD;
        keywords["else"] = TokenType::KEYWORD;
        keywords["while"] = TokenType::KEYWORD;
        keywords["return"] = TokenType::KEYWORD;
        keywords["for"] = TokenType::KEYWORD;
        keywords["main"] = TokenType::KEYWORD;
    }

    // Function to check if a character is whitespace
    bool isWhitespace(char c) {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }

    // Function to check if a character is alphabetic
    bool isAlpha(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    // Function to check if a character is a digit
    bool isDigit(char c) { return c >= '0' && c <= '9'; }

    // Function to check if a character is alphanumeric
    bool isAlphaNumeric(char c) {
        return isAlpha(c) || isDigit(c);
    }

    // Function to get the next word (identifier or keyword)
    // from the input
    string getNextWord() {
        size_t start = position;
        while (position < input.length() && isAlphaNumeric(input[position])) {
            position++;
        }
        return input.substr(start, position - start);
    }

    // Function to get the next number (integer or float)
    // from the input
    string getNextNumber() {
        size_t start = position;
        bool hasDecimal = false;
        while (position < input.length() && (isDigit(input[position]) || input[position] == '.')) {
            if (input[position] == '.') {
                if (hasDecimal)
                    break;
                hasDecimal = true;
            }
            position++;
        }
        return input.substr(start, position - start);
    }

public:
    // Constructor for LexicalAnalyzer
    LexicalAnalyzer(const string &source)
        : input(source), position(0) {
        initKeywords();
        line = 1;
    }

    // Function to tokenize the input string
    vector<Token> tokenize() {
        vector<Token> tokens;

        while (position < input.length()) {
            char c = input[position];

            if (c == '\n') {
                line++;
                position++;
                continue;
            }

            // Skip whitespace
            if (isWhitespace(c)) {
                position++;
                continue;
            }

            // Identifiers & keywords
            if (isAlpha(c)) {
                string word = getNextWord();
                if (keywords.count(word))
                    tokens.emplace_back(TokenType::KEYWORD, word, line);
                else
                    tokens.emplace_back(TokenType::IDENTIFIER, word,line);
                continue;
            }

            // Numbers
            if (isDigit(c)) {
                string num = getNextNumber();
                TokenType type = (num.find('.') != string::npos)
                                     ? TokenType::FLOAT_LITERAL
                                     : TokenType::INTEGER_LITERAL;
                tokens.emplace_back(type, num,line);
                continue;
            }

            // Two-char operators: ==, !=, <=, >=, &&, ||
            if ((c == '=' || c == '!' || c == '<' || c == '>' ||
                 c == '&' || c == '|') &&
                position + 1 < input.length()) {
                char d = input[position + 1];
                string two = string() + c + d;
                if (two == "==" || two == "!=" ||
                    two == "<=" || two == ">=" ||
                    two == "&&" || two == "||") {
                    tokens.emplace_back(TokenType::OPERATOR, two,line);
                    position += 2;
                    continue;
                }
            }

            // Single-char operators
            if (c == '=' || c == '!' || c == '<' || c == '>' ||
                c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '@') {
                tokens.emplace_back(TokenType::OPERATOR, string(1, c),line);
                position++;
                continue;
            }

            // Punctuators
            if (c == '(' || c == ')' || c == '{' || c == '}' ||
                c == ';' || c == ',') {
                tokens.emplace_back(TokenType::PUNCTUATOR, string(1, c),line);
                position++;
                continue;
            }

            // Everything else
            tokens.emplace_back(TokenType::UNKNOWN, string(1, c),line);
            position++;
        }

        return tokens;
    }
};

// Function to convert TokenType to string for printing
string getTokenTypeName(TokenType type) {
    switch (type) {
    case TokenType::KEYWORD:
        return "KEYWORD";
    case TokenType::IDENTIFIER:
        return "IDENTIFIER";
    case TokenType::INTEGER_LITERAL:
        return "INTEGER_LITERAL";
    case TokenType::FLOAT_LITERAL:
        return "FLOAT_LITERAL";
    case TokenType::OPERATOR:
        return "OPERATOR";
    case TokenType::PUNCTUATOR:
        return "PUNCTUATOR";
    case TokenType::UNKNOWN:
        return "UNKNOWN";
    default:
        return "UNDEFINED";
    }
}

// Function to print all tokens
void printTokens(const vector<Token> &tokens) {
    for (const auto &token : tokens) {
        cout << "Type: " << getTokenTypeName(token.type)
             << ", Value: " << token.value << endl;
    }
}

// // Driver Code
// int main() {
//     // Sample source code to be analyzed
//     string sourceCode = "int main() { float x = 3.14; float y=3.15; "
//                         "float z=x+y; return 0; }";

//     // Create a LexicalAnalyzer object
//     LexicalAnalyzer lexer(sourceCode);

//     // Tokenize the source code
//     vector<Token> tokens = lexer.tokenize();

//     // Print the original source code
//     cout << "Source code: " << sourceCode << endl
//          << endl;

//     // Print all identified tokens
//     cout << "Tokens Generate by Lexical Analyzer:" << endl;
//     printTokens(tokens);

//     return 0;
// }
