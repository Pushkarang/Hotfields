#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Analysis/LoopInfo.h"

using namespace llvm;
namespace {
	struct HotPass : public FunctionPass {
		typedef std::map<std::string ,float>profile;
		profile p; 	
		static char ID;
		HotPass() : FunctionPass(ID) {} 
		void getAnalysisUsage(AnalysisUsage &AU) const override {
			AU.setPreservesCFG();
			AU.addRequired<LoopInfoWrapperPass>();
		}

		bool runOnFunction(Function &F) override;

	};
}
bool HotPass::runOnFunction(Function &F) {
	LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
// Iterate through each instruction 
	for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I)
		//Get number of oprand in inst and iterate through each operand
		for (unsigned op = 0; op < I->getNumOperands();op++){
			if(Instruction *II = dyn_cast<Instruction>(I->getOperand(op))){
				// if instruction is of getelementptr and of struct type then put into map with loopdepth as value
				if(II->getOpcode()==Instruction::GetElementPtr){
					if(GetElementPtrInst *gp = dyn_cast<GetElementPtrInst>(I->getOperand(op))){
						Type *T = gp->getSourceElementType();
						if(T->isStructTy()){
							BasicBlock *b = II->getParent();
							std::string ts ;
							llvm::raw_string_ostream rso(ts);
							(gp->getPointerOperandType()->print(rso));

							std::string s = rso.str();

							gp->getOperand(2)->print(rso);
							s = rso.str();
							// s is a string with pointeroperandtype and struct name as a key in map

							if(p.find(s)== p.end()){
								p[s] = LI.getLoopDepth(b);
							}  
							//if already present add loopdepth to it's value
							else{
								p[s] = p[s]+LI.getLoopDepth(b);
							}



						}	

					}
				}
			} 	
		}
		typedef profile::iterator iter;
		iter it = p.begin();
		iter end = p.end();

		float max_value = it->second;
		float min_value = 999999;
		std::string str = it->first;
		std::string strm = it->first;
// finding max and min value for all entries in map for normalization
		for( ; it != end; ++it) {
			if((it->second) > max_value) {
				max_value = it->second;
				str = it->first;
			}
			if(it->second < min_value){
				min_value = it->second;
				strm = it->first;
			}

		}
		//Normalizing map 
		
		float f =max_value - min_value;
		for( it = p.begin(), end = p.end()	; it != end; ++it) {
			float tmp = it->second;
			tmp = tmp - min_value;
			tmp = tmp / f;
			p[it->first] = tmp;

		}
		//print key and value of map if its value  is > 0.6
		for( it = p.begin(), end = p.end();	 it != end; ++it) {
			if(it->second > .6){
				errs()<<F.getName()<<" " <<it->first<<"is HOT!!\n";
			}
		}

		return false;
	}
	char HotPass::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
	static RegisterPass<HotPass> X("printhotfields", "Prints Hot fields for each function");