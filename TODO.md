Non-exhaustive scratchpad-like list of features needing implementation and ideas.
Mostly so I don't forget.

Next:
- Platform timer abstraction
- Bitmaps

TODOs:
- Improve separation of game code 
- Optimize auto ordering
- use builtin_ctz for iteraring over entities
- fast internal access to systems
- refactor storage cleanup to properly set the entity context
- early opt out when there are no systems to tick
- Internal version of CE_ECS_MainStorage_getEntityData with less branches
- reduce error handling on non debug builds

Ideas:
- expand max entities to 512
- Add a debug phase for systems that gets deactivated for release builds and can be toggled off
- render culling based on 2d spatial hashing and bitsets
- Add a way to disable a global system (normal systems are ok)
- cache parent entity and transform

Future:
- Support for multiple copies of the same component/relationship
- Grow component storage dynamically
- make a better global system runner, may need a better codegen/preprocessor
- dynamic bitsets
