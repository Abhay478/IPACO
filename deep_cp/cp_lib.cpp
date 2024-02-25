#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Analysis/ConstantFolding.h"
#include "llvm/IR/Constants.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
// #include "llvm/IR/Module.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <set>
#include <map>
#include <fstream>
#include <signal.h>
#include "llvm/IR/Instructions.h"


using namespace llvm;

namespace {


DenseSet<BasicBlock*> basicBlocks ;

typedef DenseMap<Value*, Constant*> constantMap;
DenseMap<BasicBlock*, constantMap> blockMap;

DenseMap<Instruction*, constantMap> instMap;

DenseMap<Value*,StringRef> varConstMAp;
    
Function * thefunc;



void printWrongFunc(Function &F){
       
        for (auto &BB : F)
        {
            errs() << BB.getName() << " : \n";

            for(auto &I : BB){

                constantMap cm = instMap[&I];
                

                switch (I.getOpcode())
                {
                    case Instruction::Alloca:{
                        errs() << I << " --> ";

                        Value * alloc_val = dyn_cast<Value>(&I);
                        /* auto *instruct = &I;
                        
                        if(!instruct->hasName()){
                            errs() << "%"<< varConstMAp[instruct] << ":";  
                        }
                        else{
                            errs() << "%" ;
                            errs() << instruct->getName() << ":";
                        }


                        if(cm.find(instruct)!=cm.end()){
                            if(auto* t = dyn_cast<ConstantDataArray>(cm[instruct])){
                                if (t->isString() && t->getAsCString() == "BOTTOM") {
                                    errs() << "BOTTOM, ";
                                }else if(t->isString() && t->getAsCString() == "TOP"){
                                    errs() << "TOP, ";
                                } 
                            }else{
                                errs() << *cm[instruct] << " , ";  
                            }
                        }

 */
                        
                        errs() << "%" << alloc_val->getName() << ":";
                        //if alloc_val in cm
                        if(cm.find(alloc_val)!=cm.end()){
                            if(auto* t = dyn_cast<ConstantDataArray>(cm[alloc_val])){
                                if (t->isString() && t->getAsCString() == "BOTTOM") {
                                    errs() << "BOTTOM, ";
                                }else if(t->isString() && t->getAsCString() == "TOP"){
                                    errs() << "TOP, ";
                                } 
                            }else{
                                errs() << *cm[alloc_val] << " , ";  
                            }
                        }

                        errs()<<"\n";  
                       
                        break;
                    }
                            
                        
                    case Instruction::Load : {
                        errs() << I << " --> ";

                        Value * load_val = I.getOperand(0);

                        auto *instruct = &I;

                        // errs() << "\nValue of instruct at load :"<< I << "\n";

                        if(!instruct->hasName()){
                            // errs() << "\nNo name :::serach in varmap:::";
                            
                            errs() << "%"<< varConstMAp[instruct] << ":";
                            // errs() << "\n";
                        }else{
                            errs() << "%" << instruct->getName() << ":";
                        }


                        if(cm.find(instruct)!=cm.end()){
                            if(auto* t = dyn_cast<ConstantDataArray>(cm[instruct])){
                                if (t->isString() && t->getAsCString() == "BOTTOM") {
                                    errs() << "BOTTOM, ";
                                }else if(t->isString() && t->getAsCString() == "TOP"){
                                    errs() << "TOP, ";
                                } 
                            }else{
                                errs() << *cm[instruct] << " , ";  
                            }
                        }

                        
                    
                        errs() << "%" <<load_val->getName() << ":";
                        
                        //if load_val in cm
                        if(cm.find(load_val)!=cm.end()){
                            if(auto* t = dyn_cast<ConstantDataArray>(cm[load_val])){
                                if (t->isString() && t->getAsCString() == "BOTTOM") {
                                    errs() << "BOTTOM, ";
                                }else if(t->isString() && t->getAsCString() == "TOP"){
                                    errs() << "TOP, ";
                                } 
                            }else{
                                errs() << *cm[load_val] << " , ";  
                            }
                        }


                        errs()<<"\n";  
                        break;
                    }

                    case Instruction::Add: {
                        errs() << I << " --> ";

                    
                        Value *op1 = I.getOperand(0);
                        Value *op2 = I.getOperand(1);


                        if(!dyn_cast<ConstantInt>(op1)){
                            if(!op1->hasName()){
                                errs() << "%"<< varConstMAp[op1] << ":";
                            }else{
                                errs() << "%" << op1->getName() << ":";
                            }

                            if(cm.find(op1)!=cm.end()){
                                if(auto* t = dyn_cast<ConstantDataArray>(cm[op1])){
                                    if (t->isString() && t->getAsCString() == "BOTTOM") {
                                        errs() << "BOTTOM, ";
                                    }else if(t->isString() && t->getAsCString() == "TOP"){
                                        errs() << "TOP, ";
                                    } 
                                }else{
                                    errs() << *cm[op1] << " , ";  
                                }
                            }
                        }

                        if(!dyn_cast<ConstantInt>(op2)){
                            if(!op2->hasName()){
                                errs() << "%"<< varConstMAp[op2] << ":";
                            }else{
                                errs() << "%" << op2->getName() << ":";
                            }

                            if(cm.find(op2)!=cm.end()){
                                if(auto* t = dyn_cast<ConstantDataArray>(cm[op2])){
                                    if (t->isString() && t->getAsCString() == "BOTTOM") {
                                        errs() << "BOTTOM, ";
                                    }else if(t->isString() && t->getAsCString() == "TOP"){
                                        errs() << "TOP, ";
                                    } 
                                }else{
                                    errs() << *cm[op2] << " , ";  
                                }
                            }
                        }



                        auto *instruct = &I;

                        if(!instruct->hasName()){
                            // errs() << "\nNo name :::serach in varmap:::";
                            
                            errs() << "%"<< varConstMAp[instruct] << ":";
                            // errs() << "\n";
                        }else{
                            errs() << "%" << instruct->getName() << ":";
                        }


                        if(cm.find(instruct)!=cm.end()){
                            if(auto* t = dyn_cast<ConstantDataArray>(cm[instruct])){
                                if (t->isString() && t->getAsCString() == "BOTTOM") {
                                    errs() << "BOTTOM, ";
                                }else if(t->isString() && t->getAsCString() == "TOP"){
                                    errs() << "TOP, ";
                                } 
                            }else{
                                errs() << *cm[instruct] << " , ";  
                            }
                        }


                        errs() <<"\n";
                        break;
                    } 


                    case Instruction::Sub: {
                        errs() << I << " --> ";

                        Value *op1 = I.getOperand(0);
                        Value *op2 = I.getOperand(1);


                        if(!dyn_cast<ConstantInt>(op1)){
                            if(!op1->hasName()){
                                errs() << "%"<< varConstMAp[op1] << ":";
                            }else{
                                errs() << "%" << op1->getName() << ":";
                            }

                            if(cm.find(op1)!=cm.end()){
                                if(auto* t = dyn_cast<ConstantDataArray>(cm[op1])){
                                    if (t->isString() && t->getAsCString() == "BOTTOM") {
                                        errs() << "BOTTOM, ";
                                    }else if(t->isString() && t->getAsCString() == "TOP"){
                                        errs() << "TOP, ";
                                    } 
                                }else{
                                    errs() << *cm[op1] << " , ";  
                                }
                            }
                        }

                        if(!dyn_cast<ConstantInt>(op2)){
                            if(!op2->hasName()){
                                errs() << "%"<< varConstMAp[op2] << ":";
                            }else{
                                errs() << "%" << op2->getName() << ":";
                            }

                            if(cm.find(op2)!=cm.end()){
                                if(auto* t = dyn_cast<ConstantDataArray>(cm[op2])){
                                    if (t->isString() && t->getAsCString() == "BOTTOM") {
                                        errs() << "BOTTOM, ";
                                    }else if(t->isString() && t->getAsCString() == "TOP"){
                                        errs() << "TOP, ";
                                    } 
                                }else{
                                    errs() << *cm[op2] << " , ";  
                                }
                            }
                        }



                        auto *instruct = &I;

                        if(!instruct->hasName()){
                            // errs() << "\nNo name :::serach in varmap:::";
                            
                            errs() << "%"<< varConstMAp[instruct] << ":";
                            // errs() << "\n";
                        }else{
                            errs() << "%" << instruct->getName() << ":";
                        }


                        if(cm.find(instruct)!=cm.end()){
                            if(auto* t = dyn_cast<ConstantDataArray>(cm[instruct])){
                                if (t->isString() && t->getAsCString() == "BOTTOM") {
                                    errs() << "BOTTOM, ";
                                }else if(t->isString() && t->getAsCString() == "TOP"){
                                    errs() << "TOP, ";
                                } 
                            }else{
                                errs() << *cm[instruct] << " , ";  
                            }
                        }


                        errs() <<"\n";
                        break;
                    } 


                    case Instruction::Mul: {
                        errs() << I << " --> ";

                    
                        Value *op1 = I.getOperand(0);
                        Value *op2 = I.getOperand(1);


                        if(!dyn_cast<ConstantInt>(op1)){
                            if(!op1->hasName()){
                                errs() << "%"<< varConstMAp[op1] << ":";
                            }else{
                                errs() << "%" << op1->getName() << ":";
                            }

                            if(cm.find(op1)!=cm.end()){
                                if(auto* t = dyn_cast<ConstantDataArray>(cm[op1])){
                                    if (t->isString() && t->getAsCString() == "BOTTOM") {
                                        errs() << "BOTTOM, ";
                                    }else if(t->isString() && t->getAsCString() == "TOP"){
                                        errs() << "TOP, ";
                                    } 
                                }else{
                                    errs() << *cm[op1] << " , ";  
                                }
                            }
                        }

                        if(!dyn_cast<ConstantInt>(op2)){
                            if(!op2->hasName()){
                                errs() << "%"<< varConstMAp[op2] << ":";
                            }else{
                                errs() << "%" << op2->getName() << ":";
                            }

                            if(cm.find(op2)!=cm.end()){
                                if(auto* t = dyn_cast<ConstantDataArray>(cm[op2])){
                                    if (t->isString() && t->getAsCString() == "BOTTOM") {
                                        errs() << "BOTTOM, ";
                                    }else if(t->isString() && t->getAsCString() == "TOP"){
                                        errs() << "TOP, ";
                                    } 
                                }else{
                                    errs() << *cm[op2] << " , ";  
                                }
                            }
                        }



                        auto *instruct = &I;

                        if(!instruct->hasName()){
                            // errs() << "\nNo name :::serach in varmap:::";
                            
                            errs() << "%"<< varConstMAp[instruct] << ":";
                            // errs() << "\n";
                        }else{
                            errs() << "%" << instruct->getName() << ":";
                        }


                        if(cm.find(instruct)!=cm.end()){
                            if(auto* t = dyn_cast<ConstantDataArray>(cm[instruct])){
                                if (t->isString() && t->getAsCString() == "BOTTOM") {
                                    errs() << "BOTTOM, ";
                                }else if(t->isString() && t->getAsCString() == "TOP"){
                                    errs() << "TOP, ";
                                } 
                            }else{
                                errs() << *cm[instruct] << " , ";  
                            }
                        }


                        errs() <<"\n";
                        break;
                    } 

                    case Instruction::SDiv: {
                        errs() << I << " --> ";

                    
                        Value *op1 = I.getOperand(0);
                        Value *op2 = I.getOperand(1);


                        if(!dyn_cast<ConstantInt>(op1)){
                            if(!op1->hasName()){
                                errs() << "%"<< varConstMAp[op1] << ":";
                            }else{
                                errs() << "%" << op1->getName() << ":";
                            }

                            if(cm.find(op1)!=cm.end()){
                                if(auto* t = dyn_cast<ConstantDataArray>(cm[op1])){
                                    if (t->isString() && t->getAsCString() == "BOTTOM") {
                                        errs() << "BOTTOM, ";
                                    }else if(t->isString() && t->getAsCString() == "TOP"){
                                        errs() << "TOP, ";
                                    } 
                                }else{
                                    errs() << *cm[op1] << " , ";  
                                }
                            }
                        }

                        if(!dyn_cast<ConstantInt>(op2)){
                            if(!op2->hasName()){
                                errs() << "%"<< varConstMAp[op2] << ":";
                            }else{
                                errs() << "%" << op2->getName() << ":";
                            }

                            if(cm.find(op2)!=cm.end()){
                                if(auto* t = dyn_cast<ConstantDataArray>(cm[op2])){
                                    if (t->isString() && t->getAsCString() == "BOTTOM") {
                                        errs() << "BOTTOM, ";
                                    }else if(t->isString() && t->getAsCString() == "TOP"){
                                        errs() << "TOP, ";
                                    } 
                                }else{
                                    errs() << *cm[op2] << " , ";  
                                }
                            }
                        }



                        auto *instruct = &I;

                        if(!instruct->hasName()){
                            // errs() << "\nNo name :::serach in varmap:::";
                            
                            errs() << "%"<< varConstMAp[instruct] << ":";
                            // errs() << "\n";
                        }else{
                            errs() << "%" << instruct->getName() << ":";
                        }


                        if(cm.find(instruct)!=cm.end()){
                            if(auto* t = dyn_cast<ConstantDataArray>(cm[instruct])){
                                if (t->isString() && t->getAsCString() == "BOTTOM") {
                                    errs() << "BOTTOM, ";
                                }else if(t->isString() && t->getAsCString() == "TOP"){
                                    errs() << "TOP, ";
                                } 
                            }else{
                                errs() << *cm[instruct] << " , ";  
                            }
                        }


                        errs() <<"\n";
                        break;
                    } 

                    case Instruction::SRem: {
                        errs() << I << " --> ";

                    
                        Value *op1 = I.getOperand(0);
                        Value *op2 = I.getOperand(1);


                        if(!dyn_cast<ConstantInt>(op1)){
                            if(!op1->hasName()){
                                errs() << "%"<< varConstMAp[op1] << ":";
                            }else{
                                errs() << "%" << op1->getName() << ":";
                            }

                            if(cm.find(op1)!=cm.end()){
                                if(auto* t = dyn_cast<ConstantDataArray>(cm[op1])){
                                    if (t->isString() && t->getAsCString() == "BOTTOM") {
                                        errs() << "BOTTOM, ";
                                    }else if(t->isString() && t->getAsCString() == "TOP"){
                                        errs() << "TOP, ";
                                    } 
                                }else{
                                    errs() << *cm[op1] << " , ";  
                                }
                            }
                        }

                        if(!dyn_cast<ConstantInt>(op2)){
                            if(!op2->hasName()){
                                errs() << "%"<< varConstMAp[op2] << ":";
                            }else{
                                errs() << "%" << op2->getName() << ":";
                            }

                            if(cm.find(op2)!=cm.end()){
                                if(auto* t = dyn_cast<ConstantDataArray>(cm[op2])){
                                    if (t->isString() && t->getAsCString() == "BOTTOM") {
                                        errs() << "BOTTOM, ";
                                    }else if(t->isString() && t->getAsCString() == "TOP"){
                                        errs() << "TOP, ";
                                    } 
                                }else{
                                    errs() << *cm[op2] << " , ";  
                                }
                            }
                        }



                        auto *instruct = &I;

                        if(!instruct->hasName()){
                            // errs() << "\nNo name :::serach in varmap:::";
                            
                            errs() << "%"<< varConstMAp[instruct] << ":";
                            // errs() << "\n";
                        }else{
                            errs() << "%" << instruct->getName() << ":";
                        }


                        if(cm.find(instruct)!=cm.end()){
                            if(auto* t = dyn_cast<ConstantDataArray>(cm[instruct])){
                                if (t->isString() && t->getAsCString() == "BOTTOM") {
                                    errs() << "BOTTOM, ";
                                }else if(t->isString() && t->getAsCString() == "TOP"){
                                    errs() << "TOP, ";
                                } 
                            }else{
                                errs() << *cm[instruct] << " , ";  
                            }
                        }


                        errs() <<"\n";
                        break;
                    } 


                    case Instruction::Store: {
                        auto *var = I.getOperand(1);
                        auto *val = I.getOperand(0);

                        errs() << I << " --> ";

                        if(!var->hasName()){
                            errs() << "%"<< varConstMAp[var] << ":";
                        }else{
                            errs() << "%" << var->getName() << ":";
                        }

                        if(auto v = dyn_cast<ConstantInt>(var)){
                            errs() << *v << " , ";
                        }
                        else if (cm.find(var) != cm.end()) {
                            if (auto * t=dyn_cast<ConstantDataArray>(cm[var])){
                                if (t->isString() && t->getAsCString() == "BOTTOM") {
                                    errs() << "BOTTOM, ";
                                }else if(t->isString() && t->getAsCString() == "TOP"){
                                    errs() << "TOP, ";
                                }
                            }else{
                                errs() << *cm[var] << " , ";
                            }
                             
                        }


                        if(val->hasName()){
                            // errs() << "%"<< varConstMAp[val] << ":";
                
                            errs() << "%" << val->getName() << ":";
                        


                        if(auto v = dyn_cast<ConstantInt>(val)){
                            errs() << *v << " , ";
                        }
                        else if (cm.find(val) != cm.end()) {
                            if (auto * t=dyn_cast<ConstantDataArray>(cm[val])){
                                if (t->isString() && t->getAsCString() == "BOTTOM") {
                                    errs() << "BOTTOM, ";
                                }else if(t->isString() && t->getAsCString() == "TOP"){
                                    errs() << "TOP, ";
                                }
                            }else{
                                errs() << *cm[val] << " , ";
                            }
                             
                        }
                        }



                        errs () << "\n";

                        break;
                        
                    } 

                    default:{
                        errs() << I << " --> ";
                        errs() << "\n" ;
                        break;
                    }
                }   
            }
        }     
    }

void handler(int sig) {
    errs() << "Error: Segmentation fault\n";
    printWrongFunc(*thefunc);
    exit(1);
}


void  printFile(Function &F){
    std::ofstream file;
    file.open("output.txt");
   
}

struct constant_prop : public FunctionPass {
	static char ID;
	constant_prop() : FunctionPass(ID) {}

