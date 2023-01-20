// libraries
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <cstring>

// hpp files
#include "Classes.hpp"

using namespace std;

// global 
int atLine = 0;


// lexer (aka generating tokens)

// geting types sorted especially arrays
//orignal array empty wihout type
ArrayDescriptor::ArrayDescriptor(): TypeDescriptor(ARRAY), _elType(INTEGER), _numElements(0), _values(vector<TypeDescriptor*>()) {}
// used when making an array
ArrayDescriptor::ArrayDescriptor(vector<TypeDescriptor*> values, types elType): TypeDescriptor(ARRAY), _elType(elType), _numElements(values.size()), _values(values) {}

// listing array functions
vector<TypeDescriptor*> ArrayDescriptor::value() const {return _values;}
vector<TypeDescriptor*> &ArrayDescriptor::value() {return _values;}
TypeDescriptor::types &ArrayDescriptor::elType() {return _elType;}
int ArrayDescriptor::numElements() const{return _numElements;}
int &ArrayDescriptor::numElements() {return _numElements;}
// when we want to print array 
void ArrayDescriptor::print() {
    if (numElements() == 0) {
        cout << "[]";
    } else if (elType() == INTEGER) {
        cout << '[' << dynamic_cast<IntegerDescriptor*>(_values[0])->value();
        for (int i = 1; i < numElements(); i++) {
            cout << ", " << dynamic_cast<IntegerDescriptor*>(value()[i])->value();
        }
        cout << ']';
    } else if (elType() == STRING) {
        cout << '[' << dynamic_cast<StringDescriptor*>(_values[0])->value();
        for (int i = 1; i < numElements(); i++) {
            cout << ", " << dynamic_cast<StringDescriptor*>(_values[i])->value();
        }
        cout << ']';
    } else {
        cout << "ArrayDescriptor::print Element type is Array or Unspecified";
        exit(1);
    }
}

//folowint Token class we can simply 
Token::Token(): _name{""}, _eof{false}, _eol{false}, _symbol{'\0'}, _op{""}, _isWholeNumber{false},_isString{false}, _isCmnt{false}, _str{""}, _cmnt{""}, _indent{false}, _dedent{false} {}

// handling the print of Tokens for debugging putrposes
void Token::print() const {
    if( eol() ) cout << "EOL\n" ;
    else if( eof() )  cout << "EOF" ;
    else if( isOpenParen() )  cout << "(" ;
    else if( isCloseParen() )  cout << ")" ;
    else if( isOpenBracket() )  cout << "[" ;
    else if( isCloseBracket() )  cout << "]" ;
    else if( isAssignmentOperator() )  cout << " = " ;
    else if( isMultiplicationOperator() )  cout << " * " ;
    else if( isAdditionOperator() )  cout << " + " ;
    else if( isSubtractionOperator() )  cout << " - " ;
    else if( isGreaterThanOperator() )  cout << " > ";
    else if( isGreaterThanOrEqualToOperator() )  cout << " >= ";
    else if( isLessThanOperator() )  cout << " < ";
    else if( isLessThanOrEqualToOperator() )  cout << " <= ";
    else if( isEqualityOperator() )  cout << " == ";
    else if( isInequalityOperator() )  cout << " != ";
    else if( isOrOperator() )  cout << " or ";
    else if( isAndOperator() )  cout << " and ";
    else if( isNotOperator() )  cout << " not ";
    else if( isComma() ) cout << ", ";
    else if( isColon() ) cout << ": ";
    else if( isKeyword() )  cout << getName() << " ";
    else if( isName() )  cout << getName();
    else if( isWholeNumber() ) cout << getWholeNumber();
    else if( isString() ) cout << '"' << getString() << '"';
    else if( isCmnt() ) cout << getCmnt();
    else if( indent() ) cout << "INDENT ";
    else if( dedent() ) cout << "DEDENT ";
    else cout << "Uninitialized token.\n";
}

// handling naming tokens
string Tokenizer::readName() {
    string name;
    char c;
    while( inStream.get(c) && isalnum(c) ) {
        name += c;
    }
    if(inStream.good())  // In the loop, we have read one char too many.
        inStream.putback(c);
    return name;
}
int Tokenizer::readInteger() {
    int intValue = 0;
    char c;
    while( inStream.get(c) && isdigit(c) && c != '.') {
        intValue = intValue * 10 + c - '0';
    }
    if(inStream.good() || c == '.')  
        inStream.putback(c);
    return intValue;
}
string Tokenizer::readString(char end) {
    // This function is called when it is known that
    // there is a string in input (prev char was " or ') (stored in end)
    // The function reads and returns all characters of the string.

    string s;
    char c;
    while( inStream.get(c) && c != end ) {
        if (c == '\n' || inStream.eof()) {
            cout << endl << "Tokenizer::readString EOL while scanning string literal" << " occured at this line, "<< atLine + 1 << endl;
            exit(1);
        }
        s += c;
    }
    return s;
}

Tokenizer::Tokenizer(ifstream &stream): ungottenToken{false}, inStream{stream}, lastToken{}, parsingANewLine{true} {indentStack.push_back(0);}

Token Tokenizer::getToken() {

    if (ungottenToken) {
        ungottenToken = false;
        return lastToken;
    }

    char c;
    int numSpaces = 0;
    while (inStream.get(c) && isspace(c) && c != '\n')
        numSpaces++;

    if (inStream.bad()) {
        cout << "Error while reading the input stream in Tokenizer.\n";
        exit(1);
    }

    Token token;

    if (inStream.eof() || inStream.fail()) {
        if (indentStack.back() != 0) {
            token.dedent() = true;
            indentStack.pop_back();
            _tokens.push_back(token);
            return lastToken = token;
        }
        token.eof() = true;
        _tokens.push_back(token);
        return lastToken = token;
    }

    if (parsingANewLine) {
        if (c == '\n') {
            token.eol() = true;
            parsingANewLine = true;
            atLine ++;
            _tokens.push_back(token);
            return lastToken = token;
        }
        if (numSpaces > indentStack.back()) {
            inStream.putback(c);
            token.indent() = true;
            indentStack.push_back(numSpaces);
            parsingANewLine = false;
            _tokens.push_back(token);
            return lastToken = token;
        } else if (numSpaces < indentStack.back()) {
            bool valid = false;
            for (int i = indentStack.size(); i >= 0; i--) {
                if (numSpaces == indentStack[i]) {
                    valid = true;
                    break;
                }
            }
            if (!valid) {
                cout << endl << "Tokenizer::getToken invalid indentation level" << " occured at this line, "<< atLine + 1 << endl;
                exit(1);
            }
            inStream.putback(c);
            token.dedent() = true;
            indentStack.pop_back();
            if (indentStack.back() == numSpaces)
                parsingANewLine = false;
            _tokens.push_back(token);
            return lastToken = token;
        }
    }
    parsingANewLine = false;

    if (c == '\n') {  // will not ever be the case unless new-line characters are not suppressed.
        token.eol() = true;
        parsingANewLine = true;
        atLine ++;
    } else if (isdigit(c)) { // an integer
        inStream.putback(c);
        int left = readInteger();
        inStream.get(c);
        if (c == '.') {
            cout << endl << "Tokenizer::getToken invalid value, Theres a double which is not supported" << " occured at this line, "<< atLine + 1 <<  endl;
        } else {
            inStream.putback(c);
            token.setWholeNumber(left);
        }

    } else if (c == '=') {
        inStream.get(c);
        if (c == '=')
            token.op("==");
        else {
            inStream.putback(c);
            token.symbol('=');
        }
    } else if (c == '+' || c == '-' || c == '*' || c == '(' || c == ')' || c == '[' || c == ']' || c == ',' || c == ':' ){
        token.symbol(c);
    } else if (c == '>' || c == '<' || c == '!') {
        char c1 = c;
        inStream.get(c);
        if (c1 == '<' && c == '>'){token.op("<>");}
        else if (c == '=') {token.op(string(1, c1) + '=');} 
        else {
            inStream.putback(c);
            token.symbol(c1);
        }
     } 
    else if (c == '"') {token.setString(readString(c));} 
    else if (c == '\'') {token.setString(readString(c));}
    else if (c == '#') {
        string cmnt = "#";
        while (inStream.get(c) && c != '\n')
            cmnt += c;
        token.setCmnt(cmnt);
        inStream.putback(c);
    } else if (isalpha(c)) { 
        // put c back into the stream so we can read the entire name in a function.
        inStream.putback(c);
        string name = readName();
        if (name == "or") {token.op("or");}
        else if (name == "and") {token.op("and");}
        else if (name == "not") {token.op("not");}
        else {token.setName(name);}
            
    } else {
        cout << "Unknown character in input. ->" << c << "<-" << " occured at this line, "<< atLine + 1 << endl;
        exit(1);
    }
    _tokens.push_back(token);
    return lastToken = token;
}


