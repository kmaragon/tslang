#!/usr/bin/env python3
"""Generate per-state Graphviz DOT files from parser transition logs.

Reads a tab-separated transition log (source\\ttoken\\ttarget) produced by
the instrumented test runner, deduplicates, and generates one DOT file per
source state. Depth and per-pattern overrides are controlled by a YAML config.

Usage:
    python3 gen_diagrams.py --transitions transitions.log \
                            --config diagram_config.yaml \
                            --output-dir build/docs/diagrams/
"""

import argparse
import fnmatch
import os
import sys
from collections import defaultdict
from pathlib import Path

try:
    import yaml
except ImportError:
    yaml = None


def load_config(config_path):
    """Load YAML config, or return defaults if no config or no PyYAML."""
    defaults = {"depth": 2, "overrides": {}}
    if not config_path or not os.path.exists(config_path):
        return defaults
    if yaml is None:
        print(
            "Warning: PyYAML not installed, using default config",
            file=sys.stderr,
        )
        return defaults
    with open(config_path) as f:
        cfg = yaml.safe_load(f) or {}
    return {
        "depth": cfg.get("depth", 2),
        "overrides": cfg.get("overrides", {}),
    }


def get_depth_for_state(state_name, config):
    """Return the configured depth for a state, checking overrides."""
    for override in config["overrides"].values():
        pattern = override.get("pattern", "")
        if isinstance(pattern, list):
            patterns = pattern
        else:
            patterns = [pattern]
        for p in patterns:
            if fnmatch.fnmatch(state_name, p):
                return override.get("depth", config["depth"])
    return config["depth"]


def parse_events(transitions_path):
    """Parse the transition log into ordered events per thread.

    Lines have the format: thread_id\\tkind\\tsource\\ttoken\\ttarget
    where kind is "push" or "complete".

    Returns a dict: thread_id -> [(kind, source, token, target), ...].
    Events within each thread preserve file order. Stable sort by
    thread_id groups interleaved threads without breaking ordering.
    """
    events_by_thread = defaultdict(list)
    with open(transitions_path) as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            parts = line.split("\t")
            if len(parts) != 5:
                continue
            thread_id, kind, source, token, target = parts
            events_by_thread[thread_id].append((kind, source, token, target))
    return events_by_thread


def _pop_to(stack, state_name):
    """Pop the stack down to and including state_name.

    Some states complete with null nodes and are never logged as
    complete events. When we see a complete for a state deeper in
    the stack, we need to pop everything above it too.
    """
    try:
        idx = stack.index(state_name)
        del stack[idx:]
    except ValueError:
        pass


def compute_return_edges(events_by_thread):
    """Replay events per thread to compute return edges.

    Maintains a virtual stack to track parent-child relationships.
    Each complete event emits a return edge from the completing state
    to its parent (the new stack top after popping). Cascading
    completions each get their own return edge rather than being
    collapsed, so every level is visible within its BFS depth.

    States that complete with null nodes are invisible in the log
    (the observer skips them). The stack handles this by popping
    down to the completing state, discarding any unlogged states
    above it.
    """
    return_edges = set()

    for events in events_by_thread.values():
        stack = []

        for kind, source, token, target in events:
            if kind == "push":
                # If source isn't on the stack, a new test started
                if source not in stack:
                    stack = [source]
                elif stack[-1] != source:
                    # Source is on stack but not at top — intermediate
                    # states completed silently (null nodes). Pop down.
                    _pop_to(stack, source)
                    stack.append(source)
                stack.append(target)

            elif kind in ("complete", "cascade"):
                _pop_to(stack, source)
                if stack:
                    return_edges.add((source, token, stack[-1]))

    return return_edges


