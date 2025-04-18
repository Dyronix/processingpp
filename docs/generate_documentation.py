import subprocess
import sys
import os
from pathlib import Path

"""
Wrapper to call JSON and Markdown generators in sequence:
1. generate_json.py -- parses headers into JSON
2. generate_md.py   -- converts JSON into Markdown

Usage:
    generate_documentation.py <src_dir> <docs_dir>
"""

def main():
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <src_dir> <docs_dir>")
        sys.exit(1)

    # Resolve directories
    src_dir = os.path.abspath(sys.argv[1])
    docs_dir = os.path.abspath(sys.argv[2])
    script_dir = os.path.abspath(os.path.dirname(__file__))

    # Prepare intermediate JSON output
    intermediate_dir = os.path.join(script_dir, 'intermediate')
    os.makedirs(intermediate_dir, exist_ok=True)

    # Paths to helper scripts
    json_script = os.path.join(script_dir, 'generate_json.py')
    md_script   = os.path.join(script_dir, 'generate_md.py')

    # Step 1: JSON generation
    print(f"[1/2] Generating JSON into {intermediate_dir}...")
    subprocess.check_call([
        sys.executable, json_script,
        src_dir,
        intermediate_dir
    ])

    # Step 2: Markdown generation
    print(f"[2/2] Generating Markdown into {docs_dir}...")
    os.makedirs(docs_dir, exist_ok=True)
    subprocess.check_call([
        sys.executable, md_script,
        intermediate_dir,
        docs_dir
    ])

    print("Documentation generation complete.")

if __name__ == '__main__':
    main()