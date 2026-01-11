import json
from typing import List, Dict

# ----------------------------
# Configuration
# ----------------------------

MODEL_NAME = "gpt-4.1-nano"  # replace later

# ----------------------------
# Load codebase context
# ----------------------------


def load_codebase_json(path: str) -> str:
    with open(path, "r", encoding="utf-8") as f:
        return f.read()

# ----------------------------
# Build system prompt
# ----------------------------


def build_system_prompt(codebase_json: str) -> str:
    return f"""
You are a codebase-aware chatbot.

You are given a JSON description of a codebase.
This JSON is the single source of truth.

Rules:
- Do NOT invent symbols.
- Use exact names from the JSON.
- If a question cannot be answered using the JSON, say so clearly.

Codebase JSON:
{codebase_json}
"""

# ----------------------------
# Chat loop
# ----------------------------


def chat():
    from openai import OpenAI
    # client = OpenAI(api_key=API_KEY)
    client = OpenAI()

    codebase_json = load_codebase_json("AST.json")
    system_prompt = build_system_prompt(codebase_json)

    messages: List[Dict[str, str]] = [
        {"role": "system", "content": system_prompt}
    ]

    print("Codebase chatbot ready. Type 'exit' to quit.\n")

    while True:
        user_input = input(">>> ")
        if user_input.lower() in {"exit", "quit"}:
            break

        messages.append({"role": "user", "content": user_input})

        response = client.chat.completions.create(
            model=MODEL_NAME,
            messages=messages,
            temperature=0.2
        )

        assistant_msg = response.choices[0].message.content
        print("\n" + assistant_msg + "\n")

        messages.append({"role": "assistant", "content": assistant_msg})

# ----------------------------
# Entry
# ----------------------------


if __name__ == "__main__":
    chat()
