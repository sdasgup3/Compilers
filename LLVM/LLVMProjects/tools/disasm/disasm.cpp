#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Error.h"
#include "llvm/Object/Binary.h"
#include "llvm/Object/ObjectFile.h"
#include "llvm/Support/Signals.h"
#include "llvm/DebugInfo/DIContext.h"
#include "llvm/DebugInfo/DWARF/DWARFContext.h"
#include "llvm/DebugInfo/DWARF/DWARFDie.h"
#include "llvm/DebugInfo/DWARF/DWARFCompileUnit.h"
#include "llvm/Support/Format.h"
#include "llvm/DebugInfo/DWARF/DWARFFormValue.h"

#include <algorithm>
#include <list>
#include <string>
#include <system_error>

using namespace llvm;
using namespace object;

static cl::list<std::string>
InputFilenames(cl::Positional, cl::desc("<input object files"),
               cl::ZeroOrMore);


static void error(StringRef Filename, std::error_code EC) {
  if (!EC)
    return;
  errs() << Filename << ": " << EC.message() << "\n";
  exit(1);
}

class DwarfVariableFinder final {
public:
  DwarfVariableFinder(const DWARFDie &die) {
    if(!die.hasChildren()) {
      outs() << "No child \n\n";
      return;
    }
        
    for (auto child = die.getFirstChild(); child; child = child.getSibling()) {
      //Go over all the top level sub_programs
      if(child.isSubprogramDIE() || child.isSubroutineDIE()) {
        getInfo(child);

        //Look for variables among children of sub_program die
        if(!child.hasChildren()) {
          continue;
        }
        findVariables(child);
      }
     // outs() << "=============================== \n\n";
    }
  }

  void findVariables(const DWARFDie &die) {
    
    for (auto child = die.getFirstChild(); child; child = child.getSibling()) {
      switch(child.getTag()) {
        case dwarf::DW_TAG_variable:
        case dwarf::DW_TAG_formal_parameter:
        case dwarf::DW_TAG_constant:
          handleVariable(child);
          break;
        default:
          if (child.hasChildren())
            findVariables(child);
      }
    }
  }

  void handleVariable(const DWARFDie &die) {
    getInfo(die);
  }

  void getInfo(const DWARFDie &die) {
    auto tagString = TagString(die.getTag());
    if (tagString.empty())
      outs() << format("DW_TAG_Unknown_%x", die.getTag());
    auto formVal = die.find(dwarf::DW_AT_name);
    formVal->dump(outs());

    //    outs()<< tagString
    //      << "\n\t" << child.getSubroutineName(DINameKind::ShortName);
  }


private:
};

static void DumpObjectFile(ObjectFile &Obj, Twine Filename) {
  std::unique_ptr<DWARFContext> DICtx(new DWARFContextInMemory(Obj));

  outs() << Filename.str() << ":\tfile format " << Obj.getFileFormatName()
         << "\n\n";

  for (const auto &CU : DICtx->compile_units()) {
    const DWARFDie &die = CU->getUnitDIE(false);
    DwarfVariableFinder finder(die);
  }
}

static void DumpInput(StringRef Filename) {
  ErrorOr<std::unique_ptr<MemoryBuffer>> BuffOrErr =
      MemoryBuffer::getFileOrSTDIN(Filename);
  error(Filename, BuffOrErr.getError());
  std::unique_ptr<MemoryBuffer> Buff = std::move(BuffOrErr.get());

  Expected<std::unique_ptr<Binary>> BinOrErr =
      object::createBinary(Buff->getMemBufferRef());
  if (!BinOrErr)
    error(Filename, errorToErrorCode(BinOrErr.takeError()));

  if (auto *Obj = dyn_cast<ObjectFile>(BinOrErr->get())) {
    DumpObjectFile(*Obj, Filename);
  }
}

int main(int argc, char **argv) {
  // Print a stack trace if we signal out.
  sys::PrintStackTraceOnErrorSignal(argv[0]);
  PrettyStackTraceProgram X(argc, argv);
  llvm_shutdown_obj Y;  // Call llvm_shutdown() on exit.

  cl::ParseCommandLineOptions(argc, argv, "llvm dwarf dumper\n");

  // Defaults to a.out if no filenames specified.
  if (InputFilenames.size() == 0)
    InputFilenames.push_back("a.out");

  std::for_each(InputFilenames.begin(), InputFilenames.end(), DumpInput);

  return EXIT_SUCCESS;
}