void Tokenizer::ungetToken() {
    ungottenToken = true;
}

void Tokenizer::printProcessedTokens() {
    for(auto iter = _tokens.begin(); iter != _tokens.end(); ++iter) {
        iter->print();
        cout << endl;
    }
}
// making the symbol table for functions 
SymTab::SymTab() {
    i = 0;
    map<string, TypeDescriptor*> firstSymTab;
    symTabs.push_back(firstSymTab);
}

bool SymTab::isDefined(string vName) {
    return symTabs[i].find(vName) != symTabs[i].end();
}

void SymTab::setValueFor(string vName, TypeDescriptor* td) {
    // Define a variable by setting its initial value.
    if (isDefined(vName)) {
        // delete symTabs[i].find(vName)->second;
        symTabs[i].erase(vName);
    }
    symTabs[i][vName] = td;
}

TypeDescriptor* SymTab::getValueFor(string vName) {
    if( ! isDefined(vName)) {
		cout << "SymTab::getValueFor: " << vName << " has not been defined.\n";
		exit(1);
    }

    return symTabs[i].find(vName)->second;
}

void SymTab::print() {
    for(auto it = symTabs[i].begin(); it != symTabs[i].end(); it++) {
        cout << it->first << " = ";
        it->second->print();
        cout << endl;
    }
}

void SymTab::openScope(map<string, TypeDescriptor*> newSymTab) {
    symTabs.push_back(newSymTab);
    i++;
}

void SymTab::closeScope() {
    symTabs.pop_back();
    i--;
}

void SymTab::storeReturnValue(TypeDescriptor *val) {
    returnVal = val;
}

TypeDescriptor* SymTab::getReturnValue() {
    return returnVal;
}

// now getting the tree code 
void die(string where, string op, TypeDescriptor::types lType, TypeDescriptor::types rType) {
    cout << where << " Unsupported operand types for " << op << ": ";
    switch ( lType ) {
        case ( TypeDescriptor::INTEGER ):
            cout << "Integer";
            break;
        case ( TypeDescriptor::STRING ):
            cout << "String";
            break;
        case ( TypeDescriptor::ARRAY ):
            cout << "Array";
            break;
    }
    cout << " and ";
    switch ( rType ) {
        case ( TypeDescriptor::INTEGER ):
            cout << "Integer";
            break;
        case ( TypeDescriptor::STRING ):
            cout << "String";
            break;
        case ( TypeDescriptor::ARRAY ):
            cout << "Array";
            break;
    }
    cout << " occured at this line, "<< atLine + 1 << endl;
    exit(1);
}

// ExprNode
ExprNode::ExprNode(Token token): _token{token} {}

Token ExprNode::token() const { return _token; }

// InfixExprNode functions
InfixExprNode::InfixExprNode(Token tk) : ExprNode{tk}, _left(nullptr), _right(nullptr) {}

ExprNode *&InfixExprNode::left() { return _left; }

ExprNode *&InfixExprNode::right() { return _right; }

TypeDescriptor* InfixExprNode::evaluate(SymTab &symTab) {
    // evaluates an infix expression using a post-order traversal of the expression tree.
    TypeDescriptor* lValue = left()->evaluate(symTab);
    TypeDescriptor::types lType = lValue->type();
    TypeDescriptor* rValue = right()->evaluate(symTab);
    TypeDescriptor::types rType = rValue->type();
    if( token().isAdditionOperator() ) {
        if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(dynamic_cast<IntegerDescriptor *>(lValue)->value() +
                                         dynamic_cast<IntegerDescriptor *>(rValue)->value());
        else if ( lType == TypeDescriptor::STRING && rType == TypeDescriptor::STRING )
            return new StringDescriptor(dynamic_cast<StringDescriptor*>(lValue)->value() +
                                        dynamic_cast<StringDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::ARRAY && rType == TypeDescriptor::ARRAY ) {
            ArrayDescriptor* lArray = dynamic_cast<ArrayDescriptor*>(lValue);
            ArrayDescriptor* rArray = dynamic_cast<ArrayDescriptor*>(rValue);
            vector<TypeDescriptor*> newArray;
            
            for (int i = 0; i < lArray->numElements(); i++)
                newArray.push_back(lArray->value()[i]);
            for (int i = 0; i < rArray->numElements(); i++)
                newArray.push_back(rArray->value()[i]);
            
            return new ArrayDescriptor(newArray, lArray->elType());
        }
        die("InfixExprNode::evaluate", "+", lType, rType);
    }
    else if(token().isSubtractionOperator()) {
        if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() -
                                         dynamic_cast<IntegerDescriptor*>(rValue)->value());
        // If operand types are unsupported
        die("InfixExprNode::evaluate", "-", lType, rType);
    }
    else if(token().isMultiplicationOperator()) {
        if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() *
                                         dynamic_cast<IntegerDescriptor*>(rValue)->value());
        // If operand types are unsupported
        die("InfixExprNode::evaluate", "*", lType, rType);
    }
    else {
        cout << "InfixExprNode::evaluate: don't know how to evaluate this operator\n" <<  atLine;
;
        token().print();
        cout << endl;
        exit(2);
    }

    return nullptr; // Will never reach here
}

void InfixExprNode::print() {
    _left->print();
    token().print();
    _right->print();
}

// RelExprNode
RelExprNode::RelExprNode(Token tk) : ExprNode{tk}, _left(nullptr), _right(nullptr) {}

ExprNode *&RelExprNode::left() { return _left; }

ExprNode *&RelExprNode::right() { return _right; }

