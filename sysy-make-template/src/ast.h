#ifndef AST_H
#define AST_H
#include<iostream>
#include<fstream>

// 所有 AST 的基类
class BaseAST
{
        public:
                virtual ~BaseAST() = default;
                virtual void Dump() const = 0;
                virtual void KoopaIR(const char* file)  = 0;
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
                void KoopaIR(const char *file) override
                {
                        std::ofstream outfile;
                        outfile.open(file, std::ios::app);
                        outfile << "define ";
                        func_def->KoopaIR(file);
                        outfile.close();

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
                void KoopaIR(const char *file) override
                {
                        std::ofstream outfile;
                        outfile.open(file,std::ios::app);
                        //outfile << "define ";
                        
                        outfile <<"fun"<< " @" << ident << "()";
                        func_type->KoopaIR(file);
                        outfile << "{"<<std::endl;                        
                        block->KoopaIR(file);
                        outfile << std::endl << "}";
                        outfile.close();
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
                void KoopaIR(const char *file) override
                {
                        std::ofstream outfile;
                        outfile.open(file,std::ios::app);
                        outfile << type;
                        outfile.close();
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
                void KoopaIR(const char *file) override
                {
                        std::ofstream outfile;
                        outfile.open(file,std::ios::app);
                        outfile << "%entry:"<< std::endl;
                        stmt->KoopaIR(file);
                        outfile.close();
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
                void KoopaIR(const char *file) override
                {
                        std::ofstream outfile;
                        outfile.open(file,std::ios::app);
                        outfile << "ret " << number;
                        outfile.close();
                }
};


#endif