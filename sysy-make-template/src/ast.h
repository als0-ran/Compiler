#ifndef AST_H
#define AST_H
#include<iostream>
#include<sstream>

// 所有 AST 的基类
class BaseAST
{
        public:
                virtual ~BaseAST() = default;
                virtual void Dump() const = 0;
                virtual void KoopaIR(std::string &ir_str)  = 0;
};

// CompUnit 是 BaseAST
class CompUnitAST : public BaseAST
{
        public:
                // 用智能指针管理对象
                std::unique_ptr<BaseAST> func_def;
                void Dump() const override
                {
                        //std::cout << "CompUnit : " << std::endl;
                        func_def->Dump();
                        //std::cout << " }";
                }
                void KoopaIR(std::string &ir_str ) override
                {
                        
                        func_def->KoopaIR(ir_str);
                }
                
};

// FuncDef 也是 BaseAST
class FuncDefAST : public BaseAST
{
        public:
                std::unique_ptr<BaseAST> func_type;
                std::string ident;
                std::unique_ptr<BaseAST> block;

                void Dump() const override
                {
                        std::cout << "fun ";
                        
                        std::cout << "@" << ident << "(): ";
                        func_type->Dump();
                        std::cout << "{"<<std::endl;
                        block->Dump();
                        std::cout<< std::endl;
                        std::cout << " }";
                }
                void KoopaIR(std::string &ir_str ) override
                {
                        std::string str;
                        str  = "fun @"+ ident +"():";
                        ir_str = str;
                        func_type->KoopaIR(ir_str);
                        str = "{\n";
                        ir_str += str;
                        block->KoopaIR( ir_str);
                        str = "\n}";
                        ir_str+=str;
                        
                }
};

class FunctypeAST: public BaseAST
{
        public:
                std::string type;
                void Dump() const override
                {
                        std::cout << type;
                }
                void KoopaIR(std::string &ir_str ) override
                {
                         std::string str;
                        str = type;
                        ir_str += str;
                }
};

class BlockAST:public BaseAST
{
        public:
                std::unique_ptr<BaseAST> stmt;
                void Dump() const override
                {
                        std::cout << "%entry:"<< std::endl;
                        stmt->Dump();
                        //std::cout << " }";
                }
                void KoopaIR(std::string &ir_str ) override
                {
                         std::string str;
                        str = "%entry:\n";
                        ir_str += str;
                        stmt->KoopaIR(ir_str);
                        //str = "\n}";
                        //ir_str+=str;
                }
};
class StmtAST:public BaseAST
{
        public:
                int number;
                void Dump() const override
                {
                        std::cout << " ret " << number ;
                }
                void KoopaIR(std::string &ir_str ) override
                {
                        ir_str += "ret " + std:: to_string(number);
                }
};


#endif