#define DEBUG_TYPE "functionCloner"
#include "llvm/Bitcode/BitcodeReader.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Transforms/Scalar.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/ADT/SmallBitVector.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Pass.h"
#include "llvm/Support/Debug.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/ValueMapper.h"
#include "llvm/IR/IRBuilder.h"

using namespace llvm;

struct functionCloner : public ModulePass {
  static char ID;
  functionCloner() : ModulePass(ID) {}
  bool runOnModule(Module &M);
  Function *cloneFunctionWithExtraArgument(Function *F);

  void clone();
  void convertCall();
  void defuse_helper(Value *, int, int );

  virtual void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesCFG();
  }

private:
  Module *Mod;
  LLVMContext *ctx;
  PointerType *ptr_to_int8_type;
  IntegerType *int8_type;

  DenseMap<llvm::Function *, llvm::Function*> cloneMap;

  // const llvm::DataLayout *DL;
};

char functionCloner::ID = 0;
static RegisterPass<functionCloner> X("func-clone", "Function Cloner ",
                             false /* does not modify the CFG */,
                             true /* transformation, not just analysis */);
/*******************************************************************
* Function : runOnModule
* Purpose : Entry point for Test
********************************************************************/
bool functionCloner::runOnModule(Module &M) {
  Mod = &M;
  ctx = &M.getContext();
  ptr_to_int8_type = Type::getInt8PtrTy(*ctx);
  int8_type = Type::getInt8Ty(*ctx);

  clone();
  convertCall();
  return true;
}

Function *functionCloner::cloneFunctionWithExtraArgument(Function *F) {
  std::vector<Type *> ArgTypes;
  ValueToValueMapTy VMap;

  for (Function::const_arg_iterator I = F->arg_begin(), E = F->arg_end();
       I != E; ++I) {
    ArgTypes.push_back(I->getType());
  }

  // extra argument
  ArgTypes.push_back(ptr_to_int8_type);
  ArgTypes.push_back(ptr_to_int8_type);
  ArgTypes.push_back(ptr_to_int8_type);

  // Create a new function type considering the extra argument
  FunctionType *FTy =
      FunctionType::get(F->getFunctionType()->getReturnType(), ArgTypes,
                        F->getFunctionType()->isVarArg());

  // Create the new function...
  Function *NewF = Function::Create(FTy, F->getLinkage(), F->getName());

  // Loop over the arguments, copying the names of the mapped arguments over...
  Function::arg_iterator DestI = NewF->arg_begin();
  for (Function::const_arg_iterator I = F->arg_begin(), E = F->arg_end();
       I != E; ++I) {
    DestI->setName(I->getName());
    VMap[&*I] = &*DestI;
    DestI++;
  }
  DestI->setName("_parent_stack_start_ptr_");
  DestI++;
  DestI->setName("_parent_stack_end_ptr_");
  DestI++;
  DestI->setName("_parent_stack_rbp_ptr_");
  DestI++;

  SmallVector<ReturnInst *, 8> Returns; // Ignore returns cloned.
  CloneFunctionInto(NewF, F, VMap, false, Returns, "");

  Mod->getFunctionList().push_back(NewF);

  return NewF;
}

void functionCloner::defuse_helper(Value *W, int level, int tab) {
  if(0 == level) {
    return ;
  }

  llvm::errs() << *W ;
  for(User *U : W->users()) {
      errs() << *U << "\n"; 
    if (Instruction *I = dyn_cast<Instruction>(U)) {
      for(int k = 0 ; k < tab; k++) {
        errs() << "\t";
      }
      errs() << *I << "\n"; 
      defuse_helper(I, level-1, tab+1);
    }
  }
}


