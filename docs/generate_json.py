"""
Enhanced Doxygen-to-JSON converter with detailed tag parsing and file-level detection

Scans a directory for .h files, extracts Doxygen /** */ blocks,
parses @file, @brief, @param, @return tags into structured JSON,
groups by namespace, types (struct/class/enum), and globals.
Writes one JSON per header into `intermediate/` with nested grouping.
"""
import os
import sys
import argparse
import re
import json

# -----------------------------------------------------------------------------
# Helper: split_top_level_decls
# Splits combined declarations into individual ones at semicolons outside braces.
# -----------------------------------------------------------------------------
def split_top_level_decls(decl):
    parts, cur, depth = [], [], 0
    for ch in decl:
        cur.append(ch)
        if ch == '{':
            depth += 1
        elif ch == '}':
            depth -= 1
        if ch == ';' and depth == 0:
            parts.append(''.join(cur).strip())
            cur = []
    return [p for p in parts if p]

# -----------------------------------------------------------------------------
# Function: parse_comment
# Parses Doxygen tags (@file,@brief,@param,@return) into a dict.
# -----------------------------------------------------------------------------
def parse_comment(lines):
    file_tag = None
    desc_lines = []
    params = []
    ret = None
    cur_param = None
    for line in lines:
        line = line.lstrip()
        if line.startswith('@file'):
            parts = line.split(None, 2)
            file_tag = parts[1].strip() if len(parts) > 1 else None
            cur_param = None
        elif line.startswith('@param'):
            parts = line.split(None, 2)
            name = parts[1] if len(parts) > 1 else ''
            text = parts[2] if len(parts) > 2 else ''
            params.append({'name': name, 'description': text.strip()})
            cur_param = params[-1]
        elif line.startswith('@return') or line.startswith('@returns'):
            parts = line.split(None, 1)
            ret = parts[1].strip() if len(parts) > 1 else ''
            cur_param = None
        elif line.startswith('@brief'):
            parts = line.split(None, 1)
            desc_lines.append(parts[1].strip() if len(parts) > 1 else '')
            cur_param = None
        elif line.startswith('@'):
            cur_param = None
        else:
            if cur_param:
                cur_param['description'] += ' ' + line.strip()
            else:
                desc_lines.append(line.strip())
    description = ' '.join(filter(None, desc_lines)) or None
    return {'description': description, 'params': params, 'return': ret, 'file_tag': file_tag}

# -----------------------------------------------------------------------------
# Function: extract_blocks
# Finds /** */ blocks and associates them with declarations or enum keys.
# -----------------------------------------------------------------------------
def extract_blocks(content):
    blocks = []
    for m in re.finditer(r"/\*\*(.*?)\*/", content, re.DOTALL):
        raw = m.group(1).splitlines()
        lines = [re.sub(r"^\s*\*\s?", "", l).strip() for l in raw]
        comment = parse_comment(lines)
        # file-level comment
        if comment.get('file_tag'):
            blocks.append({'start': m.start(), 'comment': comment, 'declaration': None})
            continue
        # Collect declaration snippet after the comment
        snippet = content[m.end():]
        decl_lines = []
        pdepth = bdepth = 0
        for line in snippet.splitlines():
            s = line.strip()
            if not s or s.startswith('//') or s.startswith('/*'):
                continue
            decl_lines.append(s)
            pdepth += s.count('(') - s.count(')')
            bdepth += s.count('{') - s.count('}')
            if pdepth <= 0 and bdepth <= 0 and (s.endswith(';') or s.endswith('}')):
                break
        raw_decl = ' '.join(decl_lines) if decl_lines else None
        # Struct/Class special case: full type definition
        struct_match = raw_decl and re.match(r"(struct|class)\s+(\w+)\s*\{", raw_decl)
        if struct_match:
            kind = struct_match.group(1)
            name = struct_match.group(2)
            # locate actual declaration position
            decl_pattern = re.compile(r"\b" + kind + r"\s+" + re.escape(name) + r"\b")
            m2 = decl_pattern.search(content, m.end())
            decl_pos = m2.start() if m2 else m.start()
            blocks.append({'start': decl_pos, 'comment': comment, 'declaration': raw_decl})
            continue
        # Enum special case: extract keys
        enum_match = raw_decl and re.match(r"enum(?:\s+class)?\s+(\w+)(?:\s*:\s*[^\{]+)?\s*\{", raw_decl)
        if enum_match:
            kind = 'enum class' if raw_decl.startswith('enum class') else 'enum'
            name = enum_match.group(1)
            inside = re.search(r"\{(.*)\}", raw_decl, re.DOTALL)
            keys = []
            if inside:
                for part in inside.group(1).split(','):
                    key = part.strip().split('=')[0].strip()
                    if key:
                        keys.append(key)
            blocks.append({'start': m.start(), 'comment': comment, 'kind': kind, 'name': name, 'keys': keys})
        elif raw_decl:
            # Split multiple declarations
            for d in split_top_level_decls(raw_decl):
                blocks.append({'start': m.start(), 'comment': comment, 'declaration': d})
        else:
            blocks.append({'start': m.start(), 'comment': comment, 'declaration': None})
    return blocks

