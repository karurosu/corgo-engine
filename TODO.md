Non-exhaustive scratchpad-like list of features needing implementation and ideas.
Mostly so I don't forget.

TODOs:
- SceneTree ordering
- Cleanup relationships on entity destroy
- Optimize auto ordering

Ideas:
- Change component metadata to have a bitset to track flags (requires a variable sized bitset)
- Enforce reciprocal relationships? (how to avoid dangling relationships?)

Future:
- Support for multiple copies of the same component/relationship
- Grow component storage dynamically
