#ifndef CLASSES_HPP
#define CLASSES_HPP

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <fstream>

using namespace std;

class TypeDescriptor {
public:
    // all posible types
    enum types {INTEGER,  STRING, ARRAY, NOT_SPECIFIED};
    TypeDescriptor(): _type{NOT_SPECIFIED} {}
    TypeDescriptor(types type): _type{type} {}
    types &type() { return _type; }
    virtual ~TypeDescriptor() {}
    virtual void print() = 0;
private:
    types _type;
};

class IntegerDescriptor : public TypeDescriptor {
public:
    IntegerDescriptor(int value): TypeDescriptor(INTEGER), _value(value) {}
    int value() { return _value; }
    void print() { cout << value(); }
private:
    int _value;
};

class StringDescriptor : public TypeDescriptor {
public:
    StringDescriptor(string value): TypeDescriptor(STRING), _value(value) {}
    string value() { return _value; }
    void print() { cout << value(); }
private:
    string _value;
};

class ArrayDescriptor : public TypeDescriptor {
public:
    ArrayDescriptor();
    ArrayDescriptor(vector<TypeDescriptor*> values, types elType);

    vector<TypeDescriptor*> value() const;
    vector<TypeDescriptor*> &value();
    types &elType();
    int numElements() const;
    int &numElements();
    void print();
private:
    types _elType;
    int _numElements;
    vector<TypeDescriptor*> _values;
};




class Token {

public:
    Token();

    bool &eof()  { return _eof; }
    bool &eol()  { return _eol; }

    bool eof() const { return _eof; }
    bool eol() const  { return _eol; }

    bool isOpenParen() const  { return _symbol == '('; }
    bool isCloseParen() const { return _symbol == ')'; }

    bool isOpenBracket() const { return _symbol == '['; }
    bool isCloseBracket() const { return _symbol == ']'; }

    void symbol(char c) { _symbol = c; }
    char symbol() { return _symbol; }

    void op(string s) { _op = s; }
    string op() { return _op; }

    bool isAssignmentOperator() const     { return _symbol == '='; }
    bool isMultiplicationOperator() const { return _symbol == '*'; }
    bool isAdditionOperator() const       { return _symbol == '+'; }
    bool isSubtractionOperator() const    { return _symbol == '-'; }
    bool isArithmeticOperator() const {
        return isMultiplicationOperator() 
        || isAdditionOperator() 
        || isSubtractionOperator();
        }
    bool isGreaterThanOperator() const          { return _symbol == '>'; }
    bool isGreaterThanOrEqualToOperator() const { return _op == ">="; }
    bool isLessThanOperator() const             { return _symbol == '<'; }
    bool isLessThanOrEqualToOperator() const    { return _op == "<="; }
    bool isEqualityOperator() const             { return _op == "=="; }
    bool isInequalityOperator() const           { return _op == "!="; }
    bool isRelationalOperator() const {
        return isGreaterThanOperator() 
        || isGreaterThanOrEqualToOperator() 
        || isLessThanOperator() 
        || isLessThanOrEqualToOperator() 
        || isEqualityOperator() 
        || isInequalityOperator();
    }

    bool isOrOperator() const             { return _op == "or"; }
    bool isAndOperator() const            { return _op == "and"; }
    bool isNotOperator() const            { return _op == "not"; }
    bool isBooleanOperator() const {
        return isOrOperator() ||
               isAndOperator() ||
               isNotOperator();
    }

    bool isComma() const                  { return _symbol == ','; }
    bool isColon() const                  { return _symbol == ':'; }
    bool isName() const                   { return _name.length() > 0; }

    string getName() const                  { return _name; }
    void setName(string n) { _name = n; }

    bool isKeywordPrint() const  { return _name == "print"; }
    bool isKeywordIf() const     { return _name == "if"; }
    bool isKeywordElse() const   { return _name == "else"; }
    bool isKeywordElif() const   { return _name == "elif"; }
    bool isKeywordReturn() const { return _name == "return"; }
    bool isKeywordDef() const    { return _name == "def"; }
    bool isKeyword() const {
        return isKeywordPrint() 
        || isKeywordIf() 
        || isKeywordElse() 
        || isKeywordElif() 
        || isKeywordReturn() 
        || isKeywordDef() ;
    }

    bool &isWholeNumber() { return _isWholeNumber; }
    bool isWholeNumber() const { return _isWholeNumber; }
    int getWholeNumber() const { return _wholeNumber; }
    void setWholeNumber(int n) {
        _wholeNumber = n;
        isWholeNumber() = true;
    }

 
    bool &isString() { return _isString; }
    bool isString() const { return _isString; }
    string getString() const { return _str; }
    void setString(string s) {
        _str = s;
        isString() = true;
    }

    bool &isCmnt() { return _isCmnt; }
    bool isCmnt() const { return _isCmnt; }
    string getCmnt() const { return _cmnt; }
    void setCmnt(string s) {
        _cmnt = s;
        isCmnt() = true;
    }

