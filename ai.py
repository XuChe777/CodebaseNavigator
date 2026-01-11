from typing import Dict, List, Any
import json
import hashlib

CACHE_PATH = "AST.json"


def load_cache() -> Dict[str, Any]:
    try:
        with open(CACHE_PATH, "r") as f:
            return json.load(f)
    except FileNotFoundError:
        return {}


def save_cache(cache: Dict[str, Any]) -> None:
    with open(CACHE_PATH, "w") as f:
        json.dump(cache, f, indent=2)


def hash_function_source(src: str) -> str:
    return hashlib.sha256(src.encode("utf-8")).hexdigest()


def collect_known_callees(calls: List[str], cache: Dict[str, Any]) -> List[Dict[str, Any]]:
    callees = []

    for name in calls:
        entry = cache.get(name)
        if not entry:
            continue

        callees.append({
            "name": name,
            "summary": entry.get("summary", "unknown"),
            "side_effects": entry.get("side_effects", []),
            "confidence": entry.get("confidence", "low"),
        })

    return callees


SYSTEM_PROMPT = """You are a static analysis assistant for C programs.

You are given:
- Exact C source code
- Extracted symbol metadata
- Partial project context

Rules:
- Do not assume behavior not visible in the provided code.
- Do not invent functions, globals, or side effects.
- If information is insufficient, say "unknown".
- Respond ONLY in valid JSON following the schema.
- Do not include explanations outside JSON.
"""


def build_user_prompt(func_json: Dict[str, Any], known_callees: List[Dict[str, Any]]) -> str:
    payload = {
        "function": func_json,
        "known_callees": known_callees
    }

    return f"""
Analyze the following C function.

Context:
- Language: C
- Headers may be missing
- Globals may be unknown

Input JSON:
{json.dumps(payload, indent=2)}

Tasks:
1. Summarize the function's purpose.
2. Identify inputs and outputs.
3. Identify observable side effects.
4. Identify risks or edge cases.

If information is missing, use "unknown".
"""


def call_llm(system_prompt: str, user_prompt: str) -> Dict[str, Any]:
    """
    Replace this with the real API call.
    Must return parsed JSON (dict).
    """
    raise NotImplementedError


def analyze_function(func_json: Dict[str, Any]) -> Dict[str, Any]:
    cache = load_cache()

    source = func_json.get("function_definition", "")
    func_name = func_json.get("identifier", "unknown")
    calls = func_json.get("calls", [])

    src_hash = hash_function_source(source)

    # Cache hit
    if src_hash in cache:
        return cache[src_hash]

    known_callees = collect_known_callees(calls, cache)
    user_prompt = build_user_prompt(func_json, known_callees)

    result = call_llm(SYSTEM_PROMPT, user_prompt)

    # Store minimal, reusable info
    cache[src_hash] = {
        "name": func_name,
        "summary": result.get("summary", "unknown"),
        "side_effects": result.get("side_effects", []),
        "risks": result.get("risks", []),
        "confidence": result.get("confidence", "low"),
    }

    save_cache(cache)
    return result


def assert_valid_result(obj: Dict[str, Any]) -> None:
    required = {
        "summary",
        "inputs",
        "outputs",
        "side_effects",
        "calls",
        "risks",
        "confidence"
    }

    missing = required - obj.keys()
    if missing:
        raise ValueError(f"Missing keys: {missing}")