TypeDescriptor* RelExprNode::evaluate(SymTab &symTab) {
    // evaluates a relational expression using a post-order traversal of the expression tree.
    TypeDescriptor* lValue = left()->evaluate(symTab);
    TypeDescriptor::types lType = lValue->type();
    TypeDescriptor* rValue = right()->evaluate(symTab);
    TypeDescriptor::types rType = rValue->type();
    if (token().isGreaterThanOperator()) {
        if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() >
                                         dynamic_cast<IntegerDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::STRING && rType == TypeDescriptor::STRING )
            return new IntegerDescriptor(dynamic_cast<StringDescriptor*>(lValue)->value() >
                                         dynamic_cast<StringDescriptor*>(rValue)->value());
        // If operand types are unsupported
        die("RelExprNode::evaluate", ">", lType, rType);
    }
    else if (token().isGreaterThanOrEqualToOperator()) {
        if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() >=
                                         dynamic_cast<IntegerDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::STRING && rType == TypeDescriptor::STRING )
            return new IntegerDescriptor(dynamic_cast<StringDescriptor*>(lValue)->value() >=
                                         dynamic_cast<StringDescriptor*>(rValue)->value());
        // If operand types are unsupported
        die("RelExprNode::evaluate", ">=", lType, rType);
    }
    else if (token().isLessThanOperator()) {
        if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() <
                                         dynamic_cast<IntegerDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::STRING && rType == TypeDescriptor::STRING )
            return new IntegerDescriptor(dynamic_cast<StringDescriptor*>(lValue)->value() <
                                         dynamic_cast<StringDescriptor*>(rValue)->value());
        // If operand types are unsupported
        die("RelExprNode::evaluate", "<", lType, rType);
    }
    else if (token().isLessThanOrEqualToOperator()) {
        if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() <=
                                         dynamic_cast<IntegerDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::STRING && rType == TypeDescriptor::STRING )
            return new IntegerDescriptor(dynamic_cast<StringDescriptor*>(lValue)->value() <=
                                         dynamic_cast<StringDescriptor*>(rValue)->value());
        // If operand types are unsupported
        die("RelExprNode::evaluate", "<=", lType, rType);
    }
    else if (token().isEqualityOperator()) {
        if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() ==
                                         dynamic_cast<IntegerDescriptor*>(rValue)->value());
        else if ( lType == TypeDescriptor::STRING && rType == TypeDescriptor::STRING )
            return new IntegerDescriptor(dynamic_cast<StringDescriptor*>(lValue)->value() ==
                                         dynamic_cast<StringDescriptor*>(rValue)->value());
        // If operand types are unsupported
        die("RelExprNode::evaluate", "==", lType, rType);
    }
    else {
        cout << "RelExprNode::evaluate: don't know how to evaluate this operator\n" << atLine + 1;
        token().print();
        cout << endl;
        exit(2);
    }

    return nullptr; // Will never reach here
}

void RelExprNode::print() {
    _left->print();
    token().print();
    _right->print();
}

// BoolExprNode
BoolExprNode::BoolExprNode(Token tk) : ExprNode{tk}, _left(nullptr), _right(nullptr) {}

ExprNode *&BoolExprNode::left() { return _left; }

ExprNode *&BoolExprNode::right() { return _right; }

TypeDescriptor* BoolExprNode::evaluate(SymTab &symTab) {
    // evaluates a boolean expression using a post-order traversal of the expression tree.
    TypeDescriptor* lValue = left()->evaluate(symTab);
    TypeDescriptor::types lType = lValue->type();
    TypeDescriptor* rValue = right()->evaluate(symTab);
    TypeDescriptor::types rType = rValue->type();
	
    if (token().isOrOperator()) {
        if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() ||
                                         dynamic_cast<IntegerDescriptor*>(rValue)->value());
        // If operand types are unsupported
        die("BoolExprNode::evaluate", "or", lType, rType);
    }
    else if (token().isAndOperator()) {
        if ( lType == TypeDescriptor::INTEGER && rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(dynamic_cast<IntegerDescriptor*>(lValue)->value() &&
                                         dynamic_cast<IntegerDescriptor*>(rValue)->value());
        // If operand types are unsupported
        die("BoolExprNode::evaluate", "and", lType, rType);
    }
    else {
        cout << "BoolExprNode::evaluate: don't know how to evaluate this operator\n";
        token().print();
        cout << " occured at this line, "<< atLine + 1 << endl;
        exit(2);
    }

    return nullptr; // Will never reach here
}

void BoolExprNode::print() {
    _left->print();
    token().print();
    _right->print();
}

// NotExprNode
NotExprNode::NotExprNode(Token tk) : ExprNode{tk}, _right(nullptr) {}

ExprNode *&NotExprNode::right() { return _right; }

TypeDescriptor* NotExprNode::evaluate(SymTab &symTab) {
    // evaluates a not expression using a post-order traversal of the expression tree.
    TypeDescriptor* rValue = right()->evaluate(symTab);
    TypeDescriptor::types rType = rValue->type();
    if (token().isNotOperator()) {
        if ( rType == TypeDescriptor::INTEGER )
            return new IntegerDescriptor(! dynamic_cast<IntegerDescriptor*>(rValue)->value());
        // If operand types are unsupported
        cout << "NotExprNode::evaluate Bad operand type for unary operator not: '";
        switch ( rType ) {
            case ( TypeDescriptor::STRING ):
                cout << "String";
                break;
            case ( TypeDescriptor::ARRAY ):
                cout << "Array";
                break;
        }
        cout << '\'' << " occured at this line, "<< atLine + 1 << endl;
        exit(1);
    }
    else {
        cout << "NotExprNode::evaluate: don't know how to evaluate this operator\n";
        token().print();
        cout << endl;
        exit(2);
    }

    return nullptr; // Will never reach here
}

void NotExprNode::print() {
    token().print();
    _right->print();
}

// Subscription
Subscription::Subscription(Token varName, ExprNode* subscript): ExprNode{varName}, _subscript{subscript} {}

void Subscription::print() {
    token().print();
    cout << '[';
    _subscript->print();
    cout << ']';
}

TypeDescriptor* Subscription::evaluate(SymTab &symTab) {
    TypeDescriptor* subVal = subscript()->evaluate(symTab);
    TypeDescriptor::types subType = subVal->type();
    if (token().isName()) {
        if ( subType == TypeDescriptor::INTEGER ) {
            int index = dynamic_cast<IntegerDescriptor*>(subVal)->value();
            if (index < 0) {
                cout << "Subscription::evaluate Index out of bounds, less than 0" << " occured at this line, "<< atLine + 1 << endl;
                exit(1);
            }
            string varName = token().getName();
            if (symTab.isDefined(varName)) {
                TypeDescriptor *array = symTab.getValueFor(varName);
                if (array->type() == TypeDescriptor::ARRAY) {
                    vector<TypeDescriptor*> values = dynamic_cast<ArrayDescriptor*>(array)->value();
                    if (index >= values.size()) {
                        cout << "Subscription::evaluate Index out of bounds, larger than array size" << " occured at this line, "<< atLine + 1 << endl;
                        exit(1);
                    }
                    return values[index];
                } else {
                    cout << "Subscription::evaluate Can't subscript into a";
                    switch ( subType ) {
                        case ( TypeDescriptor::INTEGER ):
                            cout << "n Integer";
                            break;
                        case ( TypeDescriptor::STRING ):
                            cout << " String";
                            break;
                    }
                    cout << endl;
                    exit(1);
                }
            } else {
                cout << "Subscription::evaluate Use of undefined variable, " << varName << " occured at this line, "<< atLine + 1 << endl;
                exit(1);
            }
        }
        cout << "Subscription::evaluate Bad index type for subscription operation: '";
        switch ( subType ) {
            case ( TypeDescriptor::STRING ):
                cout << "String";
                break;
            case ( TypeDescriptor::ARRAY ):
                cout << "Array";
                break;
        }
        cout << '\'' << " occured at this line, "<< atLine + 1 << endl;
        exit(1);

    } else {
        cout << "Subscription::evaluate: don't know how to evaluate this operator\n";
        token().print();
        cout << " occured at this line, "<< atLine + 1 << endl;
        exit(2);
    }
}

