#include "vm_tran.h"
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

// Helper function to check if path is a directory
bool isDirectory(const char* path) {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) {
        return false;
    }
    return S_ISDIR(path_stat.st_mode);
}

// Helper function to check if filename ends with .vm
bool hasVmExtension(const char* filename) {
    const char* dot = strrchr(filename, '.');
    return (dot && strcmp(dot, ".vm") == 0);
}

// Helper function to extract base name from path (without extension)
void getBaseName(const char* path, char* baseName, size_t size) {
    const char* lastSlash = strrchr(path, '/');
    const char* lastBackslash = strrchr(path, '\\');
    const char* start = path;
    
    if (lastSlash && (!lastBackslash || lastSlash > lastBackslash)) {
        start = lastSlash + 1;
    } else if (lastBackslash) {
        start = lastBackslash + 1;
    }
    
    strncpy(baseName, start, size - 1);
    baseName[size - 1] = '\0';
    
    // Remove .vm extension if present
    char* dot = strrchr(baseName, '.');
    if (dot && strcmp(dot, ".vm") == 0) {
        *dot = '\0';
    }
}

// Process a single .vm file
void processVmFile(CodeWriter* writer, const char* filepath) {
    FILE* input_file = fopen(filepath, "r");
    if (!input_file) {
        fprintf(stderr, "Error: Could not open input file %s\n", filepath);
        return;
    }
    
    // Set the current file name for static variable scoping
    char baseName[128];
    getBaseName(filepath, baseName, sizeof(baseName));
    setFileName(writer, baseName);
    
    char line[256];
    while (fgets(line, sizeof(line), input_file)) {
        if (isCommentOrEmpty(line)) {
            continue;
        }
        VMCommand vm_command;
        parseCommand(line, &vm_command);
        
        writeComment(writer, line);
        
        if (vm_command.type == ARITHMETIC) {
            writeArithmetic(writer, &vm_command);
        } else if (vm_command.type == PUSH || vm_command.type == POP) {
            writePushPop(writer, &vm_command);
        } else if (vm_command.type == LABEL) {
            writeLabel(writer, vm_command.label);
        } else if (vm_command.type == GOTO) {
            writeGoto(writer, vm_command.label);
        } else if (vm_command.type == IF_GOTO) {
            writeIfGoto(writer, vm_command.label);
        } else if (vm_command.type == FUNCTION) {
            writeFunction(writer, vm_command.functionName, vm_command.numArgs);
        } else if (vm_command.type == CALL) {
            writeCall(writer, vm_command.functionName, vm_command.numArgs);
        } else if (vm_command.type == RETURN) {
            writeReturn(writer);
        }
    }
    
    fclose(input_file);
}

// Main file - we take input from command line arguments and process the VM files
int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input.vm or directory>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* input_path = argv[1];
    char output_filename[512];
    bool isDir = isDirectory(input_path);
    
    if (isDir) {
        // For directory: output file is DirectoryName/DirectoryName.asm
        char dirCopy[512];
        strncpy(dirCopy, input_path, sizeof(dirCopy) - 1);
        dirCopy[sizeof(dirCopy) - 1] = '\0';
        
        // Remove trailing slash if present
        size_t len = strlen(dirCopy);
        if (len > 0 && (dirCopy[len-1] == '/' || dirCopy[len-1] == '\\')) {
            dirCopy[len-1] = '\0';
        }
        
        // Get the directory base name
        char dirBaseName[128];
        getBaseName(dirCopy, dirBaseName, sizeof(dirBaseName));
        
        // Create output filename: directory/DirectoryName.asm
        snprintf(output_filename, sizeof(output_filename), "%s/%s.asm", input_path, dirBaseName);
    } else {
        // For single file: replace .vm with .asm
        char input_copy[512];
        strncpy(input_copy, input_path, sizeof(input_copy) - 1);
        input_copy[sizeof(input_copy) - 1] = '\0';
        
        char* dot = strrchr(input_copy, '.');
        if (dot && strcmp(dot, ".vm") == 0) {
            *dot = '\0';
        }
        snprintf(output_filename, sizeof(output_filename), "%s.asm", input_copy);
    }

    FILE* output_file = fopen(output_filename, "w");
    if (!output_file) {
        fprintf(stderr, "Error: Could not open output file %s\n", output_filename);
        return EXIT_FAILURE;
    }

    CodeWriter writer;
    codewriterInit(&writer, output_file);
    
    if (isDir) {
        // Write bootstrap code for multi-file programs
        writeInit(&writer);
        
        // Process all .vm files in directory
        DIR* dir = opendir(input_path);
        if (!dir) {
            fprintf(stderr, "Error: Could not open directory %s\n", input_path);
            fclose(output_file);
            return EXIT_FAILURE;
        }
        
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            if (hasVmExtension(entry->d_name)) {
                char filepath[512];
                snprintf(filepath, sizeof(filepath), "%s/%s", input_path, entry->d_name);
                fprintf(stderr, "Processing: %s\n", filepath);
                processVmFile(&writer, filepath);
            }
        }
        closedir(dir);
    } else {
        // Process single file (no bootstrap code needed)
        processVmFile(&writer, input_path);
    }

    fclose(output_file);
    printf("Output written to: %s\n", output_filename);
    return EXIT_SUCCESS;
}