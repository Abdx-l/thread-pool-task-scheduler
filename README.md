# Thread Pool and Task Scheduler

C++17 fixed worker pool with futures, exception propagation, FIFO ordering within priority bands, drain-on-destruction, and concurrent producer stress tests.

## Run

`cmake -S . -B build && cmake --build build && ctest --test-dir build --output-on-failure`. Sanitizers: add -fsanitize=thread to compiler/linker flags on a supported toolchain.
