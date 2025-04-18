// Copywrite (c) 2025 Cyan Kneelawk
//
// MIT Licensed

/*
 * ktest.hpp
 *
 * Kneelawk's simple testing framework. Because my teacher told me not to use external testing frameworks.
 *
 * Portions of this testing framework were inspired by GoogleTest.
 */

#ifndef KTEST_HPP
#define KTEST_HPP

#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <sys/wait.h>

namespace ktest {
    class KAssertionError final : public std::exception {
    public:
        KAssertionError() = default;
    };

    class KAssertionResult final {
        std::string msg_;
        bool success_;

    public:
        KAssertionResult(const std::string &msg, const bool success)
            : msg_(msg),
              success_(success) {
        }

        explicit operator bool() const {
            return success_;
        }

        std::string msg() const {
            return msg_;
        }
    };

    class KAssertionHelper final {
        std::string msg;
        std::string filepath;
        size_t line;

    public:
        KAssertionHelper(const std::string &msg, const std::string &filepath, const size_t line)
            : msg(msg),
              filepath(filepath),
              line(line) {
        }

        KAssertionHelper &operator=(const std::stringstream &str) const {
            // we use the '=' operator because that takes the lowest precedence while still being an infix operator.
            std::cout << filepath << ":" << line << ": Assertion Failure" << std::endl;
            std::cout << msg << std::endl;
            if (str.rdbuf()->in_avail())
                std::cout << "    " << str.str() << std::endl;
            throw KAssertionError();
        }
    };

    inline KAssertionResult assert_true(const std::string &checkStr, bool checkRes) {
        return {
            (std::stringstream() << "ASSERT_TRUE - Expected the following to be true:\n  '" << checkStr << "': " <<
             checkRes).str(),
            checkRes
        };
    }

#define KASSERT_TRUE(check) \
    if (const ::ktest::KAssertionResult res = ::ktest::assert_true(#check, (check))); \
    else ::ktest::KAssertionHelper(res.msg(), __FILE__, __LINE__) = ::std::stringstream()

#define KASSERT_EQ(expected, actual) \
    if (const ::ktest::KAssertionResult res = ::ktest::KAssertionResult((::std::stringstream() << "ASSERT_EQ - Expected the following to be equal:\n  '" << #expected << "': " << (expected) << "\n  '" << #actual << "': " << (actual)).str(), (expected) == (actual))); \
    else ::ktest::KAssertionHelper(res.msg(), __FILE__, __LINE__) = std::stringstream()

    class KTestTest;

    static std::vector<KTestTest> tests;

    class KTestTest {
        std::string name_;
        std::function<void()> fn_;

    public:
        KTestTest(const std::string &name, const std::function<void()> &fn)
            : name_(name),
              fn_(fn) {
            tests.push_back(*this);
        }

        KTestTest(const KTestTest &other) = default;

        KTestTest(KTestTest &&other) noexcept
            : name_(std::move(other.name_)),
              fn_(std::move(other.fn_)) {
        }

        KTestTest &operator=(const KTestTest &other) {
            if (this == &other)
                return *this;
            name_ = other.name_;
            fn_ = other.fn_;
            return *this;
        }

        KTestTest &operator=(KTestTest &&other) noexcept {
            if (this == &other)
                return *this;
            name_ = std::move(other.name_);
            fn_ = std::move(other.fn_);
            return *this;
        }

        std::string name() const {
            return name_;
        }

        void operator()() const {
            this->fn_();
        }
    };

#define KTEST(name) \
    void __ktest_fn_##name(); \
    class __KTest_##name : public ::ktest::KTestTest { \
    public: \
        __KTest_##name() : ::ktest::KTestTest(#name, __ktest_fn_##name) { \
        } \
    }; \
    static __KTest_##name __ktest_##name; \
    void __ktest_fn_##name()

    inline void runAllTests() {
        const char *forkEnv = std::getenv("KTEST_FORK");
        const bool shouldFork = forkEnv != nullptr && !std::strcmp(forkEnv, "1");
        const char *exitEnv = std::getenv("KTEST_EXIT");
        const bool shouldExit = exitEnv != nullptr && !std::strcmp(exitEnv, "1");

        size_t failedTests = 0;
        size_t passedTests = 0;
        for (const auto &test: tests) {
            std::cout << "Running test: \033[1;36m" << test.name() << "\033[0m" << std::endl;
            if (shouldFork) {
                const pid_t child = fork();
                if (child == 0) {
                    // we're the child process
                    try {
                        test();
                    } catch (const KAssertionError &) {
                        exit(-1);
                    }
                    exit(0);
                }
                if (child == -1) {
                    std::cerr << "Error starting test " << test.name() << ": " << std::strerror(errno) << std::endl;
                } else {
                    // we're the parent process
                    int status;
                    waitpid(child, &status, 0);

                    if (WIFEXITED(status)) {
                        const int statusRet = WEXITSTATUS(status);
                        if (!statusRet) {
                            std::cout << "Test \033[1;36m" << test.name() << "\033[0m \033[1;32mpassed\033[0m." <<
                                    std::endl;
                            ++passedTests;
                        } else {
                            std::cout << "Test \033[1;36m" << test.name() << "\033[0m \033[1;31mfailed\033[0m." <<
                                    std::endl;
                            ++failedTests;
                        }
                    } else if (WIFSIGNALED(status)) {
                        const int signal = WSTOPSIG(status);
                        std::cout << "Test \033[1;36m" << test.name() << "\033[0m \033[1;31mfailed\033[0m. Signal: " <<
                                strsignal(signal) << std::endl;
                        ++failedTests;
                    }
                }
            } else {
                try {
                    test();
                    std::cout << "Test \033[1;36m" << test.name() << "\033[0m \033[1;32mpassed\033[0m." << std::endl;
                    ++passedTests;
                } catch (const KAssertionError &) {
                    std::cout << "Test \033[1;36m" << test.name() << "\033[0m \033[1;31mfailed\033[0m." << std::endl;
                    ++failedTests;
                }
            }
        }

        std::cout << "\033[1m## TEST RESULTS ##\033[0m" << std::endl;
        std::cout << "  Tests passed: " << passedTests << std::endl;
        std::cout << "  Tests failed: " << failedTests << std::endl;

        if (failedTests) {
            std::cout << "\033[1;31m## TESTS FAILED ##\033[0m" << std::endl;
        }

        if (shouldExit && failedTests) {
            std::cout << "Exiting..." << std::endl;
            exit(-1);
        }

        std::cout << std::endl;
    }
}

#endif //KTEST_HPP
