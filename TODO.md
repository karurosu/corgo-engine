Non-exhaustive scratchpad-like list of features needing implementation and ideas.
Mostly so I don't forget.

TODOs:
- SceneTree ordering
- Optimize auto ordering
- use builtin_ctz for iteraring over entities
- cache parent entity and transform
- fast internal access to systems
- refactor storage cleanup to properly set the entity context
- Add zero storage components, aka flags

Ideas:
- expand max entities to 512
- Add a debug phase for systems that gets deactivated for release builds and can be toggled off

Future:
- Support for multiple copies of the same component/relationship
- Grow component storage dynamically
- make a better global system runner, may need a better codegen/preprocessor
- dynamic bitsets