# -----------------------------------------------------------------------------
# Functions: find_blocks, assign_fqns, find_container
# -----------------------------------------------------------------------------
def find_blocks(content, kind):
    patterns = {
        'namespace': re.compile(r'namespace\s+(\w+)\s*{'),
        'struct':    re.compile(r'struct\s+(\w+)\s*{'),
        'class':     re.compile(r'class\s+(\w+)\s*{'),
        'enum':      re.compile(r'enum(?:\s+class)?\s+(\w+)\s*{')
    }
    pattern = patterns[kind]
    blocks = []
    for m in pattern.finditer(content):
        name = m.group(1)
        depth = 1
        i = m.end()
        while depth and i < len(content):
            if content[i] == '{': depth += 1
            elif content[i] == '}': depth -= 1
            i += 1
        blocks.append({'kind': kind, 'name': name, 'start': m.start(), 'end': i})
    return blocks

def assign_fqns(blocks):
    for b in sorted(blocks, key=lambda x: x['start']):
        parents = [p for p in blocks if p['start'] < b['start'] < p['end']]
        b['fqname'] = (max(parents, key=lambda p: p['start'])['fqname'] + '::' + b['name']) if parents else b['name']
    return blocks

def find_container(pos, blocks):
    cand = [b for b in blocks if b['start'] <= pos < b['end']]
    return max(cand, key=lambda b: b['start']) if cand else None

# -----------------------------------------------------------------------------
# Main script
# -----------------------------------------------------------------------------
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Convert Doxygen to JSON with tag parsing.")
    parser.add_argument('src_dir', help="Directory with .h files to process")
    parser.add_argument('output_dir', nargs='?', help="Directory for JSON output (default: <src_dir>/intermediate)")
    args = parser.parse_args()

    # Resolve source and output directories
    src = os.path.abspath(args.src_dir)
    if args.output_dir:
        out_dir = os.path.abspath(args.output_dir)
    else:
        out_dir = os.path.join(src, 'intermediate')
    os.makedirs(out_dir, exist_ok=True)

    headers = [f for f in os.listdir(src) if f.endswith('.h')]
    for fn in headers:
        path = os.path.join(src, fn)
        content = open(path, encoding='utf-8').read()

        blocks = extract_blocks(content)
        ns_blocks = assign_fqns(find_blocks(content, 'namespace'))
        type_blocks = assign_fqns(
            find_blocks(content, 'struct') +
            find_blocks(content, 'class') +
            find_blocks(content, 'enum')
        )

        file_doc = None
        others = []
        for b in blocks:
            if file_doc is None and b['comment'].get('file_tag'):
                file_doc = {k: v for k, v in b['comment'].items() if k != 'file_tag'}
            else:
                others.append(b)

        hierarchy = {}
        for b in others:
            if 'keys' in b:
                ns = find_container(b['start'], ns_blocks)
                nsn = ns['fqname'] if ns else 'global'
                hierarchy.setdefault(nsn, {'types': {}, 'globals': []})
                hierarchy[nsn]['types'][b['name']] = {'kind': b['kind'], 'name': b['name'], 'fqname': b['name'], 'comment': b['comment'], 'keys': b['keys'], 'members': []}
                continue

            ctype = find_container(b['start'], type_blocks)
            if ctype:
                ns = find_container(ctype['start'], ns_blocks)
                nsn = ns['fqname'] if ns else 'global'
                hierarchy.setdefault(nsn, {'types': {}, 'globals': []})
                td = hierarchy[nsn]['types']
                fq = ctype['fqname']
                entry = td.setdefault(fq, {'kind': ctype['kind'], 'name': ctype['name'], 'fqname': fq, 'comment': None, 'members': []})
                if b.get('declaration', '').startswith(f"{ctype['kind']} {ctype['name']}"):
                    entry['comment'] = b['comment']
                else:
                    entry['members'].append({'declaration': b.get('declaration'), 'doc': b['comment']})
            else:
                ns = find_container(b['start'], ns_blocks)
                nsn = ns['fqname'] if ns else 'global'
                hierarchy.setdefault(nsn, {'types': {}, 'globals': []})
                decl = b.get('declaration') or ''
                match = re.search(r"\b(\w+)\s*\(", decl)
                name = match.group(1) if match else (decl.split(None,2)[1] if len(decl.split(None,2))>1 else None)
                hierarchy[nsn]['globals'].append({'declaration': decl, 'name': name, 'doc': b['comment']})

        for nsn, content in hierarchy.items():
            types = content['types']
            new_globals = []
            for g in content['globals']:
                mk = next((k for k,v in types.items() if v['name']==g['name']), None)
                if mk:
                    te = types[mk]
                    if g['doc']['description']:
                        te['comment'] = te.get('comment') or g['doc']
                        if te['comment'] and te['comment']!=g['doc']:
                            te['comment']['description'] = ' '.join(filter(None, [te['comment']['description'], g['doc']['description']]))
                else:
                    new_globals.append(g)
            content['globals'] = new_globals

        out_path = os.path.join(out_dir, os.path.splitext(fn)[0] + '.json')
        with open(out_path, 'w', encoding='utf-8') as f:
            json.dump({'file': fn, 'file_doc': file_doc, 'namespaces': hierarchy}, f, indent=2)
        print(f"Written {out_path}")
