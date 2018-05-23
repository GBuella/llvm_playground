/* vim: set filetype=cpp : */
/* vim: set noet tw=100 ts=8 sw=8 cinoptions=+4,(0,t0: */

/*
 * Chapter 3
 *
 * To execute:
 *
 * $ ./toy1 toy1.ll
 * $ llc toy1.ll
 * $ as toy1.s -o toy1.o
 * $ c99 src/LLVMEssentials1_main.c toy1.o
 * $ ./a.out alma
 */

#include <cassert>
#include <string>
#include <iostream>
#include <system_error>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"

using llvm::Type;

static llvm::LLVMContext Context;

static llvm::Module *ModuleOb = new llvm::Module("dummy1 module", Context);

static void print(const char *path)
{
	std::error_code EC;
	llvm::raw_fd_ostream stream(path, EC, llvm::sys::fs::F_Text);

	if (!EC)
		ModuleOb->print(stream, nullptr);
	else
		std::cerr << EC.message() << "\n";
}

int main(int argc, char **argv)
{
	const char *path = "/dev/stdout";
	llvm::IRBuilder<> Builder(Context);

	if (argc > 1)
		path = argv[1];

	auto Int32Ty = Builder.getInt32Ty();
	Type *VT = llvm::VectorType::get(Int32Ty, 2);
	llvm::FunctionType *FT = llvm::FunctionType::get(Int32Ty, VT->getPointerTo(0), false);
	auto Foo = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "foo", ModuleOb);
	Foo->arg_begin()->setName("x");
	
	Builder.SetInsertPoint(llvm::BasicBlock::Create(Context, "entry", Foo));
	Builder.CreateGEP(Int32Ty, Foo->arg_begin(), Builder.getInt32(1), "y");

	print(path);
}
