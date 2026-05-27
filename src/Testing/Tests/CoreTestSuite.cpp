/**
 * @file CoreTestSuite.cpp
 * @author DM8AT
 * @brief implement the core tests
 * @version 0.1
 * @date 2026-05-27
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add GLGE
#define GLGE_NO_THANKS_MSG
#include "GLGE.h"
//add the testing contract
#include "TestContract.h"

//add stringstreams
#include <sstream>

static void assertHelper(const std::string& expected, const std::string& actual, bool passed, const TestFunctions* fn) {
    TestAssertion ass;
    ass.expected = expected.c_str();
    ass.actual = actual.c_str();
    ass.passed = passed;
    (*(fn->assertion))(&ass);
}

void jobSystemTest(const TestContext* ctx, TestReport* report, const TestFunctions* fn) {
    //initialize as controlled fail
    report->result = TEST_CONTROLLED_FAIL;

    TestMessage msg;
    msg.msg = "[INFO] Testing if a task can run in the job system";
    (*(fn->log))(&msg);

    //create an employer
    GLGE::Tiny::Jobs::Employer emp = 4;

    uint64_t val = 0;
    //create a dummy task to run
    GLGE::Tiny::Jobs::Task task([](uint64_t& v){
        v = 1;
    }, std::ref(val));
    //wrap in a node and schedule
    GLGE::Tiny::Jobs::Job::TaskNode taskNode(&task);
    emp.add(&taskNode);
    //await the results
    emp.waitIdle();

    //sanity check
    if (val == 0) {
        assertHelper(
            "The value remained 0 -> the task did not run",
            "Expected the value to be set to 1 during task execution", 
            false, fn
        );
    } else if (val == 1) {
        assertHelper(
            "The value was set to 1 by the task",
            "The value was set to 1 by the task",
            true, fn
        );
    } else {
        assertHelper(
            "The value was set to something unexpected",
            "The value should be set to 1 by the task",
            false, fn
        );
    }

    msg.msg = "[INFO] Testing if automatic task ordering works";
    (*(fn->log))(&msg);

    //create two tasks: A and B. Make sure that A runs before B. 
    val = 5;
    uint64_t recordedVal = 0;
    GLGE::Tiny::Jobs::Task A([](uint64_t& val, uint64_t& recordedVal) {
        recordedVal = val;
    }, std::ref(val), std::ref(recordedVal));
    GLGE::Tiny::Jobs::Task B([](uint64_t& val) {
        val = 2;
    }, std::ref(val));
    GLGE::Tiny::Jobs::Job job;
    auto aId = job.add(A, 0);
    job.add(B, 0, {aId});
    job.finalize();
    //run the job
    emp.add(&job);
    emp.waitIdle();
    //check the result
    if (recordedVal == 5 && val == 2) {
        assertHelper(
            "Expected the recorded value to be 5 and the current value to be 2",
            "Expected the recorded value to be 5 and the current value to be 2",
            true, fn
        );
    } else {
        assertHelper(
            "Expected the recorded value to be 5 and the current value to be 2",
            "Either recorded val is not 5 or val is not 2",
            false, fn
        );
    }

    msg.msg = "[INFO] Testing if cross-thread resource sharing and bulk tasks work";
    (*(fn->log))(&msg);

    //count to a lot in the most inefficient was possible
    std::atomic_uint64_t count = 0;
    constexpr size_t expectedCount = 1E5;
    GLGE::Tiny::Jobs::BulkTask tasks(expectedCount, [](std::atomic_uint64_t& count) {
        count.fetch_add(1, std::memory_order_acq_rel);
    }, std::ref(count));
    emp.add_bulk(tasks);
    emp.waitIdle();
    //check if the count is the expected count
    if (count.load(std::memory_order_acquire) == static_cast<uint64_t>(expectedCount)) {
        assertHelper(
            "Expected the count the be equal to the expected count", 
            "The count is equal to the expected count",
            true, fn
        );
    } else {
        assertHelper(
            "Expected the count the be equal to the expected count",
            "The count is not equal to the expected count",
            false, fn
        );
    }

    //success
    report->result = TEST_SUCCESS;
}

void referenceCountingTest(const TestContext* ctx, TestReport* report, const TestFunctions* fn) {
    //initialize as controlled fail
    report->result = TEST_CONTROLLED_FAIL;

    //create a simple class to track aliveness
    class Foo : public GLGE::Referable {
    public:
        Foo(bool* tracker = nullptr)
         : m_exists(tracker)
        {if (m_exists) {*m_exists = true;}}

        ~Foo() 
        {if (m_exists) {*m_exists = false;}}

    protected:
        bool* m_exists = nullptr;
    };

    bool alive = false;
    {
    TestMessage msg;
    msg.msg = "[INFO] Testing if the count is tracked correctly";
    (*(fn->log))(&msg);

    Foo f = &alive;
    if (f.getReferenceCount() == 1) {
        assertHelper(
            "Expected that 1 reference to the test object exists",
            "1 reference exists",
            true, fn
        );
    } else if (f.getReferenceCount() == 0) {
        assertHelper(
            "Expected that 1 reference to the test object exists",
            "No reference existed", 
            false, fn
        );
    } else {
        assertHelper(
            "Expected that 1 reference to the test object exists",
            "More than 1 references existed",
            false, fn
        );
    }

    msg.msg = "[INFO] Testing if the reference counter updates correctly";
    (*(fn->log))(&msg);

    uint64_t doubleCount = 0;
    uint64_t singleCount = 0;

    //reference the test object somewhere else, then delete the reference
    {
        GLGE::Reference<Foo> fRef = &f;
        doubleCount = f.getReferenceCount();
    }
    singleCount = f.getReferenceCount();

    //sanity check the results
    if (singleCount == 1 && doubleCount == 2) {
        assertHelper(
            "Expected the reference count to report 2 while another reference existed and switch to 1 if it is deleted",
            "A count of 2 was reported while two references existed and 1 if one reference died",
            true, fn
        );
    } else {
        assertHelper(
            "Expected the reference count to report 2 while another reference existed and switch to 1 if it is deleted",
            "Different counts were reported",
            false, fn
        );
    }

    }

    //check that the object was deleted correctly
    if (alive) {
        assertHelper(
            "Expected the object to be deleted once all reference were deleted",
            "The destructor was never called",
            false, fn
        );
    } else {
        assertHelper(
            "Expected the object to be deleted once all reference were deleted",
            "The object was destroyed",
            true, fn
        );
    }

    //success
    report->result = TEST_SUCCESS;
}

void eventSystemTest(const TestContext* ctx, TestReport* report, const TestFunctions* fn) {
    //initialize as controlled fail
    report->result = TEST_CONTROLLED_FAIL;

    TestMessage msg;
    msg.msg = "[INFO] Testing if events can be listened to";
    (*(fn->log))(&msg);

    GLGE::EventBus bus;
    uint64_t count = 0;
    uint64_t* countPtr = &count;
    GLGE::EventBus::Callback callback = [](const GLGE::Event& event) -> void {
        *(*(reinterpret_cast<uint64_t**>(event.getStorage()))) = 1;
    };
    bus.subscribe(0, callback);
    bus.emit(GLGE::Event(0, reinterpret_cast<GLGE::u8*>(&countPtr), sizeof(countPtr)));

    //check if the subscriber got it
    if (count == 1) {
        assertHelper(
            "Expected the count to be 1",
            "The count was 1",
            true, fn
        );
    } else {
        assertHelper(
            "Expected the count to be 1",
            "The count was not 1",
            false, fn
        );
    }

    //success
    report->result = TEST_SUCCESS;
}

void ioTest(const TestContext* ctx, TestReport* report, const TestFunctions* fn) {
    //initialize as controlled fail
    report->result = TEST_CONTROLLED_FAIL;

    TestMessage msg;
    msg.msg = "[INFO] Testing if keyboards can register presses";
    (*(fn->log))(&msg);

    GLGE::Keyboard board;
    board.press(GLGE::Key::W);
    board.update();

    if (board.pressed()[GLGE::Key::W] && board.down()[GLGE::Key::W] && !board.up()[GLGE::Key::W]) {
        assertHelper(
            "Expected the key to be pressed and down, but not up",
            "The key was pressed and down, but not up",
            true, fn
        );
    } else {
        assertHelper(
            "Expected the key to be pressed and down, but not up",
            "The expected state was not found",
            false, fn
        );
    }

    msg.msg = "[INFO] Testing if keyboards can register releases";
    (*(fn->log))(&msg);

    board.release(GLGE::Key::W);
    board.update();

    if (!board.pressed()[GLGE::Key::W] && !board.down()[GLGE::Key::W] && board.up()[GLGE::Key::W]) {
        assertHelper(
            "Expected the key to be not pressed nor down, but up",
            "The key was not pressed nor down, but up",
            true, fn
        );
    } else {
        assertHelper(
            "Expected the key to be not pressed nor down, but up",
            "The expected state was not found",
            false, fn
        );
    }

    msg.msg = "[INFO] Testing if a mouse button can be pressed";
    (*(fn->log))(&msg);

    GLGE::Mouse mouse;
    mouse.press(GLGE::MouseButton::LEFT);
    mouse.update();

    if (mouse.pressed()[GLGE::MouseButton::LEFT] && mouse.down()[GLGE::MouseButton::LEFT] && !mouse.up()[GLGE::MouseButton::LEFT]) {
        assertHelper(
            "Expected the button to be pressed and down, but not up",
            "The button was pressed and down, but not up",
            true, fn
        );
    } else {
        assertHelper(
            "Expected the button to be pressed and down, but not up",
            "The expected state was not found",
            false, fn
        );
    }

    msg.msg = "[INFO] Testing if a mouse button can be released";
    (*(fn->log))(&msg);

    mouse.release(GLGE::MouseButton::LEFT);
    mouse.update();

    if (!mouse.pressed()[GLGE::MouseButton::LEFT] && !mouse.down()[GLGE::MouseButton::LEFT] && mouse.up()[GLGE::MouseButton::LEFT]) {
        assertHelper(
            "Expected the button to be not pressed nor down, but up",
            "The button was not pressed nor down, but up",
            true, fn
        );
    } else {
        assertHelper(
            "Expected the button to be not pressed nor down, but up",
            "The expected state was not found",
            false, fn
        );
    }

    msg.msg = "[INFO] Testing if a mouse can be moved";
    (*(fn->log))(&msg);

    mouse.move(GLGE::vec2(4,4), GLGE::vec2(4,4));
    mouse.update();

    if (mouse.getPos().x == 4 && mouse.getPos().y == 4 && mouse.getDelta().x == 4 && mouse.getDelta().y == 4) {
        assertHelper(
            "Expected mouse to be at (4,4) and have moved (4,4) this tick",
            "The mouse was at (4,4) and moved (4,4) this tick",
            true, fn
        );
    } else {
        assertHelper(
            "Expected mouse to be at (4,4) and have moved (4,4) this tick",
            "The mouse was not at (4,4) or it was not moved (4,4) this tick",
            false, fn
        );
    }

    msg.msg = "[INFO] Testing if a motion decays, but the position does not";
    (*(fn->log))(&msg);

    mouse.update();

    if (mouse.getPos().x == 4 && mouse.getPos().y == 4 && mouse.getDelta().x == 0 && mouse.getDelta().y == 0) {
        assertHelper(
            "Expected mouse to be at (4,4) and have moved (0,0) this tick",
            "The mouse was at (4,4) and moved (0,0) this tick",
            true, fn
        );
    } else {
        assertHelper(
            "Expected mouse to be at (4,4) and have moved (0,0) this tick",
            "The mouse was not at (4,4) or it was not moved (0,0) this tick",
            false, fn
        );
    }

    //success
    report->result = TEST_SUCCESS;
}

void taskTest(const TestContext* ctx, TestReport* report, const TestFunctions* fn) {
    //initialize as controlled fail
    report->result = TEST_CONTROLLED_FAIL;

    GLGE::TaskQueue<1024> queue;
    bool alive = true;
    //create a task queue that runs on a different thread
    std::thread other([](GLGE::TaskQueue<1024>& queue, bool& alive) {
        while (alive) {
            queue.execute();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }, std::ref(queue), std::ref(alive));

    TestMessage msg;
    msg.msg = "[INFO] Testing if tasks get executed";
    (*(fn->log))(&msg);

    GLGE::TaskHandle<uint64_t> handle;
    GLGE::Task task = GLGE::Task::create([](uint64_t val) -> uint64_t {
        return val*2;
    }, &handle, uint64_t(21));
    queue.push(task);
    handle.wait();

    if (handle.get() == 42) {
        assertHelper(
            "Expected the handle to return 42",
            "The handle returned 42",
            true, fn
        );
    } else {
        assertHelper(
            "Expected the handle to return 42",
            "The handle did not return 42",
            false, fn
        );
    }

    msg.msg = "[INFO] Testing if a lot of tasks can be pushed";
    (*(fn->log))(&msg);

    uint64_t count = 0;
    GLGE::Task t = GLGE::Task::create<void, uint64_t&>([](uint64_t& count) -> void {++count;}, nullptr, std::ref(count));
    for (size_t i = 0; i < 100; ++i)
    {queue.push(t);}
    while (!queue.isEmpty()) {std::this_thread::sleep_for(std::chrono::milliseconds(1));}

    if (count == 100) {
        assertHelper(
            "Expected the count to be 100",
            "The count was 100",
            true, fn
        );
    } else {
        assertHelper(
            "Expected the count to be 100",
            "The count was not 100",
            false, fn
        );
    }

    //join the thread back
    alive = false;
    other.join();

    //success
    report->result = TEST_SUCCESS;
}

std::vector<Test> tests {
    Test{
        .header = {
            .sType = TEST_TEST,
            .pNext = nullptr
        },
        .entry = {
            .header = {
                .sType = TEST_ENTRY,
                .pNext = nullptr
            },
            .name = "Job system test",
            .tags = "job async core",
            .description = "Test the functionality of the job system",
            .timeout = uint64_t(1E4),
            .requirements = TEST_REQUIREMENT_ASYNC_BIT
        },
        .invoker = &jobSystemTest
    },
    Test{
        .header = {
            .sType = TEST_TEST,
            .pNext = nullptr
        },
        .entry = {
            .header = {
                .sType = TEST_ENTRY,
                .pNext = nullptr
            },
            .name = "Intrinsic reference counting test",
            .tags = "reference_counting core",
            .description = "Test the functionality of the intrinsic reference counting system",
            .timeout = uint64_t(1E4),
            .requirements = 0
        },
        .invoker = &referenceCountingTest
    },
    Test{
        .header = {
            .sType = TEST_TEST,
            .pNext = nullptr
        },
        .entry = {
            .header = {
                .sType = TEST_ENTRY,
                .pNext = nullptr
            },
            .name = "Event system test",
            .tags = "events core",
            .description = "Test that the event system works",
            .timeout = uint64_t(1E4),
            .requirements = 0
        },
        .invoker = &eventSystemTest
    },
    Test{
        .header = {
            .sType = TEST_TEST,
            .pNext = nullptr
        },
        .entry = {
            .header = {
                .sType = TEST_ENTRY,
                .pNext = nullptr
            },
            .name = "Keyboard and mouse test",
            .tags = "keyboard mouse core",
            .description = "Test that the abstract keyboard and mouse classes work",
            .timeout = uint64_t(1E4),
            .requirements = 0
        },
        .invoker = &ioTest
    },
    Test{
        .header = {
            .sType = TEST_TEST,
            .pNext = nullptr
        },
        .entry = {
            .header = {
                .sType = TEST_ENTRY,
                .pNext = nullptr
            },
            .name = "Task queue test",
            .tags = "tasks async core",
            .description = "Test if tasks can be executed through task queues",
            .timeout = uint64_t(1E4),
            .requirements = TEST_REQUIREMENT_ASYNC_BIT
        },
        .invoker = &taskTest
    }
};

TEST_REGISTER_TEST_FN {
    //store the test count
    if (data == NULL) {
        *size = tests.size();
        return;
    }
    //register all math tests
    memcpy(data, tests.data(), sizeof(Test)* (*size));
}

TEST_GET_ABI_VERSION_FN {
    return 1; //use the initial test ABI version
}