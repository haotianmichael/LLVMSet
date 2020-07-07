extern "C" {
    #include "/LLVM/llvm-install/include/clang-c/Index.h"
}

#include "/LLVM/llvm-install/include/llvm/Support/CommandLine.h"
#include <iostream>


using namespace llvm;

static cl::opt<std::string>
    FileName(cl::Positional, cl::desc("Input file"), cl::Required);



enum    CXChildVisitResult visitNode(CXCursor cursor, CXCursor parent, CXClientData client_data) {



    


    return CXChildVisit_Recurse;
}


int main(int argc, char** argv)
{
    
    return 0;
}