ExprNode *&Subscription::subscript() { return _subscript; }

// Call
Call::Call(Token funcName, Functions* funcList, vector<ExprNode*> args): ExprNode{funcName}, _funcList{funcList}, _args{args} {}

void Call::print() {
    token().print();
    cout << '(';
    if (!_args.empty()) {
        _args[0]->print();
    }
    for (int i = 1; i < _args.size(); i++) {
        cout << ", ";
        _args[i]->print();
    }
    cout << ')';
}

TypeDescriptor* Call::evaluate(SymTab &symTab) {
    Function* func = _funcList->findFunction(token().getName());
    vector<string> params = func->args();
    if (params.size() != _args.size()) {
        cout << "Call::evaluate Unequal number or arguments and parameters" << " occured at this line, "<< atLine + 1 << endl;
        exit(1);
    }

    map<string, TypeDescriptor*> newSymTab;
    for (int i = 0; i < params.size(); i++) {
        newSymTab[params[i]] = _args[i]->evaluate(symTab);
    }

    symTab.openScope(newSymTab);

    func->body()->evaluate(symTab);

    TypeDescriptor *ret = symTab.getReturnValue();
    if (ret->type() == TypeDescriptor::NOT_SPECIFIED) {
        cout << "Call::evaluate Function has no return value so it cannot be used as an ExprNode" << " occured at this line, "<< atLine + 1 << endl;
        exit(1);
    }

    symTab.closeScope();

    return ret;
}

vector<ExprNode*> &Call::args() { return _args; }

// Len
Len::Len(Token varName): ExprNode{varName} {}

void Len::print() {
    cout << "len(";
    token().print();
    cout << ")";
}

TypeDescriptor* Len::evaluate(SymTab &symTab) {
    TypeDescriptor* value = symTab.getValueFor(token().getName());
    if (value->type() != TypeDescriptor::ARRAY) {
        cout << "Len::evaluate: Len only works when called on an array" << " occured at this line, "<< atLine + 1 << endl;
        exit(1);
    }
    int ret = dynamic_cast<ArrayDescriptor*>(value)->numElements();
	cout << ret << endl;
	
    return new IntegerDescriptor(ret);
}

// WholeNumber
WholeNumber::WholeNumber(Token token): ExprNode{token} {}

void WholeNumber::print() {
    token().print();
}

TypeDescriptor* WholeNumber::evaluate(SymTab &symTab) {
    return new IntegerDescriptor(token().getWholeNumber());
}

// Variable
Variable::Variable(Token token): ExprNode{token} {}

void Variable::print() {
    token().print();
}

TypeDescriptor* Variable::evaluate(SymTab &symTab) {
    string name = token().getName();
    if( ! symTab.isDefined(token().getName())) {
        cout << "Variable::evaluate Use of undefined variable, " << token().getName() << " occured at this line, "<< atLine + 1 << endl;
        exit(1);
    }
    
    return symTab.getValueFor(token().getName());
}


// String
String::String(Token token): ExprNode{token} {}

void String::print() {
    token().print();
}

TypeDescriptor* String::evaluate(SymTab &symTab) {
    return new StringDescriptor(token().getString());
}

// functions dealing with them

void Functions::addFunction(Function *func) {
    functions().push_back(func);
}

Function* Functions::findFunction(std::string name) {
    for (int i = 0; i < _functions.size(); i++) {
        if (_functions[i]->name() == name)
            return _functions[i];
    }
    return nullptr;
}
// now doing the statements (if,while,print,assignment, array operations) and error hadling
// Statement
Statement::Statement() {}

// Statements

Statements::Statements() {}
void Statements::addStatement(Statement *statement) { _statements.push_back(statement); }

void Statements::print() {
    for (auto s: _statements)
        s->print();
}

void Statements::evaluate(SymTab &symTab) {
    for (auto s: _statements) {
        s->evaluate(symTab);
        if (dynamic_cast<ReturnStatement*>(s))
            return;
    }
}

// AssignmentStatement

AssignmentStatement::AssignmentStatement() : _lhsExpression{nullptr}, _rhsExpression{nullptr} {}

AssignmentStatement::AssignmentStatement(ExprNode *lhsExpr, ExprNode *rhsExpr):
        _lhsExpression{lhsExpr}, _rhsExpression{rhsExpr} {}

AssignmentStatement::AssignmentStatement(ExprNode *lhsExpr, vector<ExprNode *> rhsArray):
        _lhsExpression(lhsExpr), _rhsExpression{nullptr}, _rhsArray{rhsArray} {}

void AssignmentStatement::evaluate(SymTab &symTab) {
    if (_rhsExpression == nullptr) {
        if (dynamic_cast<Variable*>(lhsExpression()) == nullptr) {
            cout << "AssignmentStatement::evaluate Can't assign arrays to Subscripts (Multidimensional arrays are unsupported)\n";
            exit(1);
        }
        Variable* lhs = dynamic_cast<Variable*>(lhsExpression());
        if (_rhsArray.empty()) {
            symTab.setValueFor(lhs->token().getName(), new ArrayDescriptor());
            return;
        }
        vector<TypeDescriptor*> arr;
        TypeDescriptor* val = _rhsArray[0]->evaluate(symTab);
        TypeDescriptor::types arrType = val->type();
        if (arrType == TypeDescriptor::ARRAY || arrType == TypeDescriptor::NOT_SPECIFIED) {
            cout << "AssignmentStatement::evaluate Right hand side array elements are of an unsupported type\n";
            exit(1);
        }
        arr.push_back(val);
        for (int i = 1; i < _rhsArray.size(); i++) {
            val = _rhsArray[i]->evaluate(symTab);
            if (val->type() != arrType) {
                cout << "AssignmentStatement::evaluate Data types of all elements in the right hand side array must be the same\n";
                exit(1);
            }
            arr.push_back(val);
        }
        ArrayDescriptor* arrDesc = new ArrayDescriptor(arr, arrType);
        symTab.setValueFor(lhs->token().getName(), arrDesc);
        return;
    }
    TypeDescriptor* rhs = rhsExpression()->evaluate(symTab);
    if (dynamic_cast<Variable*>(lhsExpression()) != nullptr) {
        Variable* lhs = dynamic_cast<Variable*>(lhsExpression());
        symTab.setValueFor(lhs->token().getName(), rhs);
    }
    else if (dynamic_cast<Subscription*>(lhsExpression()) != nullptr) {
        Subscription* lhs = dynamic_cast<Subscription*>(lhsExpression());

        TypeDescriptor* subVal = lhs->subscript()->evaluate(symTab);
        if (subVal->type() != TypeDescriptor::INTEGER) {
            cout << "AssignmentStatement::evaluate Left hand side Subscription is not an Integer\n";
            exit(1);
        }
        int index = dynamic_cast<IntegerDescriptor*>(subVal)->value();
        if (index < 0) {
            cout << "AssignmentStatement::evaluate Left hand side Subscription index out of bounds, less than 0" << " occured at this line, "<< atLine + 1 << endl;
            exit(1);
        }
        string varName = lhs->token().getName();
        if (!symTab.isDefined(varName)) {
            cout << "AssignmentStatement::evaluate Use of undefined variable, " << varName << " occured at this line, "<< atLine + 1 <<  endl;
            exit(1);
        }
        TypeDescriptor *array = symTab.getValueFor(varName);
        if (array->type() != TypeDescriptor::ARRAY) {
            cout << "AssignmentStatement::evaluate Left hand side variable is not subscriptable\n";
            exit(1);
        }
        ArrayDescriptor* arrDesc = dynamic_cast<ArrayDescriptor *>(array);
        vector<TypeDescriptor *> values = arrDesc->value();
        if (index >= values.size()) {
            cout << "AssignmentStatement::evaluate Left hand side index out of bounds, larger than array size" << " occured at this line, "<< atLine + 1 << endl;
            exit(1);
        }
        if (rhs->type() != arrDesc->elType()) {
            cout << "AssignmentStatement::evaluate Left hand side array's element type does not match right hand side type" << " occured at this line, "<< atLine + 1 <<  endl;
            exit(1);
        }
        arrDesc->value()[index] = rhs;
    } else {
        cout << "AssignmentStatement::evaluate Left hand side expression must be either a Variable or Subscription\n";
        exit(1);
    }
}