	bool runOnFunction(Function &F) override {
        // Worklist init.
        thefunc = &F;
        signal(SIGSEGV, handler);

        for (BasicBlock &BB : F) {
            basicBlocks.insert(&BB);
        }
        //iterate over F and print block names
        // for (auto &BB : F) {
        //     errs() << "Basic Block: " << BB.getName() << "\n";
        // }
        

        while (!basicBlocks.empty()) {
            // pop
            BasicBlock *BB = *basicBlocks.begin();
            basicBlocks.erase(BB);

            // errs() << "Basic Block: " << BB->getName() << "\n";


            //initialize blockMap for BB
            if (blockMap.find(BB) == blockMap.end()) {
                constantMap newMap;
                blockMap[BB] = newMap;
                // errs() << "New Map for : " << BB->getName() << "\n";
            }

            constantMap old = constantMap(blockMap[BB]);
            // bool eh = false;
            
            DenseMap<Value*, llvm::SmallVector<Constant*>> phiMap;

            // Preds
            for (BasicBlock* PredBB : predecessors(BB)) {
                //iterate over blockMap of predecessor
                for (auto it = blockMap[PredBB].begin(); it != blockMap[PredBB].end(); it++) {
                    Value* var = it->first;

                    Constant* val = it->second;
                        // errs() << *val <<"\n";
                    if (phiMap.find(var) == phiMap.end()) {
                        // init entry
                        phiMap[var] = SmallVector<Constant*>();

                    } 
                    
                    // else {errs() << "dupe\n";}

                    // errs() <<"var:" << var->getName() <<"\n";

                    // if (std::find(phiMap[var].begin(), phiMap[var].end(), val) == phiMap[var].end()){
                    //     // errs() << *val <<"\n";
                    // }
                    //    errs() <<"val:" << *val <<"\n";
                        phiMap[var].push_back(val);
                        // errs() << "phimap[var]: "<<*phiMap[var].back() <<"\n";
                }
            }

            // errs() << "PhiMap for : " << BB->getName() << "\n";

             // iterate over phiMap
            for (auto it = phiMap.begin(); it != phiMap.end(); it++) {
                    Value* var = it->first;
                    SmallVector<Constant*> vals = it->second;
                    // errs() << var->getName() <<": ";
            
                    for (auto v = vals.begin(); v!=vals.end(); v++){
                        Constant* val = *v;
                        // errs() << *val << ", ";

                        if (blockMap[BB].find(var) == blockMap[BB].end()) {
                            blockMap[BB].insert({var,val});
                        } 
                        //var is in blockMap
                        else {
                            if (auto str = dyn_cast<ConstantDataArray>(val)) {

                                 // Check if val is BOTTOM
                                if (str->isString() && str->getAsCString() == "BOTTOM") {
                                    blockMap[BB].insert({var, val});
                                    break; // No need to process further values for this variable
                                }
                            
                                // Check if val is TOP
                                else if (str->isString() && str->getAsCString() == "TOP") {
                                    blockMap[BB].insert({var, val});
                                }
                                else if (blockMap[BB][var] != val) {
                                    // Set the value to BOTTOM
                                    blockMap[BB][var] = ConstantDataArray::getString(BB->getContext(), "BOTTOM");
                                    
                                    break; // No need to process further values for this variable
                                }
                                else{ // Shouldn't this go to the outer IF?
                                    // errs() << "same value\n";
                                }
                            }
                                // Check if val is not equal to blockMap[BB][var]
                                else if (blockMap[BB][var] != val) {
                                    // Set the value to BOTTOM
                                    blockMap[BB][var] = ConstantDataArray::getString(BB->getContext(), "BOTTOM");
                                    
                                    break; // No need to process further values for this variable
                                }
                                else{ // Shouldn't this go to the outer IF?
                                    // errs() << "same value\n";
                                }
                            }
                            // else {
                            //     // errs() << "Not a string\n";
                            //     blockMap[BB][var] = ConstantDataArray::getString(BB->getContext(), "BOTTOM");

                            // }
                        // }
                    }
            
                // errs() << "\n";
            }
               
            // errs() << "OUT for : " << BB->getName() << "\n";

            // iterate over intructions in the block
            for(auto &I: *BB){
                // errs() << "Basic Block: " << BB->getName() << "\n";
                // errs() << I << "Started \n";
                switch (I.getOpcode())
                    {

                    case Instruction::Alloca : {
                        // errs() << "Alloca\n";
                        // errs() << blockMap[BB][(Value*)&I] << "\n";
                        auto *instruct = &I;
                        if (!instruct->hasName()){
                            // convert I to string
                            std::string InstString;
                            llvm::raw_string_ostream InstStream(InstString);
                            // Print the instruction to the string stream
                            instruct->print(InstStream);

                            // Extract the instruction string from the string stream
                            std::string InstText = InstStream.str();

                            // Print the instruction string
                            // errs() << InstText << "\n";

                            size_t equalPos = InstText.find("=");
                        

                            std::string LHSText = InstText.substr(0, equalPos);

                            size_t perPos = LHSText.find("%");

                            LHSText = LHSText.substr(perPos+1, LHSText.length()-perPos-1);
                            // errs() << LHSText << "\n";
                            varConstMAp[instruct] = LHSText;
                        }
 
                        Constant* ac = ConstantDataArray::getString(BB->getContext(), "TOP");

                        blockMap[BB][(Value*)&I] = ac;
                        break;
                    }
                    
                    case Instruction::Load : {
                        // errs() << "\n" << I << " --> \n";
                        auto *var = I.getOperand(0);
                        auto *instruct = &I;

                        if(!instruct->hasName()){
                         // convert I to string
                
                            std::string InstString;
                            llvm::raw_string_ostream InstStream(InstString);

                            // Print the instruction to the string stream
                            instruct->print(InstStream);

                            // Extract the instruction string from the string stream
                            std::string InstText = InstStream.str();

                            // Print the instruction string
                            // errs() << InstText << "\n";
                            size_t equalPos = InstText.find("=");
                        

                            std::string LHSText = InstText.substr(0, equalPos);

                            size_t perPos = LHSText.find("%");

                            LHSText = LHSText.substr(perPos+1, LHSText.length()-perPos-1);

                            // errs() << LHSText << "\n";
                            varConstMAp[instruct] = LHSText;

                            // errs() << "varconstMap[ " << I <<" ]" << varConstMAp[instruct] << "\n\n";
                        }

                        // printVarconstMap(F);


                        if(auto v = dyn_cast<ConstantInt>(var)){
                            blockMap[BB][(Value*)&I] = v;
                        }
                        else if (blockMap[BB].find(var) != blockMap[BB].end()) {
                            auto *val = blockMap[BB][var];
                            blockMap[BB][(Value*)&I] = val ;
                        }
                        else {
                            blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "TOP");
                        }

                        break;
                    }
                    
                    case Instruction::Store : {
                        auto *var = I.getOperand(1);
                        auto *val = I.getOperand(0);

                        if(auto v = dyn_cast<ConstantInt>(val)){
                            blockMap[BB][var] = v;
                            // errs () << "var:" << var->getName() << " , ";
                            // errs () << "\nconst value at store: " << *v << "\n";
                        }
                        // Oh hi, kid.
                        else if (blockMap[BB].find(val) != blockMap[BB].end()) {
                            //update the value of var in blockMap
                            auto *bb_val = blockMap[BB][val];
                            blockMap[BB][var] = bb_val ;
                        }
                        else {
                            blockMap[BB][var] = ConstantDataArray::getString(BB->getContext(), "TOP");
                        }
                        break;
                    }

                    case Instruction::Add :{
                        auto *op1 = I.getOperand(0);
                        auto *op2 = I.getOperand(1);

                        auto  a1 = dyn_cast<ConstantInt>(op1);
                        auto  a2 = dyn_cast<ConstantInt>(op2);


                        auto *instruct = &I;


                        if(!instruct->hasName()){
                         // convert I to string
                
                            std::string InstString;
                            llvm::raw_string_ostream InstStream(InstString);

                            // Print the instruction to the string stream
                            instruct->print(InstStream);

                            // Extract the instruction string from the string stream
                            std::string InstText = InstStream.str();

                            // Print the instruction string
                            // errs() << InstText << "\n";
                            size_t equalPos = InstText.find("=");
                        

                            std::string LHSText = InstText.substr(0, equalPos);

                            size_t perPos = LHSText.find("%");

                            LHSText = LHSText.substr(perPos+1, LHSText.length()-perPos-1);

                            // errs() << LHSText << "\n";
                            varConstMAp[instruct] = LHSText;

                            // errs() << "varconstMap[ " << I <<" ]" << varConstMAp[instruct] << "\n\n";
                        }



                        if(a1 && a2) {
                            auto *result = ConstantInt::get(a1->getContext(), a1->getValue() + a2->getValue());
                            blockMap[BB][(Value*)&I] = result;
                        }
                        else if ( a1  ) {
                            if(blockMap[BB].find(op2) != blockMap[BB].end()) {
                                auto *str = dyn_cast<ConstantDataArray>(blockMap[BB][op2]) ;

                                if(!str || !str->isString()){
                                    // str is constant
                                    auto * result = ConstantExpr::getAdd(a2, dyn_cast<Constant>(blockMap[BB][op2]));
                                    // auto *result = ConstantInt::get(a1->getContext(), a1->getValue() + dyn_cast<ConstantInt>(blockMap[BB][op2])->getValue());
                                    blockMap[BB][(Value*)&I] = result;
                                }
                                else if (str->isString() && str->getAsCString() == "BOTTOM") {
                                    blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "BOTTOM");
                                }else if(str->isString() && str->getAsCString() == "TOP"){
                                    blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "TOP");
                                }
                            }else{
                                blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "TOP");
                            }
                        }else if (a2 ) {
                            if(blockMap[BB].find(op1) != blockMap[BB].end()) {
                                auto *str = dyn_cast<ConstantDataArray>(blockMap[BB][op1]) ;
                                if(!str || !str->isString()) {
                                    // errs() << "NOT A STRING\n";
                                    // auto *result = ConstantInt::get(a1->getContext(), a2->getValue() + dyn_cast<ConstantInt>(blockMap[BB][op1])->getValue());
                                    auto * result = ConstantExpr::getAdd(a2, dyn_cast<Constant>(blockMap[BB][op1]));
                                    blockMap[BB][(Value*)&I] = result;
                                }
                                else if (str->isString() && str->getAsCString() == "BOTTOM") {
                                    // errs() << "IF\n";
                                    blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "BOTTOM");
                                }
                                else if(str->isString() && str->getAsCString() == "TOP"){
                                    // errs() << "EIF\n";
                                    blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "TOP");
                                }
                                
                            }else{
                                blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "TOP");
                            }
                        }else{
                            // both are not immediates
                            if(blockMap[BB].find(op1) != blockMap[BB].end() && blockMap[BB].find(op2) != blockMap[BB].end()) {
                                auto *str1 = dyn_cast<ConstantDataArray>(blockMap[BB][op1]) ;
                                auto *str2 = dyn_cast<ConstantDataArray>(blockMap[BB][op2]) ;
                                
                                if ( (str1->isString() && str1->getAsCString() == "BOTTOM") || (str2->isString() && str2->getAsCString() == "BOTTOM")) {
                                    blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "BOTTOM");
                                }else if((str1->isString() && str1->getAsCString() == "TOP") && (str2->isString() && str2->getAsCString() == "TOP")){
                                    blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "TOP");
                                }else{
                                    //not needed ig
                                    // auto *result = ConstantInt::get(a1->getContext(), dyn_cast<ConstantInt>(blockMap[BB][op1])->getValue() + dyn_cast<ConstantInt>(blockMap[BB][op2])->getValue());
                                    auto * result = ConstantExpr::getAdd(blockMap[BB][op1], dyn_cast<Constant>(blockMap[BB][op2]));

                                    blockMap[BB][(Value*)&I] = result;
                                }
                            }
                        }

                        break;
                        
                    }

                    case Instruction::Sub :{
                        auto *op1 = I.getOperand(0);
                        auto *op2 = I.getOperand(1);

                        auto  a1 = dyn_cast<ConstantInt>(op1);
                        auto  a2 = dyn_cast<ConstantInt>(op2);



                        auto *instruct = &I;


                        if(!instruct->hasName()){
                         // convert I to string
                
                            std::string InstString;
                            llvm::raw_string_ostream InstStream(InstString);

                            // Print the instruction to the string stream
                            instruct->print(InstStream);

                            // Extract the instruction string from the string stream
                            std::string InstText = InstStream.str();

                            // Print the instruction string
                            // errs() << InstText << "\n";
                            size_t equalPos = InstText.find("=");
                        

                            std::string LHSText = InstText.substr(0, equalPos);

                            size_t perPos = LHSText.find("%");

                            LHSText = LHSText.substr(perPos+1, LHSText.length()-perPos-1);

                            // errs() << LHSText << "\n";
                            varConstMAp[instruct] = LHSText;

                            // errs() << "varconstMap[ " << I <<" ]" << varConstMAp[instruct] << "\n\n";
                        }

                        if(a1 && a2) {
                            auto *result = ConstantInt::get(a1->getContext(), a1->getValue() - a2->getValue());
                            blockMap[BB][(Value*)&I] = result;
                        }
                        else if ( a1  ) {
                            if(blockMap[BB].find(op2) != blockMap[BB].end()) {
                                auto *str = dyn_cast<ConstantDataArray>(blockMap[BB][op2]) ;

                                if(!str || !str->isString()){
                                    // str is constant
                                    auto * result = ConstantExpr::getSub(a2, dyn_cast<Constant>(blockMap[BB][op1]));

                                    // auto *result = ConstantInt::get(a1->getContext(), a1->getValue() - dyn_cast<ConstantInt>(blockMap[BB][op2])->getValue());
                                    blockMap[BB][(Value*)&I] = result;
                                }
                                else if (str->isString() && str->getAsCString() == "BOTTOM") {
                                    blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "BOTTOM");
                                }else if(str->isString() && str->getAsCString() == "TOP"){
                                    blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "TOP");
                                }
                                
                            }else{
                                blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "TOP");
                            }
                        }else if (a2 ) {
                            if(blockMap[BB].find(op1) != blockMap[BB].end()) {
                                auto *str = dyn_cast<ConstantDataArray>(blockMap[BB][op1]) ;


                                if(!str || !str->isString()){
                                    auto * result = ConstantExpr::getSub(a2, dyn_cast<Constant>(blockMap[BB][op1]));
                                    // auto *result = ConstantInt::get(a1->getContext(), a2->getValue() - dyn_cast<ConstantInt>(blockMap[BB][op1])->getValue());
                                    blockMap[BB][(Value*)&I] = result;
                                }
                                else if (str->isString() && str->getAsCString() == "BOTTOM") {
                                    // errs() << "IF\n";
                                    blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "BOTTOM");
                                }else if(str->isString() && str->getAsCString() == "TOP"){
                                    // errs() << "EIF\n";
                                    blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "TOP");
                                }

                                // errs() << blockMap[BB][(Value*)&I] << "\n";
                                
                            }else{
                                // errs() << "ElSe\n";
                                blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "TOP");
                            }
                        }else{
                            // both are not constant
                            if(blockMap[BB].find(op1) != blockMap[BB].end() && blockMap[BB].find(op2) != blockMap[BB].end()) {
                                auto *str1 = dyn_cast<ConstantDataArray>(blockMap[BB][op1]) ;
                                auto *str2 = dyn_cast<ConstantDataArray>(blockMap[BB][op2]) ;
                                
                                if ( (str1->isString() && str1->getAsCString() == "BOTTOM") || (str2->isString() && str2->getAsCString() == "BOTTOM")) {
                                    blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "BOTTOM");
                                }else if((str1->isString() && str1->getAsCString() == "TOP") && (str2->isString() && str2->getAsCString() == "TOP")){
                                    blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "TOP");
                                }else{
                                    //not needed ig
                                    auto * result = ConstantExpr::getSub(a2, dyn_cast<Constant>(blockMap[BB][op1]));

                                    // auto *result = ConstantInt::get(a1->getContext(), dyn_cast<ConstantInt>(blockMap[BB][op1])->getValue() +-dyn_cast<ConstantInt>(blockMap[BB][op2])->getValue());
                                    blockMap[BB][(Value*)&I] = result;
                                }
                            }
                        }

                        break;
                        
                    }

                    case Instruction::Mul :{



                        auto *instruct = &I;


                        if(!instruct->hasName()){
                         // convert I to string
                
                            std::string InstString;
                            llvm::raw_string_ostream InstStream(InstString);

                            // Print the instruction to the string stream
                            instruct->print(InstStream);

                            // Extract the instruction string from the string stream
                            std::string InstText = InstStream.str();

                            // Print the instruction string
                            // errs() << InstText << "\n";
                            size_t equalPos = InstText.find("=");
                        

                            std::string LHSText = InstText.substr(0, equalPos);

                            size_t perPos = LHSText.find("%");

                            LHSText = LHSText.substr(perPos+1, LHSText.length()-perPos-1);

                            // errs() << LHSText << "\n";
                            varConstMAp[instruct] = LHSText;

                            // errs() << "varconstMap[ " << I <<" ]" << varConstMAp[instruct] << "\n\n";
                        }

                        auto *op1 = I.getOperand(0);
                        auto *op2 = I.getOperand(1);

                        auto  a1 = dyn_cast<ConstantInt>(op1);
                        auto  a2 = dyn_cast<ConstantInt>(op2);

                        if(a1 && a2) {
                            auto *result = ConstantInt::get(a1->getContext(), a1->getValue() * a2->getValue());
                            blockMap[BB][(Value*)&I] = result;
                        }
                        else if ( a1  ) {
                            if(blockMap[BB].find(op2) != blockMap[BB].end()) {
                                auto *str = dyn_cast<ConstantDataArray>(blockMap[BB][op2]) ;


                                if(!str || !str->isString()){
                                    // str is constant
                                    auto * result = ConstantExpr::getMul(a2, dyn_cast<Constant>(blockMap[BB][op1]));

                                    // auto *result = ConstantInt::get(a1->getContext(), a1->getValue() * dyn_cast<ConstantInt>(blockMap[BB][op2])->getValue());
                                    blockMap[BB][(Value*)&I] = result;
                                }
                                else if (str->isString() && str->getAsCString() == "BOTTOM") {
                                    blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "BOTTOM");
                                }else if(str->isString() && str->getAsCString() == "TOP"){
                                    blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "TOP");
                                }
                                
                            }else{
                                blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "TOP");
                            }
                        }else if (a2 ) {
                            if(blockMap[BB].find(op1) != blockMap[BB].end()) {
                                auto *str = dyn_cast<ConstantDataArray>(blockMap[BB][op1]) ;

                                if(!str || !str->isString()){
                                    auto * result = ConstantExpr::getMul(a2, dyn_cast<Constant>(blockMap[BB][op1]));
                                    // auto *result = ConstantInt::get(a1->getContext(), a2->getValue() * dyn_cast<ConstantInt>(blockMap[BB][op1])->getValue());
                                    blockMap[BB][(Value*)&I] = result;
                                }
                                else if (str->isString() && str->getAsCString() == "BOTTOM") {
                                    blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "BOTTOM");
                                }else if(str->isString() && str->getAsCString() == "TOP"){
                                    blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "TOP");
                                }
                                
                            }else{
                                blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "TOP");
                            }
                        }else{
                            // both are not constant
                            if(blockMap[BB].find(op1) != blockMap[BB].end() && blockMap[BB].find(op2) != blockMap[BB].end()) {
                                auto *str1 = dyn_cast<ConstantDataArray>(blockMap[BB][op1]) ;
                                auto *str2 = dyn_cast<ConstantDataArray>(blockMap[BB][op2]) ;
                                
                                if ( (str1->isString() && str1->getAsCString() == "BOTTOM") || (str2->isString() && str2->getAsCString() == "BOTTOM")) {
                                    blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "BOTTOM");
                                }else if((str1->isString() && str1->getAsCString() == "TOP") && (str2->isString() && str2->getAsCString() == "TOP")){
                                    blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "TOP");
                                }else{
                                    //not needed ig
                                    auto * result = ConstantExpr::getMul(blockMap[BB][op1], dyn_cast<Constant>(blockMap[BB][op2]));

                                    blockMap[BB][(Value*)&I] = result;
                                }
                            }
                        }

                        break;
                        
                    }

                    case Instruction::SDiv :{



                        auto *instruct = &I;


                        if(!instruct->hasName()){
                         // convert I to string
                
                            std::string InstString;
                            llvm::raw_string_ostream InstStream(InstString);

                            // Print the instruction to the string stream
                            instruct->print(InstStream);

                            // Extract the instruction string from the string stream
                            std::string InstText = InstStream.str();

                            // Print the instruction string
                            // errs() << InstText << "\n";
                            size_t equalPos = InstText.find("=");
                        

                            std::string LHSText = InstText.substr(0, equalPos);

                            size_t perPos = LHSText.find("%");

                            LHSText = LHSText.substr(perPos+1, LHSText.length()-perPos-1);

                            // errs() << LHSText << "\n";
                            varConstMAp[instruct] = LHSText;

                            // errs() << "varconstMap[ " << I <<" ]" << varConstMAp[instruct] << "\n\n";
                        }
                        auto *op1 = I.getOperand(0);
                        auto *op2 = I.getOperand(1);

                        auto  a1 = dyn_cast<ConstantInt>(op1);
                        auto  a2 = dyn_cast<ConstantInt>(op2);

                        if(a1 && a2) {
                            auto *result = ConstantInt::get(a1->getContext(), a1->getValue().sdiv(a2->getValue()));
                            blockMap[BB][(Value*)&I] = result;
                        }
                        else if ( a1  ) {
                            if(blockMap[BB].find(op2) != blockMap[BB].end()) {
                                auto *str = dyn_cast<ConstantDataArray>(blockMap[BB][op2]) ;

                                if(!str || !str->isString()){
                                    auto *result = ConstantInt::get(a1->getContext(), a1->getValue().sdiv(dyn_cast<ConstantInt>(blockMap[BB][op2])->getValue()));
                                    blockMap[BB][(Value*)&I] = result;
                                }
                                
                                else if (str->isString() && str->getAsCString() == "BOTTOM") {
                                    blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "BOTTOM");
                                }else if(str->isString() && str->getAsCString() == "TOP"){
                                    blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "TOP");
                                }
                                
                            }else{
                                blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "TOP");
                            }
                        }else if (a2 ) {
                            if(blockMap[BB].find(op1) != blockMap[BB].end()) {
                                auto *str = dyn_cast<ConstantDataArray>(blockMap[BB][op1]) ;

                                if(!str || !str->isString()){
                                    auto *result = ConstantInt::get(a1->getContext(), a2->getValue().sdiv(dyn_cast<ConstantInt>(blockMap[BB][op1])->getValue()));
                                    blockMap[BB][(Value*)&I] = result;
                                }
                                else if (str->isString() && str->getAsCString() == "BOTTOM") {
                                    blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "BOTTOM");
                                }else if(str->isString() && str->getAsCString() == "TOP"){
                                    blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "TOP");
                                }
                                
                            }else{
                                blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "TOP");
                            }
                        }else{
                            // both are not constant
                            if(blockMap[BB].find(op1) != blockMap[BB].end() && blockMap[BB].find(op2) != blockMap[BB].end()) {
                                auto *str1 = dyn_cast<ConstantDataArray>(blockMap[BB][op1]) ;
                                auto *str2 = dyn_cast<ConstantDataArray>(blockMap[BB][op2]) ;
                                
                                if ( (str1->isString() && str1->getAsCString() == "BOTTOM") || (str2->isString() && str2->getAsCString() == "BOTTOM")) {
                                    blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "BOTTOM");
                                }else if((str1->isString() && str1->getAsCString() == "TOP") && (str2->isString() && str2->getAsCString() == "TOP")){
                                    blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "TOP");
                                }else{
                                    //not needed ig
                                    auto *result = ConstantInt::get(a1->getContext(), dyn_cast<ConstantInt>(blockMap[BB][op1])->getValue().sdiv(dyn_cast<ConstantInt>(blockMap[BB][op2])->getValue()));
                                    blockMap[BB][(Value*)&I] = result;
                                }
                            }
                        }
                        break;   
                    }
           
                    case Instruction::SRem :{


                        auto *instruct = &I;


                        if(!instruct->hasName()){
                         // convert I to string
                
                            std::string InstString;
                            llvm::raw_string_ostream InstStream(InstString);

                            // Print the instruction to the string stream
                            instruct->print(InstStream);

                            // Extract the instruction string from the string stream
                            std::string InstText = InstStream.str();

                            // Print the instruction string
                            // errs() << InstText << "\n";
                            size_t equalPos = InstText.find("=");
                        

                            std::string LHSText = InstText.substr(0, equalPos);

                            size_t perPos = LHSText.find("%");

                            LHSText = LHSText.substr(perPos+1, LHSText.length()-perPos-1);

                            // errs() << LHSText << "\n";
                            varConstMAp[instruct] = LHSText;

                            // errs() << "varconstMap[ " << I <<" ]" << varConstMAp[instruct] << "\n\n";
                        }

                        auto *op1 = I.getOperand(0);
                        auto *op2 = I.getOperand(1);

                        auto  a1 = dyn_cast<ConstantInt>(op1);
                        auto  a2 = dyn_cast<ConstantInt>(op2);

                        if(a1 && a2) {
                            auto *result = ConstantInt::get(a1->getContext(), a1->getValue().srem(a2->getValue()));
                            blockMap[BB][(Value*)&I] = result;
                        }
                        else if ( a1  ) {
                            if(blockMap[BB].find(op2) != blockMap[BB].end()) {
                                auto *str = dyn_cast<ConstantDataArray>(blockMap[BB][op2]) ;

                                if(!str || !str->isString()){
                                    auto *result = ConstantInt::get(a1->getContext(), a1->getValue().srem(dyn_cast<ConstantInt>(blockMap[BB][op2])->getValue()));
                                    blockMap[BB][(Value*)&I] = result;
                                }
                                else if (str->isString() && str->getAsCString() == "BOTTOM") {
                                    blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "BOTTOM");
                                }else if(str->isString() && str->getAsCString() == "TOP"){
                                    blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "TOP");
                                }
                            }else{
                                blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "TOP");
                            }
                        }else if (a2 ) {
                            if(blockMap[BB].find(op1) != blockMap[BB].end()) {
                                auto *str = dyn_cast<ConstantDataArray>(blockMap[BB][op1]) ;


                                if(!str || !str->isString()){
                                    auto *result = ConstantInt::get(a1->getContext(), a2->getValue().srem(dyn_cast<ConstantInt>(blockMap[BB][op1])->getValue()));
                                    blockMap[BB][(Value*)&I] = result;
                                }
                                
                                else if (str->isString() && str->getAsCString() == "BOTTOM") {
                                    blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "BOTTOM");
                                }else if(str->isString() && str->getAsCString() == "TOP"){
                                    blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "TOP");
                                }
                            }else{
                                blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "TOP");
                            }
                        }else{
                            // both are not constant
                            if(blockMap[BB].find(op1) != blockMap[BB].end() && blockMap[BB].find(op2) != blockMap[BB].end()) {
                                auto *str1 = dyn_cast<ConstantDataArray>(blockMap[BB][op1]) ;
                                auto *str2 = dyn_cast<ConstantDataArray>(blockMap[BB][op2]) ;
                                
                                if ( (str1->isString() && str1->getAsCString() == "BOTTOM") || (str2->isString() && str2->getAsCString() == "BOTTOM")) {
                                    blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "BOTTOM");
                                }else if((str1->isString() && str1->getAsCString() == "TOP") && (str2->isString() && str2->getAsCString() == "TOP")){
                                    blockMap[BB][(Value*)&I] = ConstantDataArray::getString(BB->getContext(), "TOP");
                                }else{
                                    //not needed ig
                                    auto *result = ConstantInt::get(a1->getContext(), dyn_cast<ConstantInt>(blockMap[BB][op1])->getValue().srem(dyn_cast<ConstantInt>(blockMap[BB][op2])->getValue()));
                                    blockMap[BB][(Value*)&I] = result;
                                }
                            }
                        }
                        break; 
                    }

                    default:
                        break;
                    }
                // errs() << I << " DONE" << "\n";

                instMap[&I] = constantMap(blockMap[BB]);

                // errs() << I << " copied" << "\n";

            } 

            if(blockMap[BB] != old){
                for (BasicBlock* SuccBB : successors(BB)) {
                    basicBlocks.insert(SuccBB);
                }
            }
        }
    // printInstMap();

    // printCorrectFunc(F);

        printWrongFunc(F);

        // printVarconstMap(F);

		return false;
	}
	
    void printInstMap(){
        int non_var =0;
        for (auto it = instMap.begin(); it != instMap.end(); it++) {
            Instruction* I = it->first;
            constantMap& cm = it->second;
            errs() << *I << "-->";

            for(auto &it : cm){
                Value * v = it.first;
                if(v->getName().empty()){
                    errs() << "%" << non_var << " : ";
                    non_var++;
                }else{
                    errs() << "%" << v->getName() << " : " ;
                
                }
                Constant * c = it.second;
                if(auto str = dyn_cast<ConstantDataArray>(c)){
                    if (str->isString() && str->getAsCString() == "BOTTOM") {
                        errs() << "BOTTOM, ";
                    }else if(str->isString() && str->getAsCString() == "TOP"){
                        errs() << "TOP, ";
                    } 
                }
                else{
                    errs() << *c << ", ";
                }

            }
            errs() << "\n";
        }
    }

    void printCorrectFunc(Function &F){
        int non_var=0;
        for (auto &BB : F)
        {
            errs() << BB.getName() << " : \n";

            for(auto &I : BB){

                errs () << I << " --> ";
                // auto ops = I.getOperandList();
                // for(auto op = ops.begin(); op != ops.end(); op++){
                //     errs() << *op << ", ";
                // }

                    // ops->getNext();

                if(instMap.find(&I) != instMap.end()){

                    constantMap cm = instMap[&I];

                    for(auto &it : cm){
                        Value* v = it.first;

                        if(v->getName().empty()){
                            errs() << "%" << non_var << " : ";
                            non_var++;
                        }else{
                            errs() << "%" << v->getName() << " : " ;
                        }

                        if(auto str = dyn_cast<ConstantDataArray>(it.second)){
                            if (str->isString() && str->getAsCString() == "BOTTOM") {
                                errs() << "BOTTOM, ";
                            }else if(str->isString() && str->getAsCString() == "TOP"){
                                errs() << "TOP, ";
                            } 
                        }
                        else{
                            errs() << *it.second << ", ";
                        }
                    }
                    errs() << "\n";

                } else {
                    errs() << "No entry for this instruction\n";
                }
            }
            errs() << "\n";
        }


    }
    
    void printVarconstMap(Function &F){
        for (auto &BB : F)
        {
            errs() << BB.getName() << " : \n";

            for(auto &I : BB){
                if(varConstMAp.find(&I) != varConstMAp.end()){
                    errs() << I << " : " << varConstMAp[&I] << "\n";
                }
            }
        }
    }
    }; // end of struct constant_prop
		 
}  // end of anonymous namespace


char constant_prop::ID = 0;
static RegisterPass<constant_prop> X("libCP_given", "Constant Propagation Pass for Assignment",
										 false /* Only looks at CFG */,
										 false /* Analysis Pass */);

