#ifndef WC3LIB_JASS_LLVM_LLVM_HPP
#define WC3LIB_JASS_LLVM_LLVM_HPP
/**
 * The LLVM binding uses the visitor pattern to be separated from the jass module.
 * It creates Value objects for all AST nodes to generate LLVM's IR (http://llvm.org/docs/LangRef.html).
 * LLVM should be able to create real assembler code for all of it target architectures then.
 * This builds an abstract compiler implementation.
 * 
 * http://llvm.org/docs/tutorial/LangImpl3.html is used as reference example.
 */

//#include <cstdint> // required by LLVM

//#include <llvm/IR/Verifier.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/ADT/StringRef.h>

#include "../ast.hpp"

namespace wc3lib
{

namespace jass
{

llvm::Value* ErrorV(const char *Str) {
	// TODO store error
	//llvm::Error(Str);
	
	return 0;
}

llvm::Value* real_literal(llvm::LLVMContext &context, float32 literal) {
	return llvm::ConstantFP::get(context, llvm::APFloat(literal));
}

llvm::Value* expression(llvm::IRBuilder<> &builder, jass_expression &expression) {
	// TODO get on variant
	return 0;
}
	
llvm::Value* binary_operation(llvm::IRBuilder<> &builder, jass_binary_operation &operation) {
	llvm::Value *left = expression(builder, operation.first_expression);
	llvm::Value *right = expression(builder, operation.second_expression);
	bool isDouble = false;
	// TODO
	// check typeid is equal to ConstantFP
	
	switch (operation.op) {
		case jass_binary_operator::Plus: // TODO use FAdd when one real is there
			return builder.CreateAdd(left, right);
			
			break;
			
		case jass_binary_operator::Minus:
			return builder.CreateSub(left, right);
			
			break;
	}
	
	return 0;
}

llvm::Value* args(llvm::IRBuilder<> &builder, jass_function_args &args) {
	return 0; // TODO build
}

/**
 * Converts \ref jass_function_reference into its string represented identifier.
 */
struct function_ref_visitor : public boost::static_visitor<llvm::StringRef> {
	
	llvm::StringRef operator()(const string functionName) const {
		return functionName;
	}
	
	llvm::StringRef operator()(const jass_function_declaration *function) const {
		return function->identifier;
	}
};

llvm::Value* function_call(llvm::Module &module, llvm::IRBuilder<> &builder, jass_function_call &call) {
	// Look up the name in the global module table.
	llvm::Function *CalleeF = module.getFunction(boost::apply_visitor(function_ref_visitor(), call.function));
	
	if (CalleeF == 0) {
		return ErrorV("Unknown function referenced");
	}

	// If argument mismatch error.
	if (CalleeF->arg_size() != call.arguments.size()) {
		return ErrorV("Incorrect # arguments passed");
	}
  
	return builder.CreateCall(CalleeF, args(builder, call.arguments));
}
	
}

}

#endif
