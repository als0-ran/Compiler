#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include<fstream>
#include<cstring>
#include<koopa.h>
#include "ast.h"
using namespace std;

// 声明 lexer 的输入, 以及 parser 函数
// 为什么不引用 sysy.tab.hpp 呢? 因为首先里面没有 yyin 的定义
// 其次, 因为这个文件不是我们自己写的, 而是被 Bison 生成出来的
// 你的代码编辑器/IDE 很可能找不到这个文件, 然后会给你报错 (虽然编译不会出错)
// 看起来会很烦人, 于是干脆采用这种看起来 dirty 但实际很有效的手段
extern FILE *yyin;
extern int yyparse(unique_ptr<BaseAST> &ast);

// 函数声明略
void Visit(const koopa_raw_program_t &program);
void Visit(const koopa_raw_slice_t &slice);
void Visit(const koopa_raw_function_t &func);
void Visit(const koopa_raw_basic_block_t &bb);
void Visit(const koopa_raw_value_t &value);

	// 访问 raw program
	void Visit(const koopa_raw_program_t &program)
{
	// 执行一些其他的必要操作
	// ...
	// 访问所有全局变量
	Visit(program.values);
	// 访问所有函数
	Visit(program.funcs);
}

// 访问 raw slice
void Visit(const koopa_raw_slice_t &slice)
{
	for (size_t i = 0; i < slice.len; ++i)
	{
		auto ptr = slice.buffer[i];
		// 根据 slice 的 kind 决定将 ptr 视作何种元素
		switch (slice.kind)
		{
		case KOOPA_RSIK_FUNCTION:
			// 访问函数
			Visit(reinterpret_cast<koopa_raw_function_t>(ptr));
			break;
		case KOOPA_RSIK_BASIC_BLOCK:
			// 访问基本块
			Visit(reinterpret_cast<koopa_raw_basic_block_t>(ptr));
			break;
		case KOOPA_RSIK_VALUE:
			// 访问指令
			Visit(reinterpret_cast<koopa_raw_value_t>(ptr));
			break;
		default:
			// 我们暂时不会遇到其他内容, 于是不对其做任何处理
			assert(false);
		}
	}
}

// 访问函数
void Visit(const koopa_raw_function_t &func)
{
	// 执行一些其他的必要操作
	// ...
	// 访问所有基本块
	if (func->name[0] == '@')
	{
		// 这是一个全局函数
		cout << ".globl ";
		for (int i = 1; i < strlen(func->name); ++i)
			cout << func->name[i];
		cout << endl;
	}
	for (int i  = 1; i < strlen(func->name); ++i)
		cout<< func->name[i];
	
	cout << ":"<< endl;
	Visit(func->bbs);
}

// 访问基本块
void Visit(const koopa_raw_basic_block_t &bb)
{
	// 执行一些其他的必要操作
	// ...
	// 访问所有指令
	Visit(bb->insts);
}

// 访问指令
void Visit(const koopa_raw_value_t &value)
{
	// 根据指令类型判断后续需要如何访问
	const auto &kind = value->kind;
	switch (kind.tag)
	{
	case KOOPA_RVT_RETURN:
		// 访问 return 指令
		//Visit(kind.data.ret));
		Visit(kind.data.ret.value);
		cout << "ret" << endl;
		break;
	case KOOPA_RVT_INTEGER:
		// 访问 integer 指令
		// Visit(kind.data.integer);
		cout << "li\ta0,\t" << kind.data.integer.value << endl;
		
		break;
	default:
		// 其他类型暂时遇不到
		assert(false);
	}
}

// 访问对应类型指令的函数定义略
// 视需求自行实现
// ...

