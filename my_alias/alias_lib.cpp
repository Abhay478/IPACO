#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/InstIterator.h"
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
	// typedef DenseMap LocalMap;
	// typedef DenseSet LocalSet;
	#define LocalMap std::map
	#define LocalSet std::set
	typedef LocalSet<Value *> VSet;
	typedef LocalMap<Value *, VSet> PTS;

	struct BlockStructs {
		PTS in;
		PTS out;
		// PTS def;
		LocalSet<Value *> kill;
		PTS clones;
	};


	typedef LocalMap<BasicBlock *, BlockStructs> BlockMap; // Used for IN and OUT
	// typedef DenseMap<Instruction *, Set<Value *>> InstructionMap; // Used for storing aliases at program points.

	struct alias_c : public FunctionPass {
		static char ID;
		alias_c() : FunctionPass(ID) {}

		bool runOnFunction(Function &F) override {

			auto worklist = LocalSet<BasicBlock *>();
			auto in_out = BlockMap();

			for(auto &BB : F) {
				worklist.insert(&BB);
				in_out[&BB] = BlockStructs();
			}

			while(!worklist.empty()) {
				auto BB = *worklist.begin();
				worklist.erase(BB); 
				
				if(!in_out.count(BB)) {
					in_out[BB] = BlockStructs(); // First time visiting a block
				} 
				// auto pts = in_out[BB];
				auto bs = BlockStructs(); // temporary copy, to be compared with saved version in the map.

				for(auto P = pred_begin(BB); P != pred_end(BB); P++) {
					auto pred = *P;
					// if(!in_out.count(pred)) {
					// 	worklist.insert(pred);
					// 	continue;
					// }
					auto pred_out = in_out[pred].out;
					for(auto it = pred_out.begin(); it != pred_out.end(); it++) {
						auto v = it->first;
						auto s = it->second;
						if(!bs.in.count(v)) {
							bs.in[v] = s;
						} else {
							bs.in[v].insert(s.begin(), s.end()); // Union of sets
						}
					}

				}

				for (auto &I: *BB) {
					auto *inst = &I;

					switch(inst->getOpcode()) {
						// x = &y
						case Instruction::Alloca: {
							bs.out[(Value *)inst] = LocalSet<Value *>(); // Empty set.
						}
						break;
						// *x = y
						case Instruction::Store: {
							auto *ptr = I.getOperand(1);
							auto *val = I.getOperand(0);
							if(val->getType()->isPointerTy()) {
								for(auto pin : bs.in[ptr]) {
									for(auto ypin : bs.in[val]) {
										bs.out[pin].insert(ypin);
									}
								}
							}
							if(bs.in[ptr].size() == 1) {
								// Strong update
								bs.kill.insert(ptr);
							} else if(bs.in[ptr].size() == 0 && bs.out[ptr].size() == 1) {
								// Strong update
								// bs.kill.insert(ptr);
								bs.out[ptr].erase(*bs.out[ptr].begin());
							}
							bs.out[ptr].insert(val);
							for(auto q: bs.clones[ptr]) {
								bs.out[q].insert(val);
							}
							// for(auto q: bs.clones[val]) {
							// 	bs.out[ptr].insert(q);
							// }
							if(bs.clones[ptr].size() == 1) {
								auto lc = *bs.clones[ptr].begin();
								bs.out[lc].clear();
								bs.out[lc].insert(val);
							}
						}
						break;
						// x = *y
						case Instruction::Load: {
							if(!inst->getType()->isPointerTy()) {
								break;
							}
							// errs() << "Load" << inst->getOpcode() << "\n";
							auto *ptr = I.getOperand(0);
							// bs.kill.insert(inst);
							bs.out[(Value *)inst] = LocalSet<Value *>(); // Empty set.
							bs.clones[(Value *)inst] = LocalSet<Value *>();
							for(auto pin : bs.out[ptr]) {
								for(auto ypin : bs.out[pin]) {
									bs.out[(Value *)inst].insert(ypin);
								}
								bs.clones[(Value *)inst].insert(pin);
							}
						}
						break;

						// Copies and casts
						case Instruction::BitCast: {}
						case Instruction::GetElementPtr: {
							// ??
							// bs.out[(Value *)inst] = LocalSet<Value *>(); // Empty set.
							// for(auto pin: bs.in[inst->getOperand(0)]) {
							// 	bs.out[(Value *)inst].insert(pin);
							// }
							// bs.clones[(Value *)inst] = LocalSet<Value *>();
							// bs.clones[(Value *)inst].insert(inst->getOperand(0));
							bs.out[inst->getOperand(0)].insert(inst);
						}
						break;
					}
				}

				/* for(auto it: bs.def){
					bs.out[it.first] = DenseSet<Value *>(it.second);
					errs() << "Def: " << *it.first << " -> ";
					for(auto v: it.second) {
						errs() << *v << ", ";
					}
					errs() << "\n";
				} */
				// bs.out.insert(bs.def.begin(), bs.def.end());
				
				for(auto it: bs.in){
					if(!bs.kill.count(it.first)) {
						bs.out[it.first].insert(it.second.begin(), it.second.end());
					}
				}

				if(bs.in != in_out[BB].in || bs.out != in_out[BB].out) {
					in_out[BB] = bs;
					for(auto S = succ_begin(BB); S != succ_end(BB); S++) {
						// errs() << "Diff" << "\n";
						worklist.insert(*S);
					}
				}
				// auto things = in_out[BB];
				// errs() << "Block: " << BB->getName() << "\n";
				// for(auto q: bs.out) {
				// 	errs() << "OUT: " << *q.first << " -> ";
				// 	for(auto v: q.second) {
				// 		errs() << *v << ", ";
				// 	}
				// 	errs() << "\n";
				// }
			}

			/* for(auto q: in_out) {
				errs() << "Block: " << q.first->getName() << "\n";
				errs() << "IN: \n";
				for(auto p: q.second.in) {
					errs() << "  " << *p.first << " -> ";
					if(p.second.empty()) {
						errs() << "Unknown\n";
					} else {
						for(auto v: p.second) {
							errs() << *v << ", ";
						}
						errs() << "\n";
					}
				}
				errs() << "OUT: \n";
				for(auto p: q.second.out) {
					errs() << "  " << *p.first << " -> ";
					if(p.second.empty()) {
						errs() << "Unknown\n";
					} else {
						for(auto v: p.second) {
							errs() << *v << ", ";
						}
						errs() << "\n";
					}
				}
			}
			 */
			PTS aliases;
			/* 
			for(auto eh: in_out) {
				aliases[eh.first] = LocalSet<Value *>();
			}
			
			for(auto x: in_out) {
				for(auto y: in_out) {
					if(x.first == y.first) {
						continue;
					}
					for(auto p: x.second.out) {
						for(auto q: y.second.out) {
							if(p.second.count(q.first)) {
								// errs() << "Alias: " << *p.first << " " << *q.first << "\n";
								aliases[p.first].insert(q.first);
								aliases[q.first].insert(p.first);
								goto L1;
							}
						}
					}
					L1: ;
				}
			}
			

			while(!list.empty()) {
				auto v = list.begin();
				list.erase(v);

				for(auto BB: in_out) {

				}
			}
			*/
			
			for(auto &BB: F) {
				for(auto v: in_out[&BB].out) {
					if(!aliases.count(v.first)) aliases[v.first] = LocalSet<Value *>();
					for(auto u: in_out[&BB].out) {
						if(!aliases.count(u.first)) aliases[u.first] = LocalSet<Value *>();
						if(v.first == u.first) {
							continue;
						}
						for(auto x: v.second) {
							for(auto y: u.second) {
								if(x == y) {
									aliases[v.first].insert(u.first);
									aliases[u.first].insert(v.first);
								}
							}
						}
					}
				}
				for(auto v: in_out[&BB].clones) {
					if(!aliases.count(v.first)) aliases[v.first] = LocalSet<Value *>();
					for(auto u: v.second) {
						if(!aliases.count(u)) aliases[u] = LocalSet<Value *>();
						aliases[v.first].insert(u);
						aliases[u].insert(v.first);
					}
				}
			}

			for(auto q: aliases) {
				if(!q.first->hasName() || 
					!(q.first->getType()->isPointerTy() || q.first->getType()->isArrayTy()) || 
					!(q.first->getType()->getContainedType(0)->isPointerTy() || 
						q.first->getType()->getContainedType(0)->isArrayTy())) {
					continue;
				}
				errs() << q.first->getName() << " -> {";
				for(auto v: q.second) {
					if(!v->hasName()) {
						continue;
					}
					errs() << v->getName() << ", ";
				}
				errs() << "}\n";
			}
			return false;
		}
	}; // end of struct alias_c
}  // end of anonymous namespace

char alias_c::ID = 0;
static RegisterPass<alias_c> X("alias_lib_given", "Alias Analysis Pass for Assignment",
							 false /* Only looks at CFG */,
							 false /* Analysis Pass */);
