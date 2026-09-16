# ABI-Driven Dynamic C/C++ Test Framework

A high-performance, ABI-stable C/C++ test orchestration and execution framework designed for cross-language compatibility, asynchronous test scheduling, and multi-process isolated test execution.

By decoupling the test interface from the runner engine using a strict data layout contract, this framework allows shared libraries (`.dll`, `.so`, `.dylib`) to export natively testable suites without requiring heavy runtime linkages or forcing a specific toolchain model.

---

## Architecture Overview

The system is split into three primary boundaries to guarantee safety, modularity, and environment management:

```
┌────────────────────────────────────────────────────────┐
│                   Test Launcher / UI                   │
└───────────────────────────┬────────────────────────────┘
                            │ (TestExecutor API)
┌───────────────────────────▼────────────────────────────┐
│                    Executor Engine                     │
│  - Thread Pool Workers     - Exclusivity/Resource Caps │
│  - Resource Scheduler      - JSON Reporting Engine     │
└───────────────────────────┬────────────────────────────┘
                            │ (Subprocess Isolation + Shared Memory)
┌───────────────────────────▼────────────────────────────┐
│                  Isolated Test Worker                  │
│  - Loads target .dll/.so   - Executes PFN_TestInvoker  │
│  - Captures stdout/stderr  - Performs safe IPC Logging │
└────────────────────────────────────────────────────────┘
```

1. **The Test Contract (`TestContract.h`)**: A rigorous C23 header dictating data layout rules. It enables structural expansion via Vulkan-style `pNext` pointer chains without breaking binary compatibility.
2. **The Test Executor Frontend (`TestExecutor.h` / `Executor.hpp`)**: An orchestration engine managing automated resource-locking scheduling (GPU, Networks, Storage, RAM constraints) across dynamic thread worker loops.
3. **The Isolated Worker Context**: Test instances execute inside decoupled child subprocesses, communicating status over strict shared memory segments to protect the core suite runner from hard segment faults, hardware hangs, or runaway leaks.

---

## Core Features

* **C23 Compliant Layout Boundary**: Relies heavily on exact struct layout checking via compile-time evaluation constants (`static_assert`) mapping explicit fields, sizing parameters, and alignments.
* **Intelligent Dependency & Exclusivity Scheduler**: Test descriptors request bitwise operational restrictions (e.g., exclusive GPU access, restricted localized asset streams, or offline-only constraints). The internal dispatcher organizes parallel executions dynamically around active lock constraints.
* **Automated Failure Mitigation**: Captures rogue termination anomalies (`SIGSEGV`, forced closures, task overrides) gracefully. It records stack/exit contexts natively to persistent disk arrays while keeping alternative sibling tests active.
* **Structured Structured Diagnostics**: Builds machine-readable telemetry documents detailing real-time platform clock timelines, hardware diagnostics, console pipelines (`stdout`/`stderr`), and granular test assertions.

---

## API Specifications

### 1. The Dynamic Plugin Boundary (`TestContract.h`)

Shared libraries wishing to interact with this pipeline must export an interface corresponding to the versioning system metadata markers.

#### Structural Strategy (`pNext` Extensibility)

Every contract structure initiates with a `TestBase` segment mapping structural layout classes:

```c
typedef struct s_TestBase {
    TestType sType;
    const void* pNext;
} TestBase;
```

#### Core Entry Symbols

Plugins must explicitly expose the matching entry interfaces:

```c
// Queries the internal plugin structure to fetch or build test allocations
typedef void (TEST_CALL *PFN_RegisterTest)(uint64_t* size, Test* data);

// Validates operational compatibility parameters matching the host configuration
typedef uint64_t (TEST_CALL *PFN_GetTestABIVersion)();
```

---

### 2. Executor Component Interactivity (`TestExecutor.h`)

Application wrappers initialize runtime configurations using opaque non-dispatchable structures alongside type-mapped control blocks:

