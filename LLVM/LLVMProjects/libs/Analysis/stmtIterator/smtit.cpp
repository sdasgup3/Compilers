//===-------------- Test.cpp test Code -------===//
//
// The LLVM Compiler Infrastructure
//
// This file was developed by the LLVM research group and is distributed under
// the University of Illinois Open Source License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//
//#define LLVM_VERSION_MAJOR 3
//#define LLVM_VERSION_MINOR 4
//#define LLVM_VERSION(major,minor) (((major) << 2) | (minor))
//#define LLVM_VERSION_CODE LLVM_VERSION(LLVM_VERSION_MAJOR,LLVM_VERSION_MINOR)

#define DEBUG_TYPE "smtit"
//#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Transforms/Scalar.h"
//#include "llvm/PassManager.h"
//#include "llvm/Target/TargetLibraryInfo.h"

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
using namespace llvm;

struct smtit : public ModulePass {
  static char ID;
  smtit() : ModulePass(ID) {}
  bool runOnModule(Module &M);

  void performTest1();
  void performTest2();
  void performTest3(Module &M);
  void defuse();
  bool isLLVMPAPtrTy(llvm::Type *);

  virtual void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesCFG();
  }

private:
  Module *Mod;
  // const llvm::DataLayout *DL;
};

char smtit::ID = 0;
static RegisterPass<smtit> X("smtit", "DSAND Testing Code ",
                             false /* does not modify the CFG */,
                             false /* transformation, not just analysis */);

/*******************************************************************
* Function : runOnModule
* Purpose : Entry point for Test
********************************************************************/
SwitchInst *GSI;

bool smtit::runOnModule(Module &M) {
  Mod = &M;
  // DL = Mod->getDataLayout();
  // DEBUG(errs() << "Data Layout String\n" << Mod->getDataLayoutStr() << "\n");

  bool Changed = false;

  performTest1();
  // performTest2();
  // llvm::errs() << "Performing  ITREAD\n";
  // performTest3( M);
  //defuse();


  return Changed;
}


void defuse_helper(Instruction *W, int level, int tab) {
  if(0 == level) {
    return ;
  }

  for(User *U : W->users()) {
    if (Instruction *I = dyn_cast<Instruction>(U)) {
      for(int k = 0 ; k < tab; k++) {
        errs() << "\t";
      }
      errs() << *I << "\n"; 
      defuse_helper(I, level-1, tab+1);
    }
  }
}

void smtit::defuse() {

  std::vector<LoadInst*> worklist;
  for (Module::iterator FuncI = Mod->begin(), FuncE = Mod->end();
       FuncI != FuncE; ++FuncI) {
    Function *Func = &*FuncI;
    for (Function::iterator BBI = Func->begin(), BBE = Func->end(); BBI != BBE;
         ++BBI) {
      BasicBlock *BB = &*BBI;
      for (BasicBlock::iterator II = BB->begin(), EI = BB->end(); II != EI;
           ++II) {
        Instruction *I = &*II;
        if(LoadInst* li =  dyn_cast<LoadInst>(I)) {
          Value *ptr_operand = li->getPointerOperand();
          if (ptr_operand->getName().equals("RSP_val")) {
            worklist.push_back(li);
          }
        }
      }
    }
  }

  for(auto li: worklist) {
    errs() << "========= Def Use List =========\n";
    errs() << *li << "\n";
    defuse_helper(li, 5, 1);
  }
}

void smtit::performTest3(Module &mainModule) {
  /*
  OwningPtr<MemoryBuffer> BufferPtr;
  const char* file = "test.bc";
  std::string ErrorMsg;
  error_code ec=MemoryBuffer::getFileOrSTDIN(file, BufferPtr);
  if (ec) {
    llvm::errs() << "error loading program " << file << " " <<
  ec.message().c_str();
  }
  Module* mainModule = getLazyBitcodeModule(BufferPtr.get(), getGlobalContext(),
  &ErrorMsg);
  if (mainModule) {
    if (mainModule->MaterializeAllPermanently(&ErrorMsg)) {
      delete mainModule;
      mainModule = 0;
    }
  }
  */

  // assert(mainModule);
  // fixit
  /*
  llvm::errs() << mainModule ;

  PassManager passes;
  llvm::DominatorTree *DT = new llvm::DominatorTree();
  passes.add(DT);

  passes.run(mainModule);

  if (DT->getRoot())
    llvm::errs() << DT->getRoot();
*/
}

