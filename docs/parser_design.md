# Parser Design {#parser_design}

The parser uses a stack-based state machine where each state is a class
inheriting from `parser_state`. This design prevents invalid token
combinations through type-safe state transitions.

## State Machine Overview

The parser maintains a stack of states. The root state (`module_scope_state`)
sits at the bottom. As tokens arrive, the current state (top of stack)
processes them and returns one of three results:

| Result | Action |
|--------|--------|
| `stay()` | Absorb token, remain in current state |
| `push(child)` | Push child state onto stack |
| `complete(node)` | Pop state, pass AST to parent |

The `.reprocess()` modifier causes the token to be re-examined by the
next handler instead of being consumed.

## State Machine Root

\dotfile module_scope_state.dot "Root State Machine (truncated)"

