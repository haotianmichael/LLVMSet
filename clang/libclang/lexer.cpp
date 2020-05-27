extern "C" {
    #include "/LLVM/llvm-install/include/clang-c/Index.h"
}
#include "/LLVM/llvm-install/include/llvm/Support/CommandLine.h"
#include <iostream>

using namespace llvm;


static cl::opt<std::string>
FileName(cl::Positional, cl::desc("Input file"), cl::Required);

int main(int argc, char** argv)
{
    
    cl::ParseCommandLineOptions(argc, argv, "My tokenizer\n");


    return 0;
}


