# Initial Version of Boilerplate Tool

The **Boilerplate Tool** is intended to be used as a lightweight boilerplate code generator. You can organize your boilerplate code under the root path (use `--print-root`) into main category folders and subfolders called actions.

For example, you can add a new path "my_template/my_action" under the root path, and then call `boilerplate my_template my_action` to execute the recipe inside the `my_action` folder. You can check the `cmake/module` recipe file as an example.

## Usage

### Configuration

To print the root path, use the following command:

```sh
boilerplate --print-root
```

### Generation

To generate boilerplate code, use the following command:

```sh
boilerplate <CAT> <CMD> <OPT>
```
- CAT: Category of the command to be executed.
- CMD: Specific command under the category.
- OPT: Additional optional arguments specific to the command.

## Build and Installation

Inside the project root folder:

```sh
cmake -S . -B ./build
cmake --build ./build
sudo cmake --install ./install
```