int main(int argc, const char *argv[]) {
  // 解析命令行参数. 测试脚本/评测平台要求你的编译器能接收如下参数:
  // compiler 模式 输入文件 -o 输出文件
  assert(argc == 5);
  auto mode = argv[1];
  auto input = argv[2];
  auto output = argv[4];
	//cout << input<<endl;
  // 打开输入文件, 并且指定 lexer 在解析的时候读取这个文件
  yyin = fopen(input, "r");
  assert(yyin);

          // 调用 parser 函数, parser 函数会进一步调用 lexer 解析输入文件的
  unique_ptr<BaseAST>    ast;
  auto ret = yyparse(ast);

  assert(!ret);

  if (strncmp(mode, "-koopa", 6)== 0 )
  {
	 // cout << mode<< endl;
    freopen(output, "w", stdout);
    ast->Dump();
  }

 //cout << endl;

	// ast->KoopaIR(output);
	// 输出解析得到的 AST, 其实就是个字符串
  if (strncmp(mode, "-riscv",6) == 0){
	string ir_str;
  	ast->KoopaIR(ir_str);
  	//cout << ir_str << endl;
	// 解析字符串 str, 得到 Koopa IR 程序
	koopa_program_t program;
	koopa_error_code_t ret = koopa_parse_from_string(ir_str.c_str(), &program);
	assert(ret == KOOPA_EC_SUCCESS); // 确保解析时没有出错
	// 创建一个 raw program builder, 用来构建 raw program
	koopa_raw_program_builder_t builder = koopa_new_raw_program_builder();
	// 将 Koopa IR 程序转换为 raw program
	koopa_raw_program_t raw = koopa_build_raw_program(builder, program);
	// 释放 Koopa IR 程序占用的内存
	koopa_delete_program(program);
	// 处理 raw program
	freopen(output, "w", stdout);
	cout << ".text" << endl;
	Visit(raw);
	/*
	for (size_t i = 0; i < raw.funcs.len; ++i)
	{
		// 正常情况下, 列表中的元素就是函数, 我们只不过是在确认这个事实
		// 当然, 你也可以基于 raw slice 的 kind, 实现一个通用的处理函数
		assert(raw.funcs.kind == KOOPA_RSIK_FUNCTION);
		// 获取当前函数
		koopa_raw_function_t func = (koopa_raw_function_t)raw.funcs.buffer[i];
		// 进一步处理当前函数
		for (size_t j = 0; j < func->bbs.len; ++j)
		{
			assert(func->bbs.kind == KOOPA_RSIK_BASIC_BLOCK);
			koopa_raw_basic_block_t bb = (koopa_raw_basic_block_t)func->bbs.buffer[j];
			// 进一步处理当前基本块
			// ...
			for (size_t k = 0; k < bb->insts.len; ++k)
			{
				//assert(bb->insts.kind == KOOPA_RSIK_INST);
				koopa_raw_value_t value = (koopa_raw_value_t)bb->insts.buffer[k];
				// 进一步处理当前指令
				// 示例程序中, 你得到的 value 一定是一条 return 指令
				assert(value->kind.tag == KOOPA_RVT_RETURN);
				// 于是我们可以按照处理 return 指令的方式处理这个 value
				// return 指令中, value 代表返回值
				koopa_raw_value_t ret_value = value->kind.data.ret.value;
				//cout << ret_value->kind.data.integer.value << endl;
				// 示例程序中, ret_value 一定是一个 integer
				assert(ret_value->kind.tag == KOOPA_RVT_INTEGER);
				// 于是我们可以按照处理 integer 的方式处理 ret_value
				// integer 中, value 代表整数的数值
				int32_t int_val = ret_value->kind.data.integer.value;
				// 示例程序中, 这个数值一定是 0
				assert(int_val == 0);
			
			}
		}
		// 处理完成, 释放 raw program builder 占用的内存
		// 注意, raw program 中所有的指针指向的内存均为 raw program builder 的内存
		// 所以不要在 raw program builder 处理完毕之前释放 builder
		koopa_delete_raw_program_builder(builder);
		}*/
		koopa_delete_raw_program_builder(builder);
	} // end for if
	
	return 0;
}


