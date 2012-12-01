// Author: Junyang

#ifndef __DYN_AA_TRACE_SLICER_H
#define __DYN_AA_TRACE_SLICER_H

#include <utility>
#include <vector>

#include "llvm/Pass.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/Analysis/AliasAnalysis.h"

#include "rcs/typedefs.h"

#include "dyn-aa/LogRecord.h"
#include "dyn-aa/LogProcessor.h"
#include "dyn-aa/Utils.h"

using namespace std;
using namespace llvm;

namespace dyn_aa {
struct LogRecordInfo{
  LogRecordInfo(): Active(false), ArgNo(-1) {}
  // for MissingAliasClassifier
  bool Active;
  bool RecordID;
  unsigned ValueID;
  // for CallInstRecord and Argument
  // ArgNo != -1 indicates a CallInstRecord
  // ArgNo = -1 indicates a TopLevelPointToRecord of CallSite type
  int ArgNo;
  // for LoadInst and StoreInst
  void *PointerAddress;
  // for PHI and SelectInst
  void *PointeeAddress;
};

struct PointerTrace{
  PointerTrace(): Active(false) {}

  unsigned StartingRecordID;
  Function *StartingFunction;
  bool Active;
  LogRecordInfo PreviousRecord;
  vector<pair<unsigned, unsigned> > Slice;
};

struct TraceSlicer: public ModulePass, public LogProcessor {
  static char ID;

  TraceSlicer(): ModulePass(ID) {}
  virtual bool runOnModule(Module &M);
  virtual void getAnalysisUsage(AnalysisUsage &AU) const;
  virtual void print(raw_ostream &O, const Module *M) const;

  // Interfaces of LogProcessor.
  void processMemAlloc(const MemAllocRecord &Record);
  void processTopLevel(const TopLevelRecord &Record);
  void processStore(const StoreRecord &Record);
  void processCall(const CallRecord &Record);
  void processReturn(const ReturnRecord &Record);

  pair<bool, bool> dependsOn(LogRecordInfo &R1, LogRecordInfo &R2);
  static bool isCalledFunction(Function *F, CallSite CS);
  Value *getLatestCommonAncestor();

 private:
  void printTrace(raw_ostream &O,
                  pair<unsigned, unsigned> TraceRecord,
                  int PointerLabel) const;

  PointerTrace Trace[2];
  unsigned CurrentRecordID;
};
}

#endif
