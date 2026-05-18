/**
 * @file ExamplePluginLoader.cpp
 * @author DM8AT
 * @brief implement the example plugin loading system
 * @version 0.1
 * @date 2026-05-18
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "ExamplePluginLoader.h"
#include "ExamplePluginContract.h"
#include "Examples.h"
#include "DynamicLibrary.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

/**
 * @brief store a vector example registry
 * 
 * This does NOT own the vector, it only references it
 */
class VectorExampleRegistry final : public ExampleRegistry {
public:

    /**
     * @brief Construct a new Vector Example Registry
     * 
     * @param out the vector to store all examples in
     */
    VectorExampleRegistry(std::vector<std::pair<const char*, PFN_ExampleFunc>>& out) noexcept
     : m_out(out)
    {}

    /**
     * @brief add an example to the registry
     * 
     * @param displayName the name to display
     * @param entry the entry function to call
     */
    void addExample(const char* displayName, PFN_ExampleFunc entry) override
    {m_out.emplace_back(displayName, entry);}

private:

    /**
     * @brief store a reference to the vector of examples
     */
    std::vector<std::pair<const char*, PFN_ExampleFunc>>& m_out;

};

/**
 * @brief Keep modules resident: example function pointers live inside them
 */
static std::vector<DynamicLibrary> g_exampleModules;

/**
 * @brief check if a file is a dynamic library
 * 
 * @param p the path to the file to check
 * @return `true` if a plugin extension was found, `false` if it is not adynamic library
 */
bool hasPluginExtension(const std::filesystem::path& p) {
    //just determine using the extensions
    const std::string ext = p.extension().string();
    return ext == ".so" || ext == ".dll" || ext == ".dylib";
}

/**
 * @brief a function to load all dynamic libraries from a directory
 * 
 * @param directoryPath the path to the directory to load from
 */
void loadExamplePluginsFromDirectory(const char* directoryPath) {
    //check if the directory path exists
    const std::filesystem::path root(directoryPath);
    if (!std::filesystem::exists(root) || !std::filesystem::is_directory(root)) 
    {throw std::runtime_error(std::string("Example plugin directory is missing or not a directory: ") + root.string());}

    //create the example registry
    VectorExampleRegistry reg(EXAMPLES);
    size_t loadedCount = 0;

    //iterate over all elements in the directory
    for (const std::filesystem::directory_entry& ent : std::filesystem::directory_iterator(root)) {
        //discard everything that is not a file
        if (!ent.is_regular_file())
        {continue;}
        //discard everything that is not a dynamic library
        const std::filesystem::path& p = ent.path();
        if (!hasPluginExtension(p))
        {continue;}

        //load the library
        DynamicLibrary lib;
        //sanity check if the load was correct
        if (!lib.open(p)) {
            //print an error and just continue
        #if defined(_WIN32)
            std::cerr << "[examples] skipped (failed to load): " << p.string() << "\n";
        #else
            const char* err = dlerror();
            std::cerr << "[examples] skipped (failed to load): " << p.string();
            if (err != nullptr) {
                std::cerr << " — " << err;
            }
            std::cerr << "\n";
        #endif
            continue;
        }

        //try to load the register function
        auto* regFn = lib.symbol<PFN_registerExamplePlugin>(EXAMPLE_SYS_REGISTER_EXAMPLE_PLUGIN_SYM);
        if (regFn == nullptr) {
            //skip the library
            std::cerr << "[examples] skipped (missing register symbol): " << p.string() << "\n";
            continue;
        }

        //register the examples contained in the library
        regFn(reinterpret_cast<ExampleRegistryPtr>(&reg));
        
        //store the library
        g_exampleModules.push_back(std::move(lib));
        //increase the amount of loaded libraries
        ++loadedCount;
    }

    //throw an error if no examples where found
    if (loadedCount == 0 && EXAMPLES.empty()) {
        throw std::runtime_error(
            "No example modules could be loaded and no static examples were registered. "
            "Add .so/.dll/.dylib plugins that export " EXAMPLE_SYS_REGISTER_EXAMPLE_PLUGIN_SYM ".");
    }
}
