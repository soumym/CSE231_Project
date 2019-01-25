#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "string"
#include "map"
#include "vector"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/LLVMContext.h"
#include "/lib231/lib231.cpp"
#include "llvm/IR/Value.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/IRBuilder.h"


using namespace llvm;

namespace {
struct CountDynamicInstructions : public FunctionPass {
  static char ID; 
  CountDynamicInstructions() : FunctionPass(ID) {}

  bool doInitialization(Module &M) override {
      M.getContext();
      return false;
  }

  bool runOnFunction(Function &F) override {
    std::map<int, int> myMap; 

    Module *mod = F.getParent();
    bool doPrint = false;

    for (Function::iterator B = F.begin(), BE = F.end(); B != BE; ++B) {
        myMap.clear();
        doPrint = false;
        for (BasicBlock::iterator I = B->begin(), IE = B->end(); I != IE; ++I) {
            auto inst = I->getOpcode();
            if (myMap.find(inst) == myMap.end()) {
                myMap[inst] = 1;
            } else {
                myMap[inst] += 1;
            }
            // errs() << I->getOpcodeName() << '\n';
            if (std::string(I->getOpcodeName()) == "ret") {
                doPrint = true;
            }
        }
        std::vector<Constant*> vv;
        std::vector<Constant*> kk;
        for(auto itr = myMap.begin(); itr != myMap.end(); ++itr) {
            auto key = ConstantInt::get(IntegerType::get(F.getContext(),32), itr->first);
            auto val = ConstantInt::get(IntegerType::get(F.getContext(),32), itr->second);
            kk.push_back(key);
            vv.push_back(val);
            // errs() << itr->first << ' ' << itr->second << '\n';
        }
        ArrayType* ArrayTy = ArrayType::get(IntegerType::get(F.getContext(), 32), kk.size());
        // errs() << F.getName() << ' ' << kk.size() << '\n';
        GlobalVariable* k = new GlobalVariable(
            *mod,
            ArrayTy,
            true,
            GlobalValue::InternalLinkage,
            ConstantArray::get(ArrayTy, kk),
            "my_array1");
        GlobalVariable* v = new GlobalVariable(
            *mod,
            ArrayTy,
            true,
            GlobalValue::InternalLinkage,
            ConstantArray::get(ArrayTy, vv),
            "my_array2");
        
        // errs() << "Created" << '\n';
        Constant* num = ConstantInt::get(IntegerType::get(F.getContext(),32), kk.size());
        auto ptrType = Type::getInt32PtrTy(mod->getContext());
        auto intType = Type::getInt32Ty(mod->getContext());
        auto voidType = Type::getVoidTy(mod->getContext());
        Constant *fooFunc = mod->getOrInsertFunction(
            "updateInstrInfo", voidType, intType, ptrType, ptrType 
            );
        IRBuilder<> Builder(&*B->getFirstInsertionPt());
        std::vector<Value*> args;
        Value* castK = Builder.CreatePointerCast(k, Type::getInt32PtrTy(F.getContext()));
        Value* castV = Builder.CreatePointerCast(v, Type::getInt32PtrTy(F.getContext()));
        args.push_back(num);
        args.push_back(castK);
        args.push_back(castV);
        Builder.CreateCall(fooFunc, args);

        if (doPrint) {
            Constant *barFunc = mod->getOrInsertFunction(
                "printOutInstrInfo", voidType
                );
            args.clear();
            Builder.CreateCall(barFunc, args);
        }
    }

    

    
    return false;
  }
}; // end of struct CountDynamicInstructions
}  // end of anonymous namespace

char CountDynamicInstructions::ID = 0;
static RegisterPass<CountDynamicInstructions> X("cse231-cdi", "Count Dynamic Instructions",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