ExprNode *&AssignmentStatement::lhsExpression() {
    return _lhsExpression;
}

ExprNode *&AssignmentStatement::rhsExpression() {
    return _rhsExpression;
}

vector<ExprNode*> AssignmentStatement::rhsArray() {
    return _rhsArray;
}

void AssignmentStatement::print() {
    _lhsExpression->print();
    cout << " = ";
    if (_rhsExpression == nullptr) {
        if (_rhsArray.empty()) {
            cout << "[]" << " occured at this line, "<< atLine + 1 << endl;
            return;
        }
        cout << '[';
        _rhsArray[0]->print();
        for (int i = 1; i < _rhsArray.size(); i++) {
            cout << ", ";
            _rhsArray[i]->print();
        }
        cout << ']';
    } else {
        _rhsExpression->print();
    }
    cout << endl;
}

// PrintStatement

PrintStatement::PrintStatement() : _list{nullptr} {}

PrintStatement::PrintStatement(vector<ExprNode*> list):
        _list{list} {}

void PrintStatement::evaluate(SymTab &symTab) {
    for (int i = 0; i < _list.size(); i++) {
        _list[i]->evaluate(symTab)->print();
        if (i < _list.size() - 1)
            cout << " ";
    }
    cout << endl;
}

vector<ExprNode*> &PrintStatement::list() {
    return _list;
}

void PrintStatement::print() {
    cout << "print ";
    for (int i = 0; i < _list.size(); i++) {
        _list[i]->print();
        if (i < _list.size() - 1)
            cout << ", ";
    }
    cout << endl;
}

// IfStatement

IfStatement::IfStatement() : _conditions{nullptr}, _bodies{nullptr} {}

IfStatement::IfStatement(vector<ExprNode*> conditions, vector<Statements*> bodies): _conditions{conditions},
                            _bodies{bodies} {}

void IfStatement::evaluate(SymTab &symTab) {
    bool done = false;
    int i;
    int size = conditions().size();
    for (i = 0; i < size; i++) {
        TypeDescriptor *cond = conditions()[i]->evaluate(symTab);
        if (cond->type() != TypeDescriptor::INTEGER) {
            cout << "IfStatement::evaluate The condition must evaluate to an integer\n";
            exit(1);
        }
        int val = dynamic_cast<IntegerDescriptor *>(cond)->value();
        if (val == 1) {
            bodies()[i]->evaluate(symTab);
            done = true;
            break;
        } else if (val == 0) {
            continue;
        } else {
            cout << "IfStatement::evaluate The condition must evaluate to either 0 or 1\n";
            exit(1);
        }
    }
    if (!done && bodies().size() > size) { // if there's an else statement
        bodies()[i]->evaluate(symTab);
    }
}

vector<ExprNode*> &IfStatement::conditions() {
    return _conditions;
}

vector<Statements*> &IfStatement::bodies() {
    return _bodies;
}

void IfStatement::print() {
    cout << "if ";
    conditions()[0]->print();
    cout << ":" << endl;
    bodies()[0]->print();
    int i;
    int size = conditions().size();
    for (i = 1; i < size; i++) {
        cout << "elif ";
        conditions()[i]->print();
        cout << ":" << endl;
        bodies()[i]->print();
    }
    if (bodies().size() > size) { // if there's an else statement
        cout << "else:" << endl;
        bodies()[i]->print();
    }
}

// ArrayOpStatement

ArrayOpStatement::ArrayOpStatement() : _arg{nullptr}, _varName{Token()} {}

ArrayOpStatement::ArrayOpStatement(Token varName, ExprNode* arg): _arg{arg}, _varName{varName} {}

void ArrayOpStatement::evaluate(SymTab &symTab) {
    TypeDescriptor* value = symTab.getValueFor(varName().getName());
    if (!symTab.isDefined(varName().getName())) {
        cout << "ArrayOpStatement::evaluate Use of undefined variable, " << varName().getName() << " occured at this line, "<< atLine + 1 << endl;
        exit(1);
    }
    if (arg() == nullptr) {
        if (value->type() != TypeDescriptor::ARRAY) {
            cout << "ArrayPopStatement::evaluate TypeError: Can only pop from an array" << " occured at this line, "<< atLine + 1 << endl;
            exit(1);
        }
        ArrayDescriptor* arr = dynamic_cast<ArrayDescriptor*>(value);
        int numElems = arr->numElements();
        if (numElems == 0) {
            cout << "ArrayPopStatement::evaluate Array is empty, no elements to pop" << " occured at this line, "<< atLine + 1 <<  endl;
            exit(1);
        }
        arr->value().pop_back();
        arr->numElements() = numElems - 1;
    } else {
        if (value->type() != TypeDescriptor::ARRAY) {
            cout << "ArrayPushStatement::evaluate TypeError: Can only push onto an array" << " occured at this line, "<< atLine + 1 <<  endl;
            exit(1);
        }
        ArrayDescriptor* arr = dynamic_cast<ArrayDescriptor*>(value);
        TypeDescriptor* elem = arg()->evaluate(symTab);
        if (arr->numElements() == 0) {
            arr->elType() = elem->type();
        } else if (arr->elType() != elem->type()) {
            cout << "ArrayPushStatement::evaluate TypeError: Can only push elements of the same type onto this array" << " occured at this line, "<< atLine + 1 << endl;
            exit(1);
        }
        arr->value().push_back(elem);
        arr->numElements() = arr->numElements() + 1;
    }
}

Token &ArrayOpStatement::varName() {
    return _varName;
}

ExprNode* &ArrayOpStatement::arg() {
    return _arg;
}

void ArrayOpStatement::print() {
    cout << varName().getName() << '.';
    if (arg() == nullptr) {
        cout << "pop()" << endl;
    } else {
        cout << "push(";
        arg()->print();
        cout << ")" << endl;
    }
}

// CallStatement

CallStatement::CallStatement(Token funcName, vector<string> params, vector<ExprNode*> args, Statements* body)
    : _funcName{funcName}, _params{params}, _args{args}, _body{body} {}

