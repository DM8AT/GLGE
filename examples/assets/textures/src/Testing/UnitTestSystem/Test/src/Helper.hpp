/**
 * @file Helper.hpp
 * @author DM8AT
 * @brief define some simple helper functions
 * @version 0.1
 * @date 2026-05-20
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the executor
#include "TestExecutor.h"
//add vectors
#include <vector>

namespace TestImpl {

/**
 * @brief resize a vector
 * 
 * This function may never throw. It gives errors back using the `TestExecResult` enum
 * 
 * @tparam T the type of the vector
 * @tparam Alloc the type of the used allocator
 * @param vec the vector to resize
 * @param size the size to resize to
 * @return `TestExecResult` the result of the resizing, `TEST_EXEC_SUCCESS` on success
 */
template <typename T, typename Alloc> 
TestExecResult resizeVector(std::vector<T, Alloc>& vec, size_t size) noexcept {
    //try to resize (remember: resize may throw)
    try {
        vec.resize(size);
    } catch (std::bad_alloc) {
        return TEST_EXEC_OUT_OF_MEMORY;
    } catch (...) {
        return TEST_EXEC_GENERIC_ERROR;
    }
    //success
    return TEST_EXEC_SUCCESS;
}

}