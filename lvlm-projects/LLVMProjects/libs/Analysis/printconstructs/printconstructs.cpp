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

#define DEBUG_TYPE "printcontructs"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/IR/IRBuilder.h"
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


struct printcontructs : public ModulePass {
  static char ID;
  printcontructs() : ModulePass(ID) {}
  bool runOnModule(Module &M);

  void performTest();
  bool isLLVMPAPtrTy(llvm::Type *);

  virtual void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesCFG();
  }

   std::tuple<VectorType*, Type*> getFPVectorTypes(int ewidth, int count);
   std::tuple<VectorType*, Type*> getIntVectorTypes(int ewidth, int count);
  template<int width, int elementwidth>
   Value *INT_AS_FPVECTOR(Value *input) ;
template<int width, int elementwidth>
 Value * INT_AS_VECTOR(Value *input);
  template <int width, int elemwidth, Instruction::BinaryOps bin_op> 
   void do_SSE_FP_VECTOR_OP(Value *v1, Value *v2);

private:
  Module *Mod;
  Instruction *EI;
  LLVMContext *ctx;
  // const llvm::DataLayout *DL;
};

char printcontructs::ID = 0;
static RegisterPass<printcontructs> X("printcontructs", "DSAND Testing Code ",
                             false /* does not modify the CFG */,
                             false /* transformation, not just analysis */);

/*******************************************************************
* Function : runOnModule
* Purpose : Entry point for Test
********************************************************************/
SwitchInst *GSI;

bool printcontructs::runOnModule(Module &M) {
  Mod = &M;
  bool Changed = false;

  BasicBlock *eb = &(Mod->begin()->front());
  EI = &*(eb->getTerminator());
  ctx = &Mod->getContext();

  performTest();


  return Changed;
}


void printcontructs::performTest() {


  IRBuilder<> IRB(EI);
  IntegerType *int128 = Type::getIntNTy(*ctx, 128);

  auto *alloca1 = IRB.CreateAlloca(int128, nullptr, "xmm1");
  auto *alloca2 = IRB.CreateAlloca(int128, nullptr, "xmm2");
  auto *load_alloca1 = IRB.CreateLoad(alloca1);
  auto *load_alloca2 = IRB.CreateLoad(alloca2);

  do_SSE_FP_VECTOR_OP<128, 64, Instruction::FAdd>(load_alloca1, load_alloca2);

  /*
  int ewidth = 64;
  int count = 2;
  Type *elem64_ty = Type::getDoubleTy(*ctx);
  Type *elem32_ty = Type::getFloatTy(*ctx);
  VectorType *vt64 = VectorType::get( elem64_ty, count);
  VectorType *vt32 = VectorType::get( elem32_ty, count);
  */


}


template <int width, int elemwidth, Instruction::BinaryOps bin_op>
void 
printcontructs::do_SSE_FP_VECTOR_OP(Value *v1, Value *v2)
{
    llvm::errs()  << *v1 << "\n"  << *v2 << "\n";
    Value *vecInput1 = INT_AS_FPVECTOR<width,elemwidth>(v1);
    Value *vecInput2 = INT_AS_FPVECTOR<width,elemwidth>(v2);

    llvm::errs()  << *vecInput1 << "\n"  << *vecInput2 << "\n";
    IRBuilder<> IRB(EI);
    Value *op_out = IRB.CreateBinOp(bin_op, vecInput1, vecInput2);
    Value *intOutput = IRB.CreateCast(Instruction::BitCast, op_out, Type::getIntNTy(*ctx, width));
    /*
    Value *op_out = BinaryOperator::Create(
        bin_op,
        vecInput1,
        vecInput2,
        "",
        b);

    Value *intOutput = CastInst::Create(
            Instruction::BitCast,
            op_out,
            Type::getIntNTy(b->getContext(), width),
            "",
            b);
        */

    llvm::errs() << *intOutput << "\n";
    return ;
}


 std::tuple<VectorType*, Type*> 
printcontructs::getIntVectorTypes(int ewidth, int count) {
    Type *elem_ty = Type::getIntNTy(*ctx, ewidth);
    VectorType *vt = VectorType::get(
            elem_ty,
            count);

    return std::tuple<VectorType*,Type*>(vt, elem_ty);
}

 std::tuple<VectorType*, Type*> 
printcontructs::getFPVectorTypes(int ewidth, int count) {
    Type *elem_ty = nullptr;
    
    switch(ewidth) {
        case 64:
            elem_ty = Type::getDoubleTy(*ctx);
            break;
        case 32:
            elem_ty = Type::getFloatTy(*ctx);
            break;
        default:
            assert(false && "Invalid width for fp vector");
    }

    VectorType *vt = VectorType::get(
            elem_ty,
            count);

    return std::tuple<VectorType*,Type*>(vt, elem_ty);
}

template<int width, int elementwidth>
 Value *
printcontructs::INT_AS_VECTOR(Value *input) {

    assert(width % elementwidth == 0);

    unsigned count = width/elementwidth;

    Type *elem_ty;
    VectorType *vt;

    std::tie(vt, elem_ty) = getIntVectorTypes(elementwidth, count);

    // convert our base value to a vector
    IRBuilder<> IRB(EI);
    Value *vecValue = IRB.CreateCast(Instruction::BitCast, input, vt);
    /*
    Value *vecValue = CastInst::Create(
            Instruction::BitCast,
            input,
            vt,
            "",
            b);
            */

    return vecValue;
}

template<int width, int elementwidth>
Value *
printcontructs::INT_AS_FPVECTOR(Value *input) {

    assert(width % elementwidth == 0);

    unsigned count = width/elementwidth;

    Type *elem_ty;
    VectorType *vt;

    std::tie(vt, elem_ty) = getFPVectorTypes(elementwidth, count);

    IRBuilder<> IRB(EI);
    // convert our base value to a vector
    Value *vecValue = IRB.CreateCast(Instruction::BitCast, input, vt);
    /*
    Value *vecValue = CastInst::Create(
            Instruction::BitCast,
            input,
            vt,
            "",
            b);
            */
    return vecValue;
}


