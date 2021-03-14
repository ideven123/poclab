#ifndef _C1_SYNTAX_TREE_CHECKER_H_
#define _C1_SYNTAX_TREE_CHECKER_H_

#include "SyntaxTree.h"
#include "ErrorReporter.h"
#include <cassert>

class SyntaxTreeChecker : public SyntaxTree::Visitor
{
public:
    SyntaxTreeChecker(ErrorReporter &e) : err(e) {}
    virtual void visit(SyntaxTree::Assembly &node) override;
    virtual void visit(SyntaxTree::FuncDef &node) override;
    virtual void visit(SyntaxTree::BinaryExpr &node) override;
    virtual void visit(SyntaxTree::UnaryExpr &node) override;
    virtual void visit(SyntaxTree::LVal &node) override;
    virtual void visit(SyntaxTree::Literal &node) override;
    virtual void visit(SyntaxTree::ReturnStmt &node) override;
    virtual void visit(SyntaxTree::VarDef &node) override;
    virtual void visit(SyntaxTree::AssignStmt &node) override;
    virtual void visit(SyntaxTree::FuncCallStmt &node) override;
    virtual void visit(SyntaxTree::BlockStmt &node) override;
    virtual void visit(SyntaxTree::EmptyStmt &node) override;
private:
    using Type = SyntaxTree::Type;
    struct Variable
    { 
        bool is_const;
        Type btype;
        int int_val;
        float float_val;        
        std::vector<int> array_length;
        Variable() = default;
        Variable(bool is_const, Type btype,int int_val, float float_val , std::vector<int> array_length) 
            : is_const(is_const), btype(btype),int_val(int_val),float_val(float_val), array_length(array_length) {}
    };
    using PtrVariable = std::shared_ptr<Variable>;
    
    void enter_scope() { variables.emplace_front(); }

    void exit_scope() { variables.pop_front(); }

    PtrVariable lookup_variable(std::string name)
    {
        for (auto m : variables)
            if (m.count(name))
                return m[name];
        return nullptr;
    }

    bool declare_variable(std::string name, bool is_const, Type btype,int int_val ,float float_val, std::vector<int> array_length)
    {
        assert(!variables.empty());
        if (variables.front().count(name))
            return false;
        variables.front()[name] = PtrVariable(new Variable(is_const, btype,int_val ,float_val, array_length));
        return true;
    }

    bool declare_function(std::string name,Type btype){
    //    assert(!functions.empty());
        if(functions.find(name)==functions.end()){
             functions.insert({name , btype});
            return true;
        }
        else {
            return false;  
        }  
    }
    Type  lookup_function(std::string name){
        
         if(functions.find(name)==functions.end()){
             return  (Type)-1;
         }else
         {
             return functions[name];
         }
         
    }
   

    std::deque<std::unordered_map<std::string, PtrVariable>> variables;

    std::unordered_map<std::string, SyntaxTree::Type> functions;

    ErrorReporter &err;
};

#endif  // _C1_SYNTAX_TREE_CHECKER_H_
