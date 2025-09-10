import sys
import os
import json
import re

from typing import List, Dict, Any

def replace(text: str, vars_map: Dict[str, Any]) -> str:
    pattern = re.compile(r"\{\{\s*([A-Za-z0-9_]+)\s*\}\}")

    def replacer(match):
        var_name = match.group(1)
        value = vars_map.get(var_name)

        if value is not None:
            print(f"Replacing {{ {var_name} }} with '{value}'")
            return str(value)
        else:
            print(f"No value for {{ {var_name} }}, keeping original")
            return match.group(0)

    return pattern.sub(replacer, text)

def main(arg: List[str]) -> int:
    vars_map = json.loads(os.getenv('VARS_JSON'))

    for filename in arg:
        with open(filename, 'r') as file:
            content: str = file.read()

        replaced = replace(content, vars_map)

        with open(filename, "w") as file:
            file.write(replaced)

        print(f"Processed {filename}")
    return 0

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage:")
        print("\tpython replace.py <file1> [<file2> ...]")
        sys.exit(1)

    sys.exit(main(sys.argv[1:]))