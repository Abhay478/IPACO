#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Analysis/ConstantFolding.h"
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
typedef DenseMap<Value *, Constant *> CMap;

/*** Custom 
 * If top, no entry for value.
 * If bottom, nullptr.
*/

struct BlockStructs {
	// Don't really need this, but used it for debugging and now I don't want to break anything by removing.
	// First rule of programming. ;)
	CMap in;
	CMap out;
};

typedef DenseMap<BasicBlock *, BlockStructs> BlockMap; // Used for IN and OUT
typedef DenseMap<Instruction *, CMap> InstructionMap; // Used for storing constants at program points.
typedef DenseMap<Value *, std::string> UnnamedMap; // Did not know about Value::setName() at the time, and am unwilling to change.

auto unm = UnnamedMap();
std::error_code EC;
raw_fd_ostream my_ostream("output.txt", EC);

struct my_cp : public FunctionPass {
	static char ID;
	
	my_cp() : FunctionPass(ID) {}

	bool runOnFunction(Function &F) override {
		// std::error_code EC;
		// raw_fd_ostream my_ostream(StringRef("cp_lib.txt"), EC);
		// write your code here
		// my_ostream = raw_fd_ostream;

		auto worklist = DenseSet<BasicBlock *>();
		auto in_out = BlockMap();
		auto store = InstructionMap();
		
		// All blocks.
		// Since it is a set, no duplicates.
		// Needed coz if no constants in entry block, will stop.
		for(auto &BB : F) {
			worklist.insert(&BB);
			in_out[&BB] = BlockStructs();
			if(F.arg_empty()) continue;
			for(auto a = F.arg_begin(); a != F.arg_end(); a++) {
				Value * v = &*a;
				in_out[&BB].in[v] = nullptr; // BOTTOM
			}

		}
		// int eh = 0;
		while(!worklist.empty()) {
			auto BB = *worklist.begin();
			worklist.erase(worklist.begin());

			if(!in_out.count(BB)) {
				in_out[BB] = BlockStructs(); // First time visiting a block
			} 
			auto bs = BlockStructs(); // temporary copy, to be compared with saved version in the map.

			// Iterate over predecessors to get IN.
			// my_ostream << "Predecessors: " << "\n";
			for(auto P = pred_begin(BB); P != pred_end(BB); P++) {
				auto *Pred = *P;
				// my_ostream << Pred->getName() ;
				// Pred has been through the pass at least once.
				if(in_out.count(Pred)) { 
					// my_ostream << ": \n";
					auto pred_bs = in_out[Pred];
					for(auto &vc_pair : pred_bs.out) {
						// my_ostream << "\t" << vc_pair.first->getName() << " : " ;
						// if(vc_pair.second) {
						// 	vc_pair.second->print(my_ostream);
						// } else {
						// 	my_ostream << "bottom";
						// }
						// my_ostream << "\n";
						// Meet operation
						if(bs.in.count(vc_pair.first)) { // Not top
							// Predecessor says bottom, or has a different value from current.
							// Current cannot be top, that goes to else.
							// Therefore, either different constants or one is bottom.
							// Set thing to bottom.
							if(!vc_pair.second || bs.in[vc_pair.first] != vc_pair.second) {
								bs.in[vc_pair.first] = nullptr; 
							} else { 
								// Equal. If current says bottom and predecessor says constant, set to constant. 
								// If it really is bottom, will get set to bottom in the next iteration.
								bs.in[vc_pair.first] = vc_pair.second;
							}
						} else { // Set to constant
							bs.in[vc_pair.first] = vc_pair.second;
						}

						// Small problem: What if top in one predecessor and constant in another?
					}


				}
			}

			// Iterate over all local constants.
			for(auto vc_pair: bs.in) {
				// Make sure the constant is not top for ALL predecessors, unless they haven't been through the pass.
				for(auto P = pred_begin(BB); P != pred_end(BB); P++) {
					auto *Pred = *P;
					if(!in_out.count(Pred)) {
						continue;
					}
					auto pred_bs = in_out[Pred];
					if(!pred_bs.out.count(vc_pair.first)) {
						// Our block may have whatever, but predecessor has top. Set to bottom
						bs.in[vc_pair.first] = nullptr;
					}
				}
			}

			auto old = in_out[BB]; // Even if freshly inited.
			bool changed = false;

			// Clone IN into OUT. Other blocks only use OUT, so this is fine.
			for(auto vc_pair : bs.in) {
				bs.out[vc_pair.first] = vc_pair.second;
			}

			// Iterate over instructions to get OUT.
			
			for(auto &I : *BB) {
				auto * inst = &I;
				// inst->print(my_ostream);
				// my_ostream << "\n";
				// Check for load, store, and alloca.
				
				switch(I.getOpcode()) {
					case Instruction::Call:
					case Instruction::Alloca: {
						// my_ostream << "Alloca\n";
						if(inst->getName().empty()) {
							std::string s;
							raw_string_ostream ss(s);
							ss << *inst;
							StringRef sr(ss.str());
							int q = sr.find("=");
							sr = sr.take_front(q - 1);
							sr = sr.drop_front(sr.find("%"));
							
							unm[inst] = sr.str();
						}
						break; // Since top means no entry in map, this is fine
					}
					case Instruction::Load: {
						auto * ptr = I.getOperand(0);
						if(inst->getName().empty()) {
							std::string s;
							raw_string_ostream ss(s);
							ss << *inst;
							StringRef sr(ss.str());
							int q = sr.find("=");
							sr = sr.take_front(q - 1);
							sr = sr.drop_front(sr.find("%"));
							
							unm[inst] = sr.str();
						}

						// If the pointer is constant, then the load is constant.
						if(auto c = dyn_cast<ConstantInt>(ptr)) {
							bs.out[(Value *)inst] = c;
						} else if(bs.out.count(ptr)) {
							bs.out[(Value *)inst] = bs.out[ptr];
						} else {
							bs.out[(Value *)inst] = nullptr;
						}
						break;
					}
					case Instruction::Store: {
						auto * ptr = I.getOperand(1);
						auto * val = I.getOperand(0);

						// Constant literal
						if(auto c = dyn_cast<ConstantInt>(val)) {
							bs.out[ptr] = c;
						} else if(bs.out.count(val)) {
							bs.out[ptr] = bs.out[val];
						} else {
							bs.out[ptr] = nullptr;
						}
						break;
					}
					default: { // Assuming basic binary arithmetic instruction.
						bool con = true;
						// First, check if instruction operands are constant.
						for(auto &V : I.operands()) {
							if(!dyn_cast<Constant>(V) && (!bs.out.count(V) || !bs.out[V])) { // Operand is bottom, or somehow top. Set instruction to bottom.
								bs.out[inst] = nullptr;
								con = false;
							}
						}

						// If all operands are constant, then instruction output is constant. Evaluate it.
						if(con) {
							if(I.getNumOperands() != 2) {
								break;
							}
							// Nested switch, woo!
							auto * op1 = I.getOperand(0);
							auto * op2 = I.getOperand(1);

							auto c1 = dyn_cast<ConstantInt>(op1);
							auto c2 = dyn_cast<ConstantInt>(op2);

							if(c1 && c2) {
								bs.out[(Value *)inst] = ConstantExpr::get(inst->getOpcode(), c1, c2);	
							} else if (c1) {
								if(!bs.out[op2]) {
									bs.out[(Value *)inst] = nullptr;
									break;
								}
								bs.out[(Value *)inst] = ConstantExpr::get(inst->getOpcode(), c1, bs.out[op2]);
							} else if (c2) {
								if(!bs.out[op1]) {
									bs.out[(Value *)inst] = nullptr;
									break;
								}
								bs.out[(Value *)inst] = ConstantExpr::get(inst->getOpcode(), bs.out[op1], c2);
							} else {
								if(!bs.out[op1] || !bs.out[op2]) {
									bs.out[(Value *)inst] = nullptr;
									break;
								}
								bs.out[(Value *)inst] = ConstantExpr::get(inst->getOpcode(), bs.out[op1], bs.out[op2]);
							}
							break;
						}
						
						if(inst->getName().empty()) {
							std::string s;
							raw_string_ostream ss(s);
							ss << *inst;
							StringRef sr(ss.str());
							int q = sr.find("=");
							sr = sr.take_front(q - 1);
							sr = sr.drop_front(sr.find("%"));
							
							unm[inst] = sr.str();
						}
						
						break;
					}
				}
			
				store[inst] = CMap(bs.out); // Store the constants at this program point.
			}
			// Check only once after everything is done.
			// Extra computation, but lyt.
			if(bs.in != old.in || bs.out != old.out) {
				changed = true;
				in_out[BB] = BlockStructs(bs);
			}
			
			// If in or out has changed, add successors to worklist
			if(changed) {
				for(auto *Succ : successors(BB)) {
					worklist.insert(Succ);
				}
			}
		}
		
		// Print the constants at each program point.
		my_ostream << F.getName() << "\n";
		for(auto &BB : F) {
			my_ostream << BB.getName() << ": \n";
			for(auto &I : BB) {
				auto * inst = &I;
				CMap map = store[inst];
				my_ostream << *inst << " --> ";
				
				for(auto &u: inst->operands()) {
					auto v = u.get();
					if(dyn_cast<ConstantInt>(v)) continue;
					if(v->getName().empty()) {
						my_ostream << unm[v] << "=";	
					}
					else {
						my_ostream << "%" << v->getName() << "=";
					}
					if(map.count(v)) {
						auto val = map[v];
						if(val) {
							chop_type(val);
						} else {
							my_ostream << "BOTTOM, ";
						}
					}
					else {
						my_ostream << "TOP, ";
					}
				}


				print_instr(map, inst);

				my_ostream << "\n";

			}
		} 

		return false;
	}