void CallStatement::evaluate(SymTab &symTab) {
    if (_params.size() != _args.size()) {
        cout << "CallStatement::evaluate Unequal number or arguments and parameters" << " occured at this line, "<< atLine + 1 << endl;
        exit(1);
    }

    map<string, TypeDescriptor*> newSymTab;
    for (int i = 0; i < _params.size(); i++) {
        newSymTab[_params[i]] = _args[i]->evaluate(symTab);
    }

    symTab.openScope(newSymTab);

    _body->evaluate(symTab);

    TypeDescriptor *ret = symTab.getReturnValue(); // it returns, but return value goes nowhere

    symTab.closeScope();
}

vector<string> &CallStatement::params() {
    return _params;
}

vector<ExprNode*> &CallStatement::args() {
    return _args;
}

Statements* &CallStatement::body() {
    return _body;
}

void CallStatement::print() {
    cout << _funcName.getName() << '(';
    if (!_args.empty()) {
        _args[0]->print();
    }
    for (int i = 1; i < _args.size(); i++) {
        cout << ", ";
        _args[i]->print();
    }
    cout << ')' << endl;
}

// ReturnStatement

ReturnStatement::ReturnStatement(ExprNode *retVal) : _retVal{retVal} {}

void ReturnStatement::evaluate(SymTab &symTab) {
    if (symTab.getI() == 0) {
        cout << "ReturnStatement::evaluate Can't return from global scope" << " occured at this line, "<< atLine + 1 << endl;
        exit(1);
    }
    symTab.storeReturnValue(_retVal->evaluate(symTab));
}

ExprNode* &ReturnStatement::retVal() {
    return _retVal;
}

void ReturnStatement::print() {
    cout << "return ";
    _retVal->print();
    cout << endl;
}

// parser (aka using token generate tree)

void Parser::die(string where, string message, Token &token) {
    cout << "Syntax Error " << where << " " << message << " ";
    token.print();
    cout <<" occured at this line, "<< atLine << endl;
    // comment the next to line for finish
    cout << "\nThe following is a list of tokens that have been identified up to this point.\n";
    tokenizer.printProcessedTokens();
    exit(1);
}

Statements *Parser::statements() {
    // This function is called when we KNOW that we are about to parse
    // a series of statements.

    // This function parses the grammar rules:

    // file_input: {NEWLINE | stmt}* ENDMARKER
    // stmt: simple_stmt | compound_stmt
    // simple_stmt: (print_stmt | assign_stmt | array_ops | call_stmt | return_stmt) NEWLINE
    // compound_stmt: if_stmt | for_stmt | func_def

    Statements *stmts = new Statements();
    Token tok = tokenizer.getToken();
    while (tok.eol())
        tok = tokenizer.getToken();
    while (tok.isName() || tok.isCmnt()) {
        if (tok.isKeyword()) {
            if (tok.isKeywordPrint()) {
                PrintStatement *printStmt = printStatement();
                stmts->addStatement(printStmt);
            } else if (tok.isKeywordIf()) {
                IfStatement *ifStmt = ifStatement();
                stmts->addStatement(ifStmt);
            } else if (tok.isKeywordReturn()) {
                ReturnStatement *returnStmt = returnStatement();
                stmts->addStatement(returnStmt);
            } else if (tok.isKeywordDef()) {
                funcs->addFunction(funcDef());
            } else {
                die("Parser::statements", "Invalid keyword", tok);
            }
        } else if (tok.isName()) {
            Token next = tokenizer.getToken();
            tokenizer.ungetToken();
            if ( next.isAssignmentOperator() || next.isOpenBracket() ) {
                AssignmentStatement *assignStmt = assignStatement(tok);
                stmts->addStatement(assignStmt);
            } else if ( next.isOpenParen() ) {
                tokenizer.getToken();
                CallStatement *callStmt = callStatement(tok);
                stmts->addStatement(callStmt);
            }
        } else if (tok.isCmnt()) {
            tok = tokenizer.getToken();
            if (!tok.eol() && !tok.eof() && !tok.dedent())
                die("Parser::statements", "Expected an eol token, instead got", tok);
        }
        if (!tok.eof()) {
            tok = tokenizer.getToken();
            while (tok.eol())
                tok = tokenizer.getToken();
        }
    }
    while (tok.eol())
        tok = tokenizer.getToken();
    tokenizer.ungetToken();
    return stmts;
}

AssignmentStatement *Parser::assignStatement(Token varName) {
    // This function parses the grammar rules:

    // assign_stmt: (ID | subscription) '=' (test | array_init)
    // ID: [_a-zA-Z][_a-zA-Z0-9]*

    if (!varName.isName())
        die("Parser::assignStatement", "Expected a name token, instead got", varName);

    Token tok = tokenizer.getToken();
    if (!tok.isAssignmentOperator() && !tok.isOpenBracket())
        die("Parser::assignStatement", "Expected an equal sign or an open bracket, instead got", tok);
    ExprNode *leftHandSideExpr;
    if (tok.isOpenBracket()) {
        leftHandSideExpr = subscription(varName);
        tok = tokenizer.getToken();
        if (!tok.isAssignmentOperator())
            die("Parser::assignStatement", "Expected an equal sign, instead got", tok);
    }
    else {
        leftHandSideExpr = new Variable(varName);
    }

    ExprNode *rightHandSideExpr = nullptr;
    vector<ExprNode*> rightHandSideArr;
    tok = tokenizer.getToken();
    if (tok.isOpenBracket()) {
        rightHandSideArr = array_init();
    } else {
        tokenizer.ungetToken();
        rightHandSideExpr = test();
    }

    Token endLine = tokenizer.getToken();
    if (!endLine.eol() && !endLine.eof() && !endLine.isCmnt() && !endLine.dedent())
        die("Parser::assignStatement", "Expected an end of line token, instead got", endLine);
    if (endLine.eof())
        tokenizer.ungetToken();

    if (rightHandSideExpr == nullptr)
        return new AssignmentStatement(leftHandSideExpr, rightHandSideArr);
    else
        return new AssignmentStatement(leftHandSideExpr, rightHandSideExpr);
}

PrintStatement *Parser::printStatement() {
    // This function parses the grammar rule:

    // print_stmt: 'print' [ testlist ]

    Token t = tokenizer.getToken();
    bool conditionCheck = false;
    if (t.eol() || t.eof() || t.isCmnt()) {
        vector<ExprNode*> empty;
        if (t.isCmnt())
            tokenizer.ungetToken();
        return new PrintStatement(empty);
    } else if (!t.isOpenParen()){
        tokenizer.ungetToken();
    } else{
        bool conditionCheck = true;
    }


    vector<ExprNode*> list = testlist();

    Token endLine = tokenizer.getToken();
    if (conditionCheck && !endLine.isCloseParen()){
        die("Parser::printStatement", "Expected ')' token, instead got", endLine);
    }else {
        endLine = tokenizer.getToken();
    }
        

    if (!endLine.eol() && !endLine.eof() && !endLine.isCmnt() && !endLine.dedent() )
        die("Parser::printStatement", "Expected an end of line token, instead got", endLine);
    if (endLine.eof() || endLine.isCmnt() || endLine.dedent())
        tokenizer.ungetToken();

    return new PrintStatement(list);
}

