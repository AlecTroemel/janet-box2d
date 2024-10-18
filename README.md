# Janet Box2d
Janet bindings to Box2d. The bindings are faithful to the original C API, especially where it makes sense,
but function names have been "lispified" (kebab case, question marks instead of the word "Is", etc.).
See api.txt for an overview of which API functions have been ported.

## Building

Clone the repo with submodules, or use git submodule update --init --recursive.

```bash
jpm build
```
