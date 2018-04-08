/* vim: set filetype=cpp : */
/* vim: set noet tw=100 ts=8 sw=8 cinoptions=+4,(0,t0: */

#include <cassert>
#include <string>
#include <iostream>
#include <system_error>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

#include "llvm/IR/Verifier.h"

// llvm streams, FS stuff
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"

/*
 * LLVM Essentials refers to a function getGlobalContext, but
 * that function no longer exists in the C++ API
 * http://reviews.llvm.org/D19094
 */
static llvm::LLVMContext Context;

static llvm::Module *ModuleOb = new llvm::Module("dummy module", Context);

/*
 * LLVM Essentials uses the ModuleOb->dump() method, but that just
 * doesn't seem to work.
 */
static void print(const char *path)
{
	std::error_code EC;
	llvm::raw_fd_ostream stream(path, EC, llvm::sys::fs::F_Text);

	if (!EC)
		ModuleOb->print(stream, nullptr);
	else
		std::cerr << EC.message() << "\n";
}

static llvm::Function *emitFunction(llvm::IRBuilder<> &Builder,
				    std::vector<llvm::Type*> ArgTypes,
				    std::vector<std::string> ArgNames,
				    std::string Name)
{
	assert(ArgTypes.size() >= ArgNames.size());

	auto funcType = llvm::FunctionType::get(Builder.getInt32Ty(), ArgTypes, false);
	auto Func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, Name, ModuleOb);
	llvm::Function::arg_iterator Arg = Func->arg_begin();
	for (auto& ArgName : ArgNames) {
		Arg->setName(ArgName);
		++Arg;
	}
	return Func;
}

static llvm::Function *emitFunction(llvm::IRBuilder<> &Builder, std::string Name)
{
	return emitFunction(Builder, {}, {}, Name);
}

int main(int argc, char **argv)
{
	const char *path = "/dev/stdout";
	llvm::IRBuilder<> Builder(Context);

	if (argc > 1)
		path = argv[1];

	auto Foo = emitFunction(Builder, "foo");
	auto Entry = llvm::BasicBlock::Create(Context, "entry", Foo);
	Builder.SetInsertPoint(Entry);

	(void) ModuleOb->getOrInsertGlobal("somevar", Builder.getInt32Ty());
	auto SomeVar = ModuleOb->getNamedGlobal("somevar");
	SomeVar->setAlignment(4);
	Builder.CreateRet(Builder.getInt32(0));

	auto Bar = emitFunction(Builder,
				{llvm::Type::getInt32Ty(Context),
				 llvm::Type::getInt64Ty(Context),
				 llvm::Type::getInt32Ty(Context),
				 llvm::Type::getInt16Ty(Context)},
				{"first", "second", "third"},
				"bar");

	auto BarEntry = llvm::BasicBlock::Create(Context, "entry", Bar);
	Builder.SetInsertPoint(BarEntry);

	auto first = Bar->arg_begin();
	auto third = first + 2;
	auto mulresult = Builder.CreateMul(first, third, "mulresult");

	auto Cmp100 = Builder.CreateICmpULT(mulresult, Builder.getInt32(123), "cmp100");
	auto ThenBB = llvm::BasicBlock::Create(Context, "then", Bar);
	auto ElseBB = llvm::BasicBlock::Create(Context, "else", Bar);
	auto IfresultBB = llvm::BasicBlock::Create(Context, "ifresult", Bar);
	Builder.CreateCondBr(Cmp100, ThenBB, ElseBB);
	Builder.SetInsertPoint(ThenBB);
	auto ThenValue = Builder.CreateSub(mulresult, Builder.getInt32(3), "thenvalue");
	Builder.CreateBr(IfresultBB);
	Builder.SetInsertPoint(ElseBB);
	auto ElseValue = Builder.CreateAdd(mulresult, Builder.getInt32(7), "elsevalue");
	Builder.CreateBr(IfresultBB);
	Builder.SetInsertPoint(IfresultBB);
	auto PHI = Builder.CreatePHI(llvm::Type::getInt32Ty(Context), 2, "result");
	PHI->addIncoming(ThenValue, ThenBB);
	PHI->addIncoming(ElseValue, ElseBB);

	Builder.CreateRet(PHI);

	llvm::verifyFunction(*Bar);

	print(path);
}