IfStatement *Parser::ifStatement() {
    // This function parses the grammar rule:

    // if_stmt: ’if’ test ’:’ suite {’elif’ test ’:’ suite}* [’else’ ’:’ suite]

    vector<ExprNode*> conditions;
    conditions.push_back(test());

    Token t = tokenizer.getToken();
    if (!t.isColon())
        die("Parser::ifStatement", "Expected a colon, instead got", t);

    vector<Statements*> bodies;
    bodies.push_back(suite());

    t = tokenizer.getToken();
    while (t.isKeywordElif()) {
        conditions.push_back(test());
        t = tokenizer.getToken();
        if (!t.isColon())
            die("Parser::ifStatement", "Expected a colon, instead got", t);
        bodies.push_back(suite());
        t = tokenizer.getToken();
    }

    if (t.isKeywordElse()) {
        t = tokenizer.getToken();
        if (!t.isColon())
            die("Parser::ifStatement", "Expected a colon, instead got", t);
        bodies.push_back(suite());
    }
    else
        tokenizer.ungetToken();

    return new IfStatement(conditions, bodies);
}

Function *Parser::funcDef() {
    // This function parses the grammar rule:

    // func_def: 'def' ID '(' [parameter_list] ')' ':' suite

    Token name = tokenizer.getToken();
    if (!name.isName())
        die("Parser::funcDef", "Expected a name token, instead got", name);
    Token tok = tokenizer.getToken();
    if (!tok.isOpenParen())
        die("Parser::funcDef", "Expected an open parenthesis token, instead got", tok);

    vector<string> args;
    tok = tokenizer.getToken();
    if (!tok.isCloseParen()) {
        tokenizer.ungetToken();
        args = parameter_list();
        tok = tokenizer.getToken();
        if (!tok.isCloseParen())
            die("Parser::funcDef", "Expected a close parenthesis token, instead got", tok);
    }

    tok = tokenizer.getToken();
    if (!tok.isColon())
        die("Parser::funcDef", "Expected a colon token, instead got", tok);

    Statements* body = suite();

    return new Function(name.getName(), args, body);
}

vector<string> Parser::parameter_list() {
    // This function parses the grammar rule:

    // parameter_list: ID {',' ID}*

    vector<string> list;
    Token tok = tokenizer.getToken();
    if (!tok.isName())
        die("Parser::parameter_list", "Expected a name token, instead got", tok);
    list.push_back(tok.getName());
    tok = tokenizer.getToken();
    while (tok.isComma()) {
        tok = tokenizer.getToken();
        if (!tok.isName())
            die("Parser::parameter_list", "Expected a name token, instead got", tok);
        list.push_back(tok.getName());
        tok = tokenizer.getToken();
    }
    tokenizer.ungetToken();
    return list;
}

CallStatement *Parser::callStatement(Token varName) {
    // This function parses the grammar rule:

    // call_stmt: call

    Function* func = funcs->findFunction(varName.getName());
    if (!func)
        die("Parser::callStatement", "No function exists with the name:", varName);
    Statements* body = func->body();
    vector<string> params = func->args();

    Token tok = tokenizer.getToken();
    vector<ExprNode*> args;
    if (!tok.isCloseParen()) {
        tokenizer.ungetToken();
        args = testlist();
        tok = tokenizer.getToken();
        if (!tok.isCloseParen())
            die("Parser::callStatement", "Expected a close parenthesis token, instead got", tok);
    }
    return new CallStatement(varName, params, args, body);
}

ExprNode *Parser::call(Token varName) {
    // This function parses the grammar rule:

    // call: ID '(' {testlist} ')'

    Function* func = funcs->findFunction(varName.getName());
    if (!func)
        die("Parser::call", "No function exists with the name:", varName);
    Statements* body = func->body();
    vector<string> params = func->args();

    Token tok = tokenizer.getToken();
    vector<ExprNode*> args;
    if (!tok.isCloseParen()) {
        tokenizer.ungetToken();
        args = testlist();
        tok = tokenizer.getToken();
        if (!tok.isCloseParen())
            die("Parser::call", "Expected a close parenthesis token, instead got", tok);
    }
    return new Call(varName, funcs, args);
}

ReturnStatement *Parser::returnStatement() {
    // This function parses the grammar rule:

    // return_stmt: 'return' [test]

    Token tok = tokenizer.getToken();
    if (tok.eof() || tok.eol() || tok.isCmnt()) {
        ExprNode *none = nullptr;
        return new ReturnStatement(none);
    } else {
        tokenizer.ungetToken();
        return new ReturnStatement(test());
    }
}


Subscription *Parser::subscription(Token varName) {
    
    // Rule (subscription: ID '[' test ']') 

    ExprNode *subscript = test();

    Token tok = tokenizer.getToken();
    if (!tok.isCloseBracket())
        die("Parser::subscription", "Expected a close bracket token, instead got", tok);

    return new Subscription(varName, subscript);
}

vector<ExprNode*> Parser::array_init() {
    // This function parses the grammar rule:

    // array_init: '[' [ testlist ] ']'

    Token t = tokenizer.getToken();
    if (t.isCloseBracket()) {
        vector<ExprNode*> empty;
        return empty;
    } else {
        tokenizer.ungetToken();
    }

    vector<ExprNode*> list = testlist();

    t = tokenizer.getToken();
    if (!t.isCloseBracket())
        die("Parser::array_init", "Expected a close bracket token, instead got", t);

    return list;
}

Statements *Parser::suite() {
    // This function parses the grammar rule:

    // suite: NEWLINE INDENT stmt+ DEDENT

    Token t = tokenizer.getToken();
    if (t.isCmnt())
        t = tokenizer.getToken();
    if (!t.eol())
        die("Parser::suite", "Expected an end of line token, instead got", t);

    t = tokenizer.getToken();
    while (t.eol())
        t = tokenizer.getToken();
    if (!t.indent())
        die("Parser::suite", "Expected an indent token, instead got", t);

    Statements *stmts = statements();

    t = tokenizer.getToken();
    if (!t.dedent() && !t.eof())
        die("Parser::suite", "Expected a dedent token, instead got", t);

    return stmts;
}

vector<ExprNode*> Parser::testlist() {
    // This function parses the grammar rule:

    // testlist: test {',' test}*

    vector<ExprNode*> list;
    list.push_back(test());
    Token tok = tokenizer.getToken();
    while (tok.isComma()) {
        list.push_back(test());
        tok = tokenizer.getToken();
    }
    tokenizer.ungetToken();
    return list;
}

ExprNode* Parser::array_len() {
    // This function parses the grammar rule:

    // array_len: 'len' '(' ID ')'

    Token tok = tokenizer.getToken();
    if (!tok.isOpenParen()) {
        die("Parser::array_len", "Expected an open parenthesis token, instead got", tok);
    }
    tok = tokenizer.getToken();
    Len* len = new Len(tok);
    tok = tokenizer.getToken();
    if (!tok.isCloseParen()) {
        die("Parser::array_len", "Expected a close parenthesis token, instead got", tok);
    }
    return len;
}


ExprNode *Parser::test() {
    // This function parses the grammar rule:

    // test: or_test

    return or_test();
}

ExprNode *Parser::or_test() {
    // This function parses the grammar rule:

    // or_test: and_test {’or’ and_test}*

    ExprNode *left = and_test();
    Token tok = tokenizer.getToken();
    while (tok.isOrOperator()) {
        BoolExprNode *p = new BoolExprNode(tok);
        p->left() = left;
        p->right() = and_test();
        left = p;
        tok = tokenizer.getToken();
    }
    tokenizer.ungetToken();
    return left;
}

ExprNode *Parser::and_test() {
    // This function parses the grammar rule:

    // and_test: not_test {’and’ not_test}*

    ExprNode *left = not_test();
    Token tok = tokenizer.getToken();
    while (tok.isAndOperator()) {
        BoolExprNode *p = new BoolExprNode(tok);
        p->left() = left;
        p->right() = not_test();
        left = p;
        tok = tokenizer.getToken();
    }
    tokenizer.ungetToken();
    return left;
}

