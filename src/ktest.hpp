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

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include <cerrno>
#include <cstring>
#include <typeinfo>

// this stuff is posix-only
#ifdef __unix__
#include <unistd.h>
#include <sys/wait.h>
#endif

namespace ktest {
    // ---- Assertion Setup Code ---- //

    class KAssertionError final : public std::exception {
    public:
        KAssertionError() = default;
    };

    class KAssertionResult final {
        std::string msg_;
        bool success_;

    public:
        KAssertionResult()
            : success_(true) {
        }

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

        KAssertionHelper &operator=(const std::ostream &ostr) const {
            // we use the '=' operator because that takes the lowest precedence while still being an infix operator.
            std::cout << filepath << ":" << line << ": Assertion Failure" << std::endl;
            std::cout << msg << std::endl;
            const auto &str = dynamic_cast<const std::stringstream&>(ostr);
            if (str.rdbuf()->in_avail())
                std::cout << "    " << str.str() << std::endl;
            throw KAssertionError();
        }
    };


    // ---- Actual Assertions ---- //

    /// Base assertion. This takes a description expression and a check expression.
#define KTEST_KASSERT_BASE(resx) \
    if (const ::ktest::KAssertionResult __ktest_res = (resx)); \
    else ::ktest::KAssertionHelper(__ktest_res.msg(), __FILE__, __LINE__) = std::stringstream()

#define KTEST_KASSERT_RES_BASE(desc, check) \
    ::std::stringstream ss; \
    ss << desc; \
    return ::ktest::KAssertionResult(ss.str(), (check))

    inline KAssertionResult ktest_assert_true(const std::string &checkStr, const bool check) {
        KTEST_KASSERT_RES_BASE("ASSERT_TRUE - Expected the following to be true:\n  '" << checkStr << "': " << check,
                                 check);
    }

    /// Asserts that an expression results in 'true'.
#define KASSERT_TRUE(check) KTEST_KASSERT_BASE(::ktest::ktest_assert_true(#check, (check)))

    inline KAssertionResult ktest_assert_false(const std::string &checkStr, const bool check) {
        KTEST_KASSERT_RES_BASE(
            "ASSERT_FALSE - Expected the following to be false:\n  '" << checkStr << "': " << check, !check);
    }

    /// Asserts that an expression results in 'false'.
#define KASSERT_FALSE(check) KTEST_KASSERT_BASE(::ktest::ktest_assert_false(#check, (check)))

    template<typename E, typename A>
    KAssertionResult ktest_assert_eq(const std::string &expectedStr, const std::string &actualStr, const E &expected, const A &actual) {
        KTEST_KASSERT_RES_BASE("ASSERT_EQ - Expected the following to be equal:\n  '" << expectedStr << "': " << expected << "\n  '" << actualStr << "': " << actual, expected == actual);
    }

    /// Asserts that two expressions are equal.
#define KASSERT_EQ(expected, actual) KTEST_KASSERT_BASE(::ktest::ktest_assert_eq(#expected, #actual, (expected), (actual)))

    template<typename E, typename A>
    KAssertionResult ktest_assert_ne(const std::string &expectedStr, const std::string &actualStr, const E &expected, const A &actual) {
        KTEST_KASSERT_RES_BASE("ASSERT_NE - Expected the following to be not equal:\n  '" << expectedStr << "': " << expected << "\n  '" << actualStr << "': " << actual, expected != actual);
    }

    /// Asserts that two expressions are equal.
#define KASSERT_NE(expected, actual) KTEST_KASSERT_BASE(::ktest::ktest_assert_ne(#expected, #actual, (expected), (actual)))

    template<typename A, typename B>
    KAssertionResult ktest_assert_gt(const std::string &aStr, const std::string &bStr, const A &a, const B &b) {
        KTEST_KASSERT_RES_BASE("ASSERT_GT - Expected the following 'a' to be greater than 'b':\n  a: '" << aStr << "': " << a << "\n  b: '" << bStr << "': " << b, a > b);
    }

    /// Asserts that two expressions are equal.
#define KASSERT_GT(a, b) KTEST_KASSERT_BASE(::ktest::ktest_assert_gt(#a, #b, (a), (b)))