def load_transitions(transitions_path):
    """Load transitions and compute return edges from the log file.

    Parses push/complete events, replays them on a virtual stack to
    derive return edges (child -> parent after completion), then builds
    a deduplicated graph: source_state -> set of (token, target, kind).
    """
    events_by_thread = parse_events(transitions_path)
    return_edges = compute_return_edges(events_by_thread)

    graph = defaultdict(set)

    # Add push and complete edges. Cascade events and <null> completions
    # are only used for return edge computation, not as graph edges.
    for events in events_by_thread.values():
        for kind, source, token, target in events:
            if kind == "cascade":
                continue
            if kind == "complete" and target == "<null>":
                continue
            graph[source].add((token, target, kind))

    # Add return edges
    for child, token, parent in return_edges:
        graph[child].add((token, parent, "return"))

    return graph


def collect_reachable(graph, start, max_depth):
    """BFS from start up to max_depth levels, returning edges to include."""
    edges = set()
    visited = set()
    frontier = {start}

    for depth in range(max_depth):
        next_frontier = set()
        for state in frontier:
            if state in visited:
                continue
            visited.add(state)
            for token, target, kind in graph.get(state, set()):
                if kind != "return":
                    edges.add((state, token, target, kind))
                    if kind == "push" and target not in visited:
                        next_frontier.add(target)
        frontier = next_frontier

    # Add return edges only when target is a visited state (reachable
    # from this root). Otherwise we'd reference states from other
    # contexts that this state happens to participate in.
    for state in visited:
        for token, target, kind in graph.get(state, set()):
            if kind == "return" and target in visited:
                edges.add((state, token, target, kind))

    return edges


def generate_dot(state_name, edges):
    """Generate DOT content for a single state's transition graph."""
    lines = []
    lines.append(f'digraph "{state_name}" {{')
    lines.append("    rankdir=LR;")
    lines.append('    node [shape=box, style=rounded, fontname="Helvetica"];')
    lines.append('    edge [fontname="Helvetica", fontsize=10];')
    lines.append("")

    # Highlight the root state
    lines.append(f'    "{state_name}" [style="rounded,filled", fillcolor="#e8f4fd"];')

    # Collect AST node targets (from complete edges) for different styling
    node_targets = set()
    for source, token, target, kind in edges:
        if kind == "complete":
            node_targets.add(target)

    # Style AST node targets with a different shape
    for node in sorted(node_targets):
        lines.append(
            f'    "{node}" [shape=ellipse, style=filled, fillcolor="#f0e6ff"];'
        )

    lines.append("")

    for source, token, target, kind in sorted(edges):
        label = token
        if label.endswith("_token"):
            label = label[: -len("_token")]

        if kind == "complete":
            lines.append(
                f'    "{source}" -> "{target}" '
                f'[label="{label}", style=dashed, color="#8855aa"];'
            )
        elif kind == "return":
            lines.append(
                f'    "{source}" -> "{target}" '
                f'[label="{label}", style=dotted, color="#338833"];'
            )
        else:
            lines.append(f'    "{source}" -> "{target}" [label="{label}"];')

    lines.append("}")
    return "\n".join(lines)


def main():
    parser = argparse.ArgumentParser(
        description="Generate per-state DOT files from transition logs"
    )
    parser.add_argument(
        "--transitions", required=True, help="Path to transitions.log"
    )
    parser.add_argument("--config", help="Path to diagram_config.yaml")
    parser.add_argument(
        "--output-dir", required=True, help="Directory for generated .dot files"
    )
    args = parser.parse_args()

    config = load_config(args.config)
    graph = load_transitions(args.transitions)

    os.makedirs(args.output_dir, exist_ok=True)

    for state_name in sorted(graph.keys()):
        depth = get_depth_for_state(state_name, config)
        edges = collect_reachable(graph, state_name, depth)
        if not edges:
            continue

        dot_content = generate_dot(state_name, edges)
        output_path = Path(args.output_dir) / f"{state_name}.dot"
        with open(output_path, "w") as f:
            f.write(dot_content)
            f.write("\n")

    print(f"Generated {len(graph)} DOT files in {args.output_dir}")


if __name__ == "__main__":
    main()