    bool &indent()  { return _indent; }
    bool indent() const { return _indent; }

    bool &dedent()  { return _dedent; }
    bool dedent() const { return _dedent; }

    void print() const;

private:
    string _name;
    bool _eof, _eol;
    bool _isWholeNumber;
    bool _isString;
    bool _isCmnt;
    char _symbol;
    string _op;
    int _wholeNumber;
    string _str;
    string _cmnt;
    bool _indent;
    bool _dedent;
};




class Tokenizer {

public:
    Tokenizer(ifstream &inStream);
    Token getToken();
    void ungetToken();
    void printProcessedTokens();

private:
    Token lastToken;
    bool ungottenToken;
    ifstream &inStream;
    vector<Token> _tokens;
    vector<int> indentStack;
    bool parsingANewLine;

private:
    string readName();
    int readInteger();
    double readDouble(int left);
    string readString(char end);
};



// // Symbokl tables  




// This is a flat symbol table. It allows for variables to be
// initialized, determines if a give variable has been defined or not, and if
// a variable has been defined, it returns its value.

class SymTab {
public:
    SymTab();

    bool isDefined(string vName);

    void setValueFor(string vName, TypeDescriptor* td);
    TypeDescriptor* getValueFor(string vName);

    void openScope(map<string, TypeDescriptor*> newSymTab);
    void storeReturnValue(TypeDescriptor *val);
    TypeDescriptor* getReturnValue();
    void closeScope();

    int getI() { return i; }

    void print();

private:
    vector<map<string, TypeDescriptor*> > symTabs;
    int i; // current SymTab depth
    TypeDescriptor* returnVal;

};


// // tree generation 



class Functions;

// Classes in this file define the internal representation of expressions.


// An ExprNode serves as the base class (super class) for an expression.
// It forces the derived classes (subclasses) to implement two functions, print and
// evaluate.
class ExprNode {
public:
    ExprNode(Token token);
    Token token() const;
    Token &token() { return _token; }
    virtual void print() = 0;
    virtual TypeDescriptor* evaluate(SymTab &symTab) = 0;

private:
    Token _token;
};


// An InfixExprNode is useful to represent binary arithmetic operators.
class InfixExprNode: public ExprNode {  // An expression tree node.

public:
    InfixExprNode(Token tk);

    ExprNode *&left();
    ExprNode *&right();
    virtual void print();
    virtual TypeDescriptor* evaluate(SymTab &symTab);

private:
    ExprNode *_left, *_right;
};


// A RelExprNode is useful to represent binary relational operators.
class RelExprNode: public ExprNode {  // An expression tree node.

public:
    RelExprNode(Token tk);

    ExprNode *&left();
    ExprNode *&right();
    virtual void print();
    virtual TypeDescriptor* evaluate(SymTab &symTab);

private:
    ExprNode *_left, *_right;
};

// A BoolExprNode is useful to represent binary boolean operators.
class BoolExprNode: public ExprNode {  // An expression tree node.

public:
    BoolExprNode(Token tk);

    ExprNode *&left();
    ExprNode *&right();
    virtual void print();
    virtual TypeDescriptor* evaluate(SymTab &symTab);

private:
    ExprNode *_left, *_right;
};

// A NotExprNode is useful to represent the unary 'not' operator.
class NotExprNode: public ExprNode {  // An expression tree node.

public:
    NotExprNode(Token tk);

    ExprNode *&right();
    virtual void print();
    virtual TypeDescriptor* evaluate(SymTab &symTab);

private:
    ExprNode *_right;
};

// A Subscription is useful to represent a subscription into an array
class Subscription: public ExprNode {
public:
    Subscription(Token varName, ExprNode* subscript);

    ExprNode *&subscript();
    virtual void print();
    virtual TypeDescriptor* evaluate(SymTab &symTab);

private:
    ExprNode *_subscript;
};

// A Len is useful to represent a length call on an array
class Len: public ExprNode {
public:
    Len(Token varName);
    virtual void print();
    virtual TypeDescriptor* evaluate(SymTab &symTab);
};

class Call: public ExprNode {
public:
    Call(Token funcName, Functions* funcList, vector<ExprNode*> args);

    vector<ExprNode*> &args();
    virtual void print();
    virtual TypeDescriptor* evaluate(SymTab &symTab);
private:
    vector<ExprNode*> _args;
    Functions* _funcList;
};

// WholeNumber is a leaf-node in an expression tree. It corresponds to
// a terminal in the production rules of the grammar that describes the
// syntax of expressions.

class WholeNumber: public ExprNode {
public:
    WholeNumber(Token token);
    virtual void print();
    virtual TypeDescriptor* evaluate(SymTab &symTab);
};

// Variable is a leaf-node in an expression tree. It corresponds to
// a terminal in the production rules of the grammar that describes the
// syntax of expressions.

class Variable: public ExprNode {
public:
    Variable(Token token);
    virtual void print();
    virtual TypeDescriptor* evaluate(SymTab &symTab);
};


