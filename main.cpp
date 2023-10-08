// Standard Inlcudes
#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <streambuf>
#include <algorithm>

// 3rd party
#include <fplus/fplus.hpp>
// Custom Includes
#include "info.h"

namespace fs = std::filesystem;

enum class BP_COMMAND
{
    TARGET_DIR,
    FILE,
    RENAME,
    REPLACE,
    INVALID
};

fs::path target_dir{"."};
fs::path root_path{BP_PREFIX_PATH "/" BP_INSTALL_PATH "/templates"};

BP_COMMAND to_bp_command(std::string cmd)
{
    return cmd == "FILE" ? BP_COMMAND::FILE : cmd == "RENAME"   ? BP_COMMAND::RENAME
                                          : cmd == "REPLACE"    ? BP_COMMAND::REPLACE
                                          : cmd == "TARGET_DIR" ? BP_COMMAND::TARGET_DIR
                                                                : BP_COMMAND::INVALID;
}

void printHelp()
{
    std::cout << "Boilerplate version "
              << BP_VERSION << "\n\n";

    std::cout << "\nConfiguration:\n";
    std::cout << "--print-root\n";
    std::cout << "prints the root of the categories and actions repository.\n";

    std::cout << "\nUsage:\n";
    std::cout << "boilerplate <CAT> <ACTION> <OPT_ARG>\n";
    std::cout << "<CAT>: the category which refers to the parent folder of the action to be executed.\n";
    std::cout << "<ACTION>: the folder name that has the recipe file.\n";
    std::cout << "<OPT_ARG>: an optional argument that can be used inside the recipe file. you can use it with the $1 notation.\n";
    std::cout << "\n";

    std::cout << "Example:\n";
    std::cout << "boilerplate cmake module my_module\n";
    std::cout << "the tool will look for the file cmake/module/recipe.txt under the root path to execute the recipe commands.\n";
}

std::string evaluate_exp(std::string exp, std::string opt_arg1)
{
    /*
    * Assumptions:
        exp has a vaild path
        some path contents could be a dollar sign expression like $1
    */

    return fplus::replace_tokens({"$1"}, opt_arg1, exp);
}

void execute_recipe_file_cmd(fs::path root, std::string from)
{
    const auto dest = "." / target_dir;
    fs::copy(root / from, dest, fs::copy_options::update_existing);
}

void execute_recipe_dir_cmd(fs::path root, std::string dir_name, std::string opt_arg1)
{
    const auto cur_path = fs::current_path();
    target_dir = evaluate_exp(dir_name, opt_arg1);
    std::cout << "creating directory: " << dir_name << "\n";
    std::cout << "creating directory: " << target_dir << "\n";

    fs::create_directory(target_dir);
}

void execute_recipe_rename_cmd(std::string old_name, std::string new_name, std::string opt_arg1)
{
    fs::rename("." / target_dir / old_name, "." / target_dir / evaluate_exp(new_name, opt_arg1));
}

void execute_recipe_replace_cmd(std::string file, std::string old_token, std::string new_token, std::string opt_arg1)
{
    file = evaluate_exp(file, opt_arg1);
    old_token = evaluate_exp(old_token, opt_arg1);
    new_token = evaluate_exp(new_token, opt_arg1);
    // read file contents
    std::string cmake_file_contents;
    {
        std::ifstream cmake_file{"." / target_dir / file};
        cmake_file_contents = std::string((std::istreambuf_iterator<char>(cmake_file)),
                                          std::istreambuf_iterator<char>());
    }

    std::cout << "replacing " << old_token << " with " << evaluate_exp(new_token, opt_arg1) << "\n";

    // replace
    cmake_file_contents = fplus::replace_tokens(old_token, new_token, cmake_file_contents);

    // update file contents
    {
        std::ofstream cmake_file{"." / target_dir / file};
        cmake_file << cmake_file_contents;
    }
}

void execute_line(fs::path root, std::string cmd, std::string opt_arg1)
{
    const auto full_command = fplus::split(' ', false, cmd);
    const auto command = to_bp_command(full_command[0]);

    switch (command)
    {
    case BP_COMMAND::FILE:
        execute_recipe_file_cmd(root, full_command[1]);
        break;
    case BP_COMMAND::TARGET_DIR:
        execute_recipe_dir_cmd(root, full_command[1], opt_arg1);
        break;
    case BP_COMMAND::RENAME:
        execute_recipe_rename_cmd(full_command[1], full_command[2], opt_arg1);
        break;
    case BP_COMMAND::REPLACE:
        execute_recipe_replace_cmd(full_command[1], full_command[2], full_command[3], opt_arg1);
        break;
    default:
        std::cout << "Invalid command: \"" << full_command[0] << "\" please try --help for full list of commands."
                  << "\n";
        break;
    }
    std::cout
        << "executing " << cmd << "\n";
}

int main(const int argc, char **argv)
{
    if (argc < 2)
    {
        printHelp();
        return 0;
    }

    if (std::string{argv[1]} == std::string{"-h"} || std::string{argv[1]} == std::string{"--help"})
    {
        printHelp();
        std::cout
            << "Invalid arguement number, please try -h or --help for help";
        return 0;
    }

    if (std::string{argv[1]} == std::string{"--print-root"})
    {
        std::cout << "Current root path is: " << root_path << "\n";
        return 0;
    }

    const std::string language{argv[1]};
    const std::string action{argv[2]};
    const std::string opt_arg{argv[3]};

    std::cout << "language " << language << "\n";
    std::cout << "action " << action << "\n";

    // go to the action folder
    const auto tmplates_srcs = fs::path(root_path);
    if (!fs::exists(tmplates_srcs))
    {
        std::cout << "Couldn't find template root path! current path is: " << tmplates_srcs << "\n";
        std::cout << "Please check the root path using --print-root>";
        return 0;
    }
    else if (std::filesystem::is_empty(tmplates_srcs))
    {
        std::cout << "Templates root path is empty! please add some categories and actions.\n";
        std::cout << "Please check the root path using --print-root>";
        return 0;
    }

    const fs::path action_path = tmplates_srcs / language / action;
    const auto destination = fs::current_path();

    if (!fs::exists(action_path))
    {
        std::cout << action_path << " dosn't exist\n Please add a template folder for it.\n";
        return 0;
    }

    // read the recipe
    std::ifstream recipe = std::ifstream(action_path / "recipe.txt");
    std::string line;

    while (std::getline(recipe, line))
    {
        // ignore blanks
        if (line.length() > 0 && line[0] != '#')
            execute_line(action_path, line, opt_arg);
    }

    return 0;
}