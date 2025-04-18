import json
import argparse
from pathlib import Path
import re

def extract_member(decl: str):
    decl_clean = decl.split('=')[0].strip().rstrip(';').strip()
    parts = decl_clean.split()
    if len(parts) >= 2:
        return parts[-1], ' '.join(parts[:-1])
    return decl_clean, ''

def extract_method(decl: str):
    sig = decl.strip().rstrip(';')
    m = re.match(r'[\w:<>]+\s+(\w+)\s*\(', sig)
    name = m.group(1) if m else sig
    return name, sig

def generate_markdown(json_data):
    lines = []

    # Title
    if 'file' in json_data:
        lines.append(f"# {json_data['file']}")
        lines.append('')

    # CSS for styling
    lines.append("<style>")
    lines.append("  .file-summary { font-size: 1.2em; font-style: italic; margin-bottom: 1em; }")
    lines.append("  table { width: 100%; table-layout: fixed; border-collapse: collapse; }")
    lines.append("  th, td { border: 1px solid #ddd; padding: 8px; word-wrap: break-word; }")
    lines.append("  th { background-color: #f4f4f4; }")
    lines.append("</style>")
    lines.append("")

    # File description
    if 'file_doc' in json_data and 'description' in json_data['file_doc']:
        lines.append(f"<p class=\"file-summary\">{json_data['file_doc']['description']}</p>")
        lines.append('')

    for ns, content in json_data.get('namespaces', {}).items():
        lines.append('<hr style="border-top:5px solid #aaa" />')
        lines.append('')
        lines.append(f"## `{ns}`")
        lines.append('')

        if content.get('types'):
            lines.append("### Types")
            lines.append('')

        for type_info in content.get('types', {}).values():
            kind = type_info.get('kind', '')
            name = type_info.get('name', '')

            lines.append('<hr style="border-top:3px solid #ccc" />')
            lines.append('')
            lines.append(f"#### {kind} `{name}`")
            lines.append('')

            # Description
            desc = type_info.get('comment', {}).get('description')
            if desc:
                lines.append("**Description:**")
                lines.append("```")
                lines.append(desc)
                lines.append("```")
                lines.append('')

            # Struct fields/methods
            if kind.lower() == 'struct':
                fields = [m for m in type_info.get('members', []) if '(' not in m['declaration']]
                if fields:
                    lines.append("**Fields:**")
                    lines.append('')
                    lines.append("| Member | Type | Description |")
                    lines.append("|---|---|---|")
                    for m in fields:
                        mname, mtype = extract_member(m['declaration'])
                        mdesc = m.get('doc', {}).get('description', '')
                        lines.append(f"| `{mname}` | `{mtype}` | {mdesc} |")
                    lines.append('')

                methods = [m for m in type_info.get('members', []) if '(' in m['declaration']]
                if methods:
                    lines.append("**Methods:**")
                    lines.append('')
                    lines.append("| Method | Signature | Description |")
                    lines.append("|---|---|---|")
                    for m in methods:
                        mname, sig = extract_method(m['declaration'])
                        mdesc = m.get('doc', {}).get('description', '')
                        lines.append(f"| `{mname}` | `{sig}` | {mdesc} |")
                    lines.append('')

            # Enum and struct syntax last
            lines.append("**Syntax:**")
            lines.append("```cpp")
            if kind.lower().startswith('enum'):
                # Multi-line enum syntax
                lines.append(f"enum class {name} {{")
                for key in type_info.get('keys', []):
                    lines.append(f"  {key},")
                lines.append("};")
            else:
                decl_lines = [f"struct {name} {{"] + \
                             [f"  {m['declaration'].strip()}" for m in type_info.get('members', [])] + \
                             ["};"]
                lines.extend(decl_lines)
            lines.append("```")
            lines.append('')
            lines.append('<hr style="border-top:1px solid #eee" />')
            lines.append('')

        # Functions
        globals_ = content.get('globals', [])
        if globals_:
            lines.append("### Functions")
            lines.append('<hr style="border-top:3px solid #ccc" />')
            lines.append('')
            for func in globals_:
                name = func.get('name', '')
                desc = func.get('doc', {}).get('description')
                params = func.get('doc', {}).get('params', [])
                ret = func.get('doc', {}).get('return')
                sig = func.get('declaration', '').strip()

                lines.append(f"#### {name}")
                lines.append('')

                if desc:
                    lines.append("**Description:**")
                    lines.append("```")
                    lines.append(desc)
                    lines.append("```")
                    lines.append('')

                if params:
                    lines.append("**Parameters:**")
                    lines.append('')
                    for p in params:
                        lines.append(f"- `{p['name']}`: {p['description']}")
                    lines.append('')

                if ret:
                    lines.append("**Returns:**")
                    lines.append("```")
                    lines.append(ret)
                    lines.append("```")
                    lines.append('')

                lines.append("**Syntax:**")
                lines.append("```cpp")
                lines.append(sig)
                lines.append("```")
                lines.append('')
                lines.append('<hr style="border-top:1px solid #eee" />')
                lines.append('')

    return "\n".join(lines)

def main():
    parser = argparse.ArgumentParser(description="Generate Markdown references from JSON descriptors.")
    parser.add_argument("input_folder", help="Folder with JSON files")
    parser.add_argument("output_folder", help="Folder for Markdown output")
    args = parser.parse_args()

    inp = Path(args.input_folder)
    out = Path(args.output_folder)
    out.mkdir(parents=True, exist_ok=True)

    for jf in inp.glob("*.json"):
        data = json.loads(jf.read_text())
        md = generate_markdown(data)
        md_file = out / f"{jf.stem}.md"
        md_file.write_text(md)
        print(f"Generated {md_file}")

if __name__ == "__main__":
    main()
