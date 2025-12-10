#include "compilation_engine.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }
    
    std::string inputFilename = argv[1];
    std::string outputFilename = inputFilename.substr(0, inputFilename.find_last_of(".")) + ".xml";
    
    CompilationEngine engine(inputFilename, outputFilename);
    engine.compileClass();
    
    return 0;
}