```c
// Creates a new managed orchestration context
TestExecResult testExecCreateExecutor(
    const TestExecExecutorCreateInfo* pCreateInfo,
    TestExecExecutor* pExecutor
);

// Feeds binary libraries to the active scheduling layout
TestExecResult testExecLoadTests(
    TestExecExecutor executor, 
    const TestExecLoadTestInfo* pLoadTestInfo
);

// Schedules designated tasks to the async thread-pool worker arrangement
TestExecResult testExecExecuteTest(
    TestExecExecutor executor,
    const TestExecExecuteTestInfo* pExecuteTestInfo
);
```

---

## Scheduling Engine & Resource Restrictions

The scheduler allows multiple workers to run tasks in parallel while ensuring that strict resource locks are respected. The following request flags can be attached to any `TestEntry` descriptor block:

| Requirement Flag Bit | Operational Impact on Thread Pipeline |
| --- | --- |
| `TEST_REQUIREMENT_RUN_EXCLUSIVE_BIT` | Freezes adjacent active tasks. Runs strictly as the lone active system pipeline component. |
| `TEST_REQUIREMENT_GPU_BIT` | Instructs the framework to bypass execution if a compatible local graphics engine isn't configured. |
| `TEST_REQUIREMENT_GPU_ACCESS_EXCLUSIVE_BIT` | Binds hardware acceleration pipelines safely, blocking overlapping GPU tasks until execution finishes. |
| `TEST_REQUIREMENT_FILE_ACCESS_EXCLUSIVE_BIT` | Locks asset workspaces securely, blocking concurrent I/O operations from corrupting temporary assets. |
| `TEST_REQUIREMENT_RAM_EXCLUSIVE_BIT` | Isolates tasks with high memory usage to prevent over-allocating system memory resources. |
| `TEST_REQUIREMENT_NETWORKING_BIT` | Validates network connectivity, preventing network-dependent tasks from failing silently while offline. |

---

## Implementation Sample

### Building a Standard Compliant Plugin Interface

```c
#include "TestContract.h"
#include <string.h>

//a basic validation task matching the core operational contract
void TEST_CALL testCaseExample(const TestContext* ctx, TestReport* report, const TestFunctions* fns) {
    //optional localized console communication
    if (fns && fns->log) {
        TestMessage msg = {
            .header = { .sType = TEST_MESSAGE, .pNext = NULL },
            .msg = "Initializing dynamic plugin execution tests...",
            .messageType = TEST_MESSAGE_TYPE_INFO
        };
        fns->log(&msg);
    }

    //set fallback result defaults
    report->result = TEST_SUCCESS;
}

//exportable boundary registration setup
TEST_REGISTER_TEST_FN {
    if (data == NULL) {
        *size = 1; //reporting suite scale
        return;
    }

    //assigning specification contract metrics
    data[0].header.sType = TEST_TEST;
    data[0].header.pNext = NULL;
    
    data[0].entry.header.sType = TEST_ENTRY;
    data[0].entry.name = "ExampleVerificationSuite";
    data[0].entry.tags = "Sanity Rapid Hardware";
    data[0].entry.description = "Performs dynamic structural interface validation across runtime boundaries.";
    data[0].entry.timeout = 5000; //timeout limit in milliseconds
    data[0].entry.requirements = TEST_REQUIREMENT_FILE_ACCESS_BIT;
    
    data[0].invoker = testCaseExample;
}

TEST_GET_ABI_VERSION_FN {
    return 1; //version 1, alias initial release
}
```

---

## Generated Diagnostic Formats (JSON Output)

Every test writes a structured JSON document to its log path, mapping execution metrics and output streams:

```json
{
    "schema_version": 1,
    "meta": {
        "name": "ExampleVerificationSuite",
        "tags": [
            "Sanity",
            "Rapid",
            "Hardware"
        ],
        "description": "Performs dynamic structural interface validation across runtime boundaries."
    },
    "start_time_unix_ms": 1779264000000,
    "log": [
        {
            "type": "info",
            "message": "Initializing dynamic plugin execution tests..."
        }
    ],
    "stdout": [
        "Allocated workspace resources successfully."
    ],
    "stderr": [],
    "end_time_unix_ms": 1779264000150,
    "duration_ms": 150,
    "return": {
        "exit_code": 0,
        "state": "TEST_EXEC_TEST_RESULT_SUCCESS",
        "termination_signal": 0,
        "termination_reason": "exited"
    }
}
```