ExprNode *Parser::not_test() {
    // This function parses the grammar rule:

    // and_test: 'not' not_test | comparison

    int notCount = 0;
    Token tok = tokenizer.getToken();
    while(tok.isNotOperator()) {
        notCount++;
        tok = tokenizer.getToken();
    }
    tokenizer.ungetToken();
    if (notCount % 2 == 1) {
        Token n;
        n.op("not");
        NotExprNode *p = new NotExprNode(n);
        p->right() = comparison();
        return p;
    }
    else
        return comparison();
}

ExprNode *Parser::comparison() {
    // This function parses the grammar rules:

    // comparison: arith_expr {comp_op arith_expr}*
    // comp_op: '<'|'>'|'=='|'>='|'<='|'<>'|'!='

    ExprNode *left = arithExpr();
    Token tok = tokenizer.getToken();
    while (tok.isRelationalOperator()) {
        RelExprNode *p = new RelExprNode(tok);
        p->left() = left;
        p->right() = arithExpr();
        left = p;
        tok = tokenizer.getToken();
    }
    tokenizer.ungetToken();
    return left;
}

ExprNode *Parser::arithExpr() {
    // This function parses the grammar rule:

    // arith_expr: term {('+'|'-') term}*

    // However, the implementation makes the operator left associative.

    ExprNode *left = term();
    Token tok = tokenizer.getToken();
    while (tok.isAdditionOperator() || tok.isSubtractionOperator()) {
        InfixExprNode *p = new InfixExprNode(tok);
        p->left() = left;
        p->right() = term();
        left = p;
        tok = tokenizer.getToken();
    }
    tokenizer.ungetToken();
    return left;
}


ExprNode *Parser::term() {
    // This function parses the grammar rule:

    // term: factor {('*'|'/'|'%'|'//') factor}*

    // However, the implementation makes the operator left associate.

    ExprNode *left = factor();
    Token tok = tokenizer.getToken();

    while (tok.isMultiplicationOperator()) {
        InfixExprNode *p = new InfixExprNode(tok);
        p->left() = left;
        p->right() = factor();
        left = p;
        tok = tokenizer.getToken();
    }
    tokenizer.ungetToken();
    return left;
}

ExprNode *Parser::factor() {
    // This function parses the grammar rule:

    // factor: {'-'} factor | atom | call | subscription | array_len

    Token tok = tokenizer.getToken();

    int numHyphens = 0;

    while (tok.isSubtractionOperator()) {
        numHyphens++;
        tok = tokenizer.getToken();
    }
    Token prev = tok;
    tok = tokenizer.getToken();
    if (tok.isOpenBracket()) {
        if (numHyphens % 2 == 1) {
            Token mult = Token();
            mult.symbol('*');
            InfixExprNode *negVar = new InfixExprNode(mult);
            Token num = Token();
            num.setWholeNumber(-1);
            negVar->left() = new WholeNumber(num);
            negVar->right() = subscription(prev);
            return negVar;
        }
        return subscription(prev);
    }
    if (tok.isOpenParen()) {
        if (numHyphens % 2 == 1) {
            Token mult = Token();
            mult.symbol('*');
            InfixExprNode *negVar = new InfixExprNode(mult);
            Token num = Token();
            num.setWholeNumber(-1);
            negVar->left() = new WholeNumber(num);
            negVar->right() = call(prev);
            return negVar;
        }
        return call(prev);
    }
    tokenizer.ungetToken();
    return atom(prev, numHyphens);
}


ExprNode *Parser::atom(Token prev, int numHyphens) {
    // This function parses the grammar rules:

    // atom: ID | NUMBER | STRING+ | '(' test ')' 
    // ID: [_a-zA-Z][_a-zA-Z0-9]*
    // NUMBER: [0-9]+
    // STRING: [_a-zA-Z0-9]*

    Token tok = prev;

    if ( tok.isWholeNumber() ) {
        if (numHyphens % 2 == 1)
            tok.setWholeNumber(-(tok.getWholeNumber()));
        return new WholeNumber(tok);
    } else if( tok.isName() && !tok.isKeyword() ) {
        if (numHyphens % 2 == 1) {
            Token mult = Token();
            mult.symbol('*');
            InfixExprNode *negVar = new InfixExprNode(mult);
            Token num = Token();
            num.setWholeNumber(-1);
            negVar->left() = new WholeNumber(num);
            negVar->right() = new Variable(tok);
            return negVar;
        }
        return new Variable(tok);
    } else if( tok.isString() ) {
        if (numHyphens > 0)
            die("Parser::atom", "Bad operand type for unary -: 'str':", tok);
        return new String(tok);
    } else if ( tok.isOpenParen() ) {
        if (numHyphens % 2 == 1) {
            Token mult = Token();
            mult.symbol('*');
            InfixExprNode *negExpr = new InfixExprNode(mult);
            Token num = Token();
            num.setWholeNumber(-1);
            negExpr->left() = new WholeNumber(num);
            negExpr->right() = test();
            tok = tokenizer.getToken();
            if ( !tok.isCloseParen() )
                die ("Parser::atom", "Expected a close parenthesis, instead got", tok);
            return negExpr;
        }
        ExprNode *expr = test();

        tok = tokenizer.getToken();
        if ( !tok.isCloseParen() )
            die ("Parser::atom", "Expected a close parenthesis, instead got", tok);
        return expr;
    }

    die("Parser::atom", "Expected a whole number, name, double, string, or parenthesis, instead got", tok);

    return nullptr; // Will never reach here
}


// main program ( aka read the file and go line by line)

// Prints each token and the number of lines in the input file.
void identifyTokens(Tokenizer tokenizer) {
    Token tok = tokenizer.getToken();
    int numLines = 1;
    while (!tok.eof()) {
        if (tok.eol()) {
            cout << endl;
            numLines = numLines +1;
        }
        else
            tok.print();
        tok = tokenizer.getToken();
    }
    cout << "\nThe input has " << " occured at this line, "<< atLine + 1 << " lines.\n\n";
}


int main(int argc, const char *argv[]) {
	
    if (argc == 1 || argc > 3 || (argc == 2 && strcmp(argv[1], "-v") == 0)) {
        cout << "Usage: ./python.exe [-v] <path>\n";
		cout << "-v is optional, include it to enable verbose mode, mainly for debugging\n";
        exit(1);
	} else if (argc == 3) {
		
        cout << "Usage: ./python.exe [-v] <path>\n";
        cout << "-v is optional, include it to enable verbose mode, mainly for debugging\n";
        exit(1);
		
	}
	
    ifstream inputStream;

    inputStream.open(argv[1], ios::in);
    if (!inputStream.is_open()) {
        cout << "Unable to open " << argv[1] << ". Terminating...\n";
        perror("Error when attempting to open the input file");
        exit(2);
    }

	

    Tokenizer tokenizer(inputStream);
    Functions *funcs = new Functions();
    Parser parser(tokenizer, funcs);
    Statements *statements = parser.statements();
    Token lastToken = tokenizer.getToken();
    if (!lastToken.eof()) {
        cout << "Unexpected token in input:" << " occured at this line, "<< atLine + 1 << endl;
        lastToken.print();
        exit(1);
    }

    SymTab symTab;
	
    statements->evaluate(symTab);
	

    return 0;
}