// String is a leaf-node in an expression tree. It corresponds to
// a terminal in the production rules of the grammar that describes the
// syntax of expressions.

class String: public ExprNode {
public:
    String(Token token);
    virtual void print();
    virtual TypeDescriptor* evaluate(SymTab &symTab);
};



// The Statement (abstract) class serves as a super class for all statements that
// are defined in the language. Ultimately, statements have to be evaluated.
// Therefore, this class defines evaluate, a pure-virtual function to make
// sure that all subclasses of Statement provide an implementation for this
// function.

class Statement {
public:
    Statement();

    virtual void print() = 0;
    virtual void evaluate(SymTab &symTab) = 0;

};


// Statements is a collection of Statement. For example, all statements in a function
// can be represented by an instance of Statements.

class Statements {
public:
    Statements();

    void addStatement(Statement *statement);
    void evaluate(SymTab &symTab);

    void print();

private:
    vector<Statement *> _statements;
};

// AssignmentStatement represents the notion of an lValue having been assigned an rValue.
// The rValue is an expression.

class AssignmentStatement : public Statement {
public:
    AssignmentStatement();
    AssignmentStatement(ExprNode *lhsExpr, ExprNode *rhsExpr);
    AssignmentStatement(ExprNode *lhsExpr, vector<ExprNode*> rhsArray);

    ExprNode *&lhsExpression();
    ExprNode *&rhsExpression();
    vector<ExprNode*> rhsArray();

    virtual void evaluate(SymTab &symTab);
    virtual void print();

private:
    ExprNode *_lhsExpression;
    ExprNode *_rhsExpression;
    vector<ExprNode*> _rhsArray;
};

class PrintStatement : public Statement {
public:
    PrintStatement();
    PrintStatement(vector<ExprNode*> list);

    vector<ExprNode*> &list();

    virtual void evaluate(SymTab &symTab);
    virtual void print();

private:
    vector<ExprNode*> _list;
};


class IfStatement : public Statement {
public:
    IfStatement();
    IfStatement(vector<ExprNode*> conditions, vector<Statements*> bodies);

    vector<ExprNode*> &conditions();
    vector<Statements*> &bodies();

    virtual void evaluate(SymTab &symTab);
    virtual void print();

private:
    vector<ExprNode*> _conditions;
    vector<Statements*> _bodies;
};

class ArrayOpStatement : public Statement {
public:
    ArrayOpStatement();
    ArrayOpStatement(Token varName, ExprNode* arg);

    Token &varName();
    ExprNode *&arg();

    virtual void evaluate(SymTab &symTab);
    virtual void print();
private:
    Token _varName;
    ExprNode* _arg;
};

class CallStatement : public Statement {
public:
    CallStatement(Token funcName, vector<string> params, vector<ExprNode*> args, Statements* body);

    vector<ExprNode*> &args();
    vector<string> &params();
    Statements *&body();

    virtual void evaluate(SymTab &symTab);
    virtual void print();
private:
    Token _funcName;
    vector<string> _params;
    vector<ExprNode*> _args;
    Statements* _body;
};

class ReturnStatement : public Statement {
public:
    ReturnStatement(ExprNode* retVal);

    ExprNode *&retVal();

    virtual void evaluate(SymTab &symTab);
    virtual void print();
private:
    ExprNode *_retVal;
};

class Function {
public:
    Function();
    Function(string name, vector<string> args, Statements *body)
            : _name{name}, _args{args}, _body{body} {}

    string &name() { return _name; }
    vector<string> &args() { return _args; }
    Statements *&body() {return _body; }

private:
    string _name;
    vector<string> _args;
    Statements *_body;
};


// Functions is a collection of Function.

class Functions {
public:
    Functions() : _functions{vector<Function *>()} {}

    vector<Function*> &functions() {return _functions; }

    void addFunction(Function *func);
    Function *findFunction(string name);

private:
    vector<Function *> _functions;
};




class Parser {
public:
    Parser(Tokenizer &tokenizer, Functions *&_funcs) : tokenizer{tokenizer}, funcs{_funcs} {}

    Statements *statements();

    // Simple statements
    AssignmentStatement *assignStatement(Token varName);
    PrintStatement *printStatement();
    ArrayOpStatement *arrayOp(Token varName);
    CallStatement *callStatement(Token varName);
    ExprNode *call(Token varName);
    ReturnStatement *returnStatement();

    // Compound statements
    IfStatement *ifStatement();
    Function *funcDef();

    Subscription *subscription(Token varName);
    vector<ExprNode*> array_init();

    Statements *suite();
    vector<ExprNode*> testlist();


    vector<string> parameter_list();

    ExprNode* array_len();

    ExprNode *test();
    ExprNode *or_test();
    ExprNode *and_test();
    ExprNode *not_test();
    ExprNode *comparison();

    ExprNode *arithExpr();
    ExprNode *term();
    ExprNode *factor();
    ExprNode *atom(Token prev, int numHyphens);

private:
    Tokenizer &tokenizer;
    Functions *&funcs;

    void die(string where, string message, Token &token);

};

#endif 