    template<typename A, typename B>
    KAssertionResult ktest_assert_ge(const std::string &aStr, const std::string &bStr, const A &a, const B &b) {
        KTEST_KASSERT_RES_BASE("ASSERT_GT - Expected the following 'a' to be greater than or equal to 'b':\n  a: '" << aStr << "': " << a << "\n  b: '" << bStr << "': " << b, a >= b);
    }

    /// Asserts that two expressions are equal.
#define KASSERT_GE(a, b) KTEST_KASSERT_BASE(::ktest::ktest_assert_ge(#a, #b, (a), (b)))

    template<typename A, typename B>
    KAssertionResult ktest_assert_lt(const std::string &aStr, const std::string &bStr, const A &a, const B &b) {
        KTEST_KASSERT_RES_BASE("ASSERT_GT - Expected the following 'a' to be less than 'b':\n  a: '" << aStr << "': " << a << "\n  b: '" << bStr << "': " << b, a < b);
    }

    /// Asserts that two expressions are equal.
#define KASSERT_LT(a, b) KTEST_KASSERT_BASE(::ktest::ktest_assert_lt(#a, #b, (a), (b)))

    template<typename A, typename B>
    KAssertionResult ktest_assert_le(const std::string &aStr, const std::string &bStr, const A &a, const B &b) {
        KTEST_KASSERT_RES_BASE("ASSERT_GT - Expected the following 'a' to be less than or equal to 'b':\n  a: '" << aStr << "': " << a << "\n  b: '" << bStr << "': " << b, a <= b);
    }

    /// Asserts that two expressions are equal.
#define KASSERT_LE(a, b) KTEST_KASSERT_BASE(::ktest::ktest_assert_le(#a, #b, (a), (b)))

    /// Asserts that a block throws an exception. Takes the expected exception type, the captures into the thrower block, and the thrower block itself.
#define KASSERT_THROWS(expected, captures, thrower) \
    if (const ::ktest::KAssertionResult res = captures { \
        try thrower \
        catch (const expected &) { \
            return ::ktest::KAssertionResult(); \
        } catch (const ::std::exception &e) { \
            ::std::stringstream ss; \
            ss << "ASSERT_THROWS - Expected the exeption '" << #expected << "' to be thrown by the following code:\n  " << #thrower << "\nbut a different exception was thrown: " << typeid(e).name() << "(\"" << e.what() << "\")"; \
            return ::ktest::KAssertionResult(ss.str(), false); \
        } \
        ::std::stringstream ss; \
        ss << "ASSERT_THROWS - Expected the exception '" << #expected << "' to be thrown by the following code:\n  " << #thrower << "\nbut no exception was thrown."; \
        return ::ktest::KAssertionResult(ss.str(), false); \
    }()); \
    else ::ktest::KAssertionHelper(res.msg(), __FILE__, __LINE__) = std::stringstream()


    // ---- Test Collector Code ---- //

    class KTestTest;

    inline std::vector<KTestTest> &getTests() {
        // avoid static initialization hell
        static std::vector<KTestTest> tests;
        return tests;
    }

    class KTestTest {
        std::string name_;
        std::function<void()> fn_;

    public:
        KTestTest(const std::string &name, const std::function<void()> &fn)
            : name_(name),
              fn_(fn) {
            getTests().push_back(*this);
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


    // ---- Test Runner Code ---- //

    /// Run all auto-registered tests.
    inline void runAllTests() {
#ifdef __unix__
        const char *forkEnv = std::getenv("KTEST_FORK");
        const bool shouldFork = forkEnv != nullptr && !std::strcmp(forkEnv, "1");
#endif
        const char *exitEnv = std::getenv("KTEST_EXIT");
        const bool shouldExit = exitEnv != nullptr && !std::strcmp(exitEnv, "1");

        size_t failedTests = 0;
        size_t passedTests = 0;
        for (const auto &test: getTests()) {
            std::cout << "Running test: \033[1;36m" << test.name() << "\033[0m" << std::endl;
#ifdef __unix__
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
#endif
                try {
                    test();
                    std::cout << "Test \033[1;36m" << test.name() << "\033[0m \033[1;32mpassed\033[0m." << std::endl;
                    ++passedTests;
                } catch (const KAssertionError &) {
                    std::cout << "Test \033[1;36m" << test.name() << "\033[0m \033[1;31mfailed\033[0m." << std::endl;
                    ++failedTests;
                }
            }
#ifdef __unix__
        }
#endif

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
