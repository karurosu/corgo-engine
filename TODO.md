Non-exhaustive scratchpad-like list of features needing implementation and ideas.
Mostly so I don't forget.

TODOs:
- SceneTree ordering
- Optimize auto ordering
- use builtin_ctz for iteraring over entities

Ideas:
- Change component metadata to have a bitset to track flags (requires a variable sized bitset)
- Add flags for entities, lightweight empty components that can be used to trigger systems
- expand max entities to 512
- separate global render system
- small cache for transform components per entity

Future:
- Support for multiple copies of the same component/relationship
- Grow component storage dynamically
- make a better global system runner, may need a better codegen/preprocessor
- dynamic bitsets
