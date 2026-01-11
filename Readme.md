# Codebase Analyzer
An AI agent that understands your entire codebase.

## Dependencies
- curl
- cJSON
- tree-sitter
- tree-sitter-c
- ninja
- gcc

If you have vcpkg configured then you can simply run `vcpkg install` at project root.

## Configure
Debugging preset:
```bash
cmake --preset debug
```

Release preset:
```bash
cmake --preset release
```

## Compiling
```bash
cmake --build ./build
```
The application's executable is at `./build/cba`
`cba` takes one command line argument `file_path`
