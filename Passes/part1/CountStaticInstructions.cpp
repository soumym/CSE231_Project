#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "string"
#include "map"
#include "llvm/IR/InstIterator.h"


using namespace llvm;

namespace {
struct CountStaticInstructions : public FunctionPass {
  static char ID;
  std::map<std::string, long> freqMap; 
  CountStaticInstructions() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    // errs() << "Jello: ";
    // errs().write_escaped(F.getName()) << '\n';
    // for (BasicBlock &BB : F) {
    //     // Print out the name of the basic block if it has one, and then the
    //     // number of instructions that it contains
    //     // errs() << "Basic block (name=" << BB.getName() << ") has "
    //     //             << BB.size() << " instructions.\n";
    //     for (Instruction &I : BB) {
    //         // The next statement works since operator<<(ostream&,...)
    //         // is overloaded for Instruction&
    //         //errs() << I.getOpcodeName() << "\n";
    //         std::string str = I.getOpcodeName();
    //         
    //         //errs() << str << "\n";
    //     }   
    // }
    for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
        std::string str = I->getOpcodeName();
        if (freqMap.find(str) == freqMap.end()) {
            freqMap[str] = 1;
        } else {
            freqMap[str] += 1;
        }
    }
    for (auto key: freqMap) {
        errs() << key.first << "\t" << key.second << "\n";
    }
    return false;
  }
}; // end of struct CountStaticInstructions
}  // end of anonymous namespace

char CountStaticInstructions::ID = 0;
static RegisterPass<CountStaticInstructions> X("cse231-csi", "Developed to test LLVM and docker",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
