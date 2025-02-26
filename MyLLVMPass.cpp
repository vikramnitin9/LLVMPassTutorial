#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/Support/raw_ostream.h"

#include <fstream>

using namespace llvm;

class MyLLVMPass : public PassInfoMixin<MyLLVMPass> {
public:
    PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM) {

      auto &CG = AM.getResult<CallGraphAnalysis>(M);
      // Iterate through functions in the module
      for (Function &F : M) {
        if (F.isDeclaration())
            continue; // Skip declarations without a body
        
        const llvm::Function *constFunctionPtr = &F;
        // Print all the functions that are called by the current function
        std::set<std::string> calledFunctions = {};
        for (auto &I : *CG[constFunctionPtr]) {
            if (I.second) {
              if (I.second->getFunction()) {
                // If it belongs to the same module
                if (I.second->getFunction()->getParent() == &M && !I.second->getFunction()->isDeclaration())
                  calledFunctions.insert(I.second->getFunction()->getName().str());
              }
            }
        }

        // Print the function name and the functions it calls
        errs() << "Function " << F.getName() << " calls: ";
        for (auto &name : calledFunctions) {
          errs() << name << " ";
        }
        errs() << "\n";
      }
      return PreservedAnalyses::all();
    }
};

// Register the pass for opt
extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return {LLVM_PLUGIN_API_VERSION, "MyLLVMPass", LLVM_VERSION_STRING,
            [](PassBuilder &PB) {
                PB.registerPipelineParsingCallback(
                    [](StringRef Name, ModulePassManager &PM, 
                       ArrayRef<PassBuilder::PipelineElement>) {
                        if (Name == "my-pass") {
                            PM.addPass(MyLLVMPass());
                            return true;
                        }
                        return false;
                    });
                // Register analysis passes properly
                PB.registerAnalysisRegistrationCallback(
                    [](ModuleAnalysisManager &AM) {
                        AM.registerPass([] { return CallGraphAnalysis(); });
                    });
            }};
}