void functionCloner::clone() {

  // Exp1: clon eeach function and replace its uses with new function
  std::vector<Function*> FunctionsToClone;
  for (Module::iterator FuncI = Mod->begin(), FuncE = Mod->end();
       FuncI != FuncE; ++FuncI) {
    Function *Func =  &*FuncI;
    FunctionsToClone.push_back(Func);
  }

  for(auto *oldFunc : FunctionsToClone) {
    if(oldFunc->isIntrinsic() || oldFunc->isDeclaration()) {
      continue;
    }
    StringRef name  =  oldFunc->getName();
    llvm::errs() << "Cloning function : " << name  << "\n";
    if(oldFunc->getName() == "main") {
      cloneMap[oldFunc] = oldFunc;
      continue;
    }
    Function *newFunc = cloneFunctionWithExtraArgument(oldFunc);
    
    cloneMap[oldFunc] = newFunc;

    for (auto UseIter = oldFunc->use_begin(), E = oldFunc->use_end();
       E != UseIter;) {
      Use &FctUse = *(UseIter++);
      User *user = FctUse.getUser();
      //Value* value = FctUse.get();

      if(isa<CallInst>(user)) {
        llvm::errs() << "\tCallInst User:" << *user << "\n";
      } else {
        //if(Constant *gv = dyn_cast<Constant>(user)) {
        if(isa<Constant>(user)) {
          Constant* const_ptr = ConstantExpr::getCast(Instruction::BitCast, newFunc, oldFunc->getType()); 
          llvm::errs() << "Const Init : " << *const_ptr << "\n";
          oldFunc->replaceAllUsesWith(const_ptr);  
        } else {
          assert(0 && "Unhandled");
        }
      }
    }
  }
}

void functionCloner::convertCall() {
  for(auto P : cloneMap) {
    auto *Func = P.second;
    llvm::errs() << "\n\nProcessing func: " << Func->getName() << "\n";
    for (Function::iterator BBI = Func->begin(), BBE = Func->end(); BBI != BBE;
         ++BBI) {
      BasicBlock *BB = &*BBI;
      for (BasicBlock::iterator II = BB->begin(), EI = BB->end(); II != EI;
           ) {
        Instruction *I = &*II;
        ++II;
        if(CallInst* ci =  dyn_cast<CallInst>(I)) {
          IRBuilder<> IRB(ci);

          Value *f  = ci->getCalledValue();
          Function *oldFunc = dyn_cast<Function>(f);
          Function *newFunc =  NULL;
          CallInst *new_ci;
          llvm::errs() << "Called Value: " << *f << "\n";
          if(nullptr == oldFunc) {
            Value *calledV = f;
            Type *type = calledV->getType();
            FunctionType *funcTy = dyn_cast<FunctionType>(type->getPointerElementType());
            assert(funcTy != NULL && "CHAECK");

            llvm::errs() << "Called Value Type: " << *type << " Function Type:" << *funcTy << "\n";

            std::vector<Type *> ArgTypes;
            FunctionType::param_iterator PI =  funcTy->param_begin();
            FunctionType::param_iterator PE =  funcTy->param_end();
            for(;PI != PE ; PI++) {
              ArgTypes.push_back(*PI);
            }
            ArgTypes.push_back(ptr_to_int8_type);
            ArgTypes.push_back(ptr_to_int8_type);
            ArgTypes.push_back(ptr_to_int8_type);

            FunctionType *newTy = FunctionType::get(ci->getType() , ArgTypes, false);
            auto *bcast = IRB.CreateBitCast(calledV, newTy->getPointerTo());
            llvm::errs() << "Created bcast: " << *bcast << "\n";

            std::vector<Value *> arguments;
            for (auto &args : ci->arg_operands()) {
              arguments.push_back(args);
            }
            auto *x = IRB.CreateAlloca(int8_type, nullptr, "a");
            auto *y = IRB.CreateAlloca(int8_type, nullptr, "b");
            auto *z = IRB.CreateAlloca(int8_type, nullptr, "c");
            arguments.push_back(x);
            arguments.push_back(y);
            arguments.push_back(z);
            new_ci = IRB.CreateCall(bcast, arguments);

          } else {
            if(0 == cloneMap.count(oldFunc)) {
              continue;
            }
            newFunc =  cloneMap[oldFunc];
            llvm::errs() << "Cloning call: " << *ci << "\n";

            std::vector<Value *> arguments;
            for (auto &args : ci->arg_operands()) {
              arguments.push_back(args);
            }
            auto *x = IRB.CreateAlloca(int8_type, nullptr, "a");
            auto *y = IRB.CreateAlloca(int8_type, nullptr, "b");
            auto *z = IRB.CreateAlloca(int8_type, nullptr, "c");
            arguments.push_back(x);
            arguments.push_back(y);
            arguments.push_back(z);
            new_ci = IRB.CreateCall(newFunc, arguments);
          }
          llvm::errs() << "Created call: " << *new_ci << "\n";
          ci->replaceAllUsesWith(new_ci);
          //ci->dropAllReferences();
          ci->eraseFromParent();
        }
      }
    }
  }
  return;
}