void smtit::performTest2() {
  for (Module::iterator FuncI = Mod->begin(), FuncE = Mod->end();
       FuncI != FuncE; ++FuncI) {
    Function *Func = &*FuncI;

    for (Function::iterator BBI = Func->begin(), BBE = Func->end(); BBI != BBE;
         ++BBI) {
      BasicBlock *BB = &*BBI;
      for (BasicBlock::iterator II = BB->begin(), EI = BB->end(); II != EI;
           ++II) {
        Instruction *I = &*II;
        if (I->getOpcode() == Instruction::Call) {
          DEBUG(errs() << "\n\n\nThis is Call Instrcution  \n" << *I << "\n");
          CallInst *CallI = cast<CallInst>(I);
          Function *fp = CallI->getCalledFunction();
          DEBUG(errs() << "Called Function " << fp->getName() << "\n");
          DEBUG(errs() << "Used in\n");

          for (Value::use_iterator i = I->use_begin(), e = I->use_end(); i != e;
               ++i) {
            Instruction *UseI = dyn_cast<Instruction>(*i);
            if (isa<SwitchInst>(UseI)) {
              SwitchInst *SI = cast<SwitchInst>(UseI);
              if (fp->getName() == "F") {
                GSI = SI;
                DEBUG(errs() << *UseI << "\n");
              } else if (fp->getName() == "G") {
                if (SI != GSI) {
                  DEBUG(errs() << "Different Swict"
                               << "\n");
                }
              }
            }
          }
        }
      }
    }
  }
}

// for (Value::use_iterator i = F->use_begin(), e = F->use_end(); i != e; ++i)
//  if (Instruction *Inst = dyn_cast<Instruction>(*i)) {
//    errs() << "F is used in instruction:\n";
//    errs() << *Inst << "\n";
//  }
//

bool smtit::isLLVMPAPtrTy(Type *Ty) {
  // Type *IntPtrTy = DL->getIntPtrType(Ty->getContext());
  // DEBUG(errs() << "\tType: " << *IntPtrTy << " Arg Type"<< *Ty  <<" \n");
  // return Ty->isPointerTy() || Ty == IntPtrTy;
  return 0;
}

void smtit::performTest1() {

  for (Module::iterator FI = Mod->begin(), FE = Mod->end(); FI != FE; ++FI) {
    Function *Func = &*FI;
    // DEBUG(errs() << *Func << "\n");
    for (Function::iterator BI = Func->begin(), BE = Func->end(); BI != BE;
         ++BI) {
      BasicBlock *BB = &*BI;
      for (BasicBlock::iterator I = BB->begin(), E = BB->end(); I != E; ++I) {
        Instruction *BBI = &*I;
        //if (true == isa<StoreInst>(BBI)) {
        if (true == isa<LoadInst>(BBI)) {
          LoadInst *li  = dyn_cast<LoadInst>(BBI);
          Value *ptrOp = li->getPointerOperand();
          DEBUG(errs() << *li << "\t Result Name: " << li->getName() << "\t Pointer Name: " << ptrOp->getName() << "\n");

          // DEBUG(errs() << "\tStore Instruction: " << *BBI << " \n");
          // DEBUG(errs() << "\t\tPointerType: " << isLLVMPAPtrTy(SI->getType())
          // << " \n");
          // Instruction* V = cast<Instruction>(SI->getOperand(1));
          // DEBUG(errs() << "\tOperand : " << *V << " \n");
          // DEBUG(errs() << "\t\tPointerType: " << isLLVMPAPtrTy(V->getType())
          // << " \n");
        } else if(true == isa<GetElementPtrInst>(BBI)) {
          GetElementPtrInst *gep  = dyn_cast<GetElementPtrInst>(BBI);
          DEBUG(errs() << *gep << "\t Result Name: " << gep->getName() << "\n");
          // DEBUG(errs() << "\tInstruction: " << *BBI << " \n");
          // DEBUG(errs() << "\t\tPointerType: " <<
          // isLLVMPAPtrTy(BBI->getType()) << " \n");
        }

        // For def-use chains: All the uses of the definition
        //DEBUG(errs() << *BBI << "\n");
        /*
        for (User *U : BBI->users()) {
          if (Instruction *Inst = dyn_cast<Instruction>(U)) {
            DEBUG(errs()<< " " <<  *Inst << "\n");
          }
        }

        for (Value::user_iterator i = BBI->user_begin(), e = BBI->user_end();
              i != e; ++i) {
          if (Instruction *user_inst = dyn_cast<Instruction>(*i)) {
            DEBUG(errs()<< " " << *user_inst << "\n");
          }
        }
        */
      }
    }
  }
}
