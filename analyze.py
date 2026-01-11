#!/usr/bin/env python3
"""
Simple AI-assisted function analyzer (standalone).

No external dependencies.
Safe for beginners.
"""

import json
import hashlib
import sys
import os

CACHE_FILE = "cache.json"
OUTPUT_FILE = "analysis.json"

# ------------------------------------------------------------
# Utility functions
# ------------------------------------------------------------


def load_json_file(path):
    with open(path, "r") as f:
        return json.load(f)


def save_json_file(path, data):
    with open(path, "w") as f:
        json.dump(data, f, indent=2)


def load_cache():
    if not os.path.exists(CACHE_FILE):
        return {}
    return load_json_file(CACHE_FILE)


def save_cache(cache):
    save_json_file(CACHE_FILE, cache)


def hash_source(text):
    return hashlib.sha256(text.encode("utf-8")).hexdigest()

# ------------------------------------------------------------
# Prompt construction
# ------------------------------------------------------------


SYSTEM_PROMPT = (
    "You are a static analysis assistant for C programs.\n"
    "Rules:\n"
    "- Do not assume behavior not visible in the code.\n"
    "- Do not invent functions or globals.\n"
    "- If unsure, say \"unknown\".\n"
    "- Respond ONLY with valid JSON.\n"
)


def build_user_prompt(function_json, known_callees):
    payload = {
        "function": function_json,
        "known_callees": known_callees
    }

    return (
        "Analyze the following C function.\n\n"
        "Context:\n"
        "- Language: C\n"
        "- Headers may be missing\n"
        "- Globals may be unknown\n\n"
        "Input JSON:\n"
        + json.dumps(payload, indent=2)
        + "\n\nTasks:\n"
        "1. Summarize the function's purpose.\n"
        "2. Identify inputs and outputs.\n"
        "3. Identify side effects.\n"
        "4. Identify risks.\n"
        "\nIf information is missing, use \"unknown\".\n"
    )

# ------------------------------------------------------------
# MOCK AI (replace later)
# ------------------------------------------------------------


def call_ai(system_prompt, user_prompt):
    """
    Fake AI response so the pipeline works end-to-end.

    Replace this function later with:
    - OpenAI
    - Local LLM
    - Server API
    """

    # VERY naive "analysis"
    return {
        "summary": "Initializes a data structure.",
        "inputs": [
            {
                "name": "table",
                "type": "SymbolTable *",
                "description": "Pointer to structure to initialize."
            }
        ],
        "outputs": [
            {
                "type": "void",
                "description": "No return value."
            }
        ],
        "side_effects": [
            "Writes to memory pointed to by input parameter."
        ],
        "calls": ["memset"],
        "risks": [
            "Undefined behavior if input pointer is NULL."
        ],
        "confidence": "medium"
    }

# ------------------------------------------------------------
# Core analysis logic
# ------------------------------------------------------------


def analyze_function(func):
    cache = load_cache()

    source = func.get("function_definition", "")
    name = func.get("identifier", "unknown")
    calls = func.get("calls", [])

    if not source:
        raise ValueError("Missing function_definition")

    key = hash_source(source)

    # Cache hit
    if key in cache:
        print(f"[cache] Using cached result for {name}")
        return cache[key]

    # Collect known callees from cache
    known_callees = []
    for c in calls:
        if c in cache:
            known_callees.append({
                "name": c,
                "summary": cache[c].get("summary", "unknown"),
                "side_effects": cache[c].get("side_effects", []),
                "confidence": cache[c].get("confidence", "low")
            })

    user_prompt = build_user_prompt(func, known_callees)

    # Call AI
    result = call_ai(SYSTEM_PROMPT, user_prompt)

    # Minimal validation
    required_keys = [
        "summary", "inputs", "outputs",
        "side_effects", "calls",
        "risks", "confidence"
    ]

    for k in required_keys:
        if k not in result:
            raise ValueError(f"AI result missing key: {k}")

    # Store in cache
    cache[key] = result
    save_cache(cache)

    return result

# ------------------------------------------------------------
# Main entry point
# ------------------------------------------------------------


def main():
    if len(sys.argv) != 2:
        print("Usage: python3 analyze.py input.json")
        sys.exit(1)

    input_path = sys.argv[1]

    try:
        func_json = load_json_file(input_path)
        analysis = analyze_function(func_json)
        save_json_file(OUTPUT_FILE, analysis)
        print(f"[ok] Analysis written to {OUTPUT_FILE}")
    except Exception as e:
        print("[error]", e)
        sys.exit(1)


if __name__ == "__main__":
    main()
