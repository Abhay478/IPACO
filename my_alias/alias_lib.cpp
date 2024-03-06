#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/InstIterator.h"
// #include "llvm/Analysis/ConstantFolding.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Use.h"
#include "llvm/IR/User.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"

#include <string>
#include <set>
#include <map>
#include <fstream>
using namespace llvm;

namespace {


typedef DenseMap<Value *, > PTS 
struct alias_c : public FunctionPass {
  static char ID;
  alias_c() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    // write your code here



    return false;
  }
}; // end of struct alias_c
}  // end of anonymous namespace

char alias_c::ID = 0;
static RegisterPass<alias_c> X("alias_lib_given", "Alias Analysis Pass for Assignment",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
