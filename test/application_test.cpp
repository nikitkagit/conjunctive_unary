#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "application/application.hpp"

namespace
{
std::string read_file(const std::string &path)
{
    std::ifstream f(path);
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

void write_file(const std::string &path, const std::string &content)
{
    std::ofstream f(path);
    f << content;
}

int run_app(const std::vector<std::string> &args,
            std::ostream &out,
            std::ostream &err)
{
    std::vector<char *> argv;
    argv.reserve(args.size());
    for (const std::string &a : args)
    {
        argv.push_back(const_cast<char *>(a.c_str()));
    }
    Application app(out, err);
    return app.run(static_cast<int>(argv.size()), argv.data());
}

const char *IN_FILE = "test_app_input.txt";
const char *OUT_FILE = "test_app_output.txt";
} // namespace

int main()
{
    // Clean up leftovers from previous runs.
    std::remove(IN_FILE);
    std::remove(OUT_FILE);

    // 1. -h prints usage to stdout and returns 0.
    {
        std::ostringstream out, err;
        const int rc = run_app({"app", "-h"}, out, err);
        assert(rc == 0);
        assert(out.str().find("Usage:") != std::string::npos);
        assert(err.str().empty());
    }

    // 2. --help behaves like -h.
    {
        std::ostringstream out, err;
        const int rc = run_app({"app", "--help"}, out, err);
        assert(rc == 0);
        assert(out.str().find("Usage:") != std::string::npos);
        assert(err.str().empty());
    }

    // 3. Unknown argument: error message and non-zero exit.
    {
        std::ostringstream out, err;
        const int rc = run_app({"app", "--bogus"}, out, err);
        assert(rc != 0);
        assert(err.str().find("Unknown argument") != std::string::npos);
        assert(err.str().find("Usage:") != std::string::npos);
    }

    // 4. Missing value for -f.
    {
        std::ostringstream out, err;
        const int rc = run_app({"app", "-f"}, out, err);
        assert(rc != 0);
        assert(err.str().find("Missing value for -f") != std::string::npos);
    }

    // 5. Missing value for -o.
    {
        std::ostringstream out, err;
        const int rc = run_app({"app", "-o"}, out, err);
        assert(rc != 0);
        assert(err.str().find("Missing value for -o") != std::string::npos);
    }

    // 6. Missing value for -n.
    {
        std::ostringstream out, err;
        const int rc = run_app({"app", "-n"}, out, err);
        assert(rc != 0);
        assert(err.str().find("Missing value for -n") != std::string::npos);
    }

    // 7. Invalid -n value.
    {
        std::ostringstream out, err;
        const int rc = run_app({"app", "-n", "abc"}, out, err);
        assert(rc != 0);
        assert(err.str().find("Invalid value for -n") != std::string::npos);
    }

    // 8. Nonexistent input file.
    {
        std::ostringstream out, err;
        const int rc = run_app({"app", "-f", "no_such_file_xyz.txt",
                                "-o", OUT_FILE},
                               out, err);
        assert(rc != 0);
        assert(err.str().find("Failed to open input file") !=
               std::string::npos);
    }

    // 9. Parse error reports the correct line number.
    {
        write_file(IN_FILE, "\n\nBAD\n");
        std::ostringstream out, err;
        const int rc = run_app({"app", "-f", IN_FILE, "-o", OUT_FILE,
                                "-n", "1"},
                               out, err);
        assert(rc != 0);
        assert(err.str().find("Parse error on line 3") != std::string::npos);
        std::remove(IN_FILE);
    }

    // 10. Simple equation: result goes both to file and stdout.
    {
        write_file(IN_FILE, "A = {1, 2, 3}\n");
        std::ostringstream out, err;
        const int rc = run_app({"app", "-f", IN_FILE, "-o", OUT_FILE,
                                "-n", "2"},
                               out, err);
        assert(rc == 0);
        assert(err.str().empty());
        assert(out.str().find("A = { 1 2 3 }") != std::string::npos);

        const std::string file_content = read_file(OUT_FILE);
        assert(file_content.find("A = { 1 2 3 }") != std::string::npos);
        assert(file_content == out.str());
        std::remove(IN_FILE);
        std::remove(OUT_FILE);
    }

    // 11. Empty input file succeeds with empty output.
    {
        write_file(IN_FILE, "");
        std::ostringstream out, err;
        const int rc = run_app({"app", "-f", IN_FILE, "-o", OUT_FILE,
                                "-n", "1"},
                               out, err);
        assert(rc == 0);
        assert(out.str().empty());
        assert(read_file(OUT_FILE).empty());
        std::remove(IN_FILE);
        std::remove(OUT_FILE);
    }

    // 12. Blank lines are skipped.
    {
        write_file(IN_FILE, "\n\nA = {1}\n\n\nB = {2}\n\n");
        std::ostringstream out, err;
        const int rc = run_app({"app", "-f", IN_FILE, "-o", OUT_FILE,
                                "-n", "1"},
                               out, err);
        assert(rc == 0);
        assert(out.str().find("A = { 1 }") != std::string::npos);
        assert(out.str().find("B = { 2 }") != std::string::npos);
        std::remove(IN_FILE);
        std::remove(OUT_FILE);
    }

    // 13. Multiple iterations converge, output sorted by name.
    {
        write_file(IN_FILE, "A = {1}\nB = A | {2}\nC = B | {3}\n");
        std::ostringstream out, err;
        const int rc = run_app({"app", "-f", IN_FILE, "-o", OUT_FILE,
                                "-n", "5"},
                               out, err);
        assert(rc == 0);
        assert(out.str().find("A = { 1 }") != std::string::npos);
        assert(out.str().find("B = { 1 2 }") != std::string::npos);
        assert(out.str().find("C = { 1 2 3 }") != std::string::npos);
        assert(read_file(OUT_FILE) == out.str());
        std::remove(IN_FILE);
        std::remove(OUT_FILE);
    }

    // 14. Without args and without a default input file, fails cleanly.
    {
        std::ifstream default_check("gcu_input.txt");
        if (!default_check)
        {
            std::ostringstream out, err;
            const int rc = run_app({"app"}, out, err);
            assert(rc != 0);
            assert(err.str().find("Failed to open input file") !=
                   std::string::npos);
        }
    }

    std::cout << "All Application tests passed\n";
    return 0;
}