	StringRef get_nth_operand(Instruction * inst, int n) {
		std::string s;
		auto ss = raw_string_ostream(s);
		ss << *inst;
		auto sr = StringRef(ss.str());

		for(int i = 0; i < n - 1; i++) {
			if(dyn_cast<ConstantInt>(inst->getOperand(i))) continue;
			auto u = sr.find("%");
			if(u >= sr.size()) {
				break;
			}
			sr = sr.drop_front(u);
		}
		sr = sr.take_front(sr.find(" ") - 1);

		return sr;
	}

	void chop_type(Constant * val) {
		std::string s;
		raw_string_ostream rso(s);
		rso << *val;
		StringRef sr(rso.str());

		sr = sr.drop_front(4);
		my_ostream << sr << ", ";
	}

	void print_instr(CMap &map, Instruction * inst) {
		switch(inst->getOpcode()) {
			case Instruction::Ret: case Instruction::Store: case Instruction::Br: return;
			default: break;
		}
		Value * inst_val = dyn_cast<Value>(inst);
		if(inst_val->getName().empty()) {
			std::string s;
			raw_string_ostream ss(s);
			ss << *inst;
			StringRef sr(ss.str());
			int q = sr.find("=");
			auto sr1 = sr.take_front(q - 1);
			auto sr2 = sr1.drop_front(sr.find("%"));
			my_ostream << sr2 << "=";	
			unm[inst] = sr2.str();
		}
		else {
			my_ostream << "%" << inst_val->getName() << "=";
		}
		if(map.count(inst_val)) {
			auto val = map[inst_val];
			if(val) {
				chop_type(val);
			} else {
				my_ostream << "BOTTOM, ";
			}
		}
		else {
			my_ostream << "TOP, ";
		}
	}

}; // end of struct my_cp
}  // end of anonymous namespace

char my_cp::ID = 0;
// Changed classname, not passname
static RegisterPass<my_cp> X("libCP_given", "Constant Propagation Pass for Assignment",
														 false /* Only looks at CFG */,
														 false /* Analysis Pass */);
