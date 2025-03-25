import argparse

import yaramod

def text(yara_file):
    print(yara_file.text)

def rules(yara_file):
    for rule in yara_file.rules:
        print(rule.name)
        print(f'  Global: {rule.is_global}')
        print(f'  Private: {rule.is_private}')

def metas(yara_file):
    for rule in yara_file.rules:
        for meta in rule.metas:
            if meta.value.is_string:
                print('String meta: ', end='')
            elif meta.value.is_int:
                print('Int meta: ', end='')
            elif meta.value.is_bool:
                print('Bool meta: ', end='')
            print(f'{meta.key} = {meta.value.pure_text}')

def variables(yara_file):
    for rule in yara_file.rules:
        for variable in rule.variables:
            if variable.value is str:
                print('Plain string: ', end='')
            elif variable.value is int:
                print('Integer: ', end='')
            elif variable.value is float:
                print('Double: ', end='')
            elif variable.value is bool:
                print('Boolean: ', end='')
            print(f'{variable.key} = {variable.value.text}')

def strings(yara_file):
    for rule in yara_file.rules:
        for string in rule.strings:
            if string.is_plain:
                print('Plain string: ', end='')
            elif string.is_hex:
                print('Hex string: ', end='')
            elif string.is_regexp:
                print('Regexp: ', end='')
            print(f'{string.identifier} = {string.text}')
            print(f'  ascii: {string.is_ascii}')
            print(f'  wide: {string.is_wide}')
            print(f'  nocase: {string.is_nocase}')
            print(f'  fullword: {string.is_fullword}')
            print(f'  private: {string.is_private}')
            print(f'  xor: {string.is_xor}')
            print(f'  base64: {string.is_base64}')
            print(f'  base64wide: {string.is_base64_wide}')

def condition(yara_file):
    for rule in yara_file.rules:
        print(rule.condition.text)

## Condition visitors
class FunctionCallDumper(yaramod.ObservingVisitor):
    def visit_FunctionCallExpression(self, expr):
        print('Function call: {}'.format(expr.function.text))
        # Visit arguments because they can contain nested function calls
        for arg in expr.arguments:
            arg.accept(self)

def includes(yara_file):
    for rule in yara_file.rules:
        print(f'{rule.name}: {rule.location.file_path}:{rule.location.line_number}')

def imports(yara_file):
    for module in yara_file.imports:
        print(f'{module.name}')

def tokens(yara_file):
    for rule in yara_file.rules:
        for string in rule.strings:
            start = string.token_first.location.begin
            end = string.token_last.location.end
            print(f'[{start.line}, {start.column}] - [{end.line}, {end.column}]')

if __name__=='__main__':
    parser = argparse.ArgumentParser(description="to parse yara rules")
    parser.add_argument("file", type=str, help="path to yara rule file", default=None)
    parser.add_argument("--text", action="store_true", help="print all content")
    parser.add_argument("-r", "--rules", action="store_true")
    parser.add_argument("-m", "--metas", action="store_true")
    parser.add_argument("-v", "--variables", action="store_true")
    parser.add_argument("-s", "--strings", action="store_true")
    parser.add_argument("-c", "--condition", action="store_true")
    parser.add_argument("-in", "--includes", action="store_true")
    parser.add_argument("-im", "--imports", action="store_true")
    parser.add_argument("-t", "--tokens", action="store_true")
    args = parser.parse_args()
    
    if not args.file:
        print("path of file is needed, pls")
        exit()
    
    y = yaramod.Yaramod(yaramod.Features.AllCurrent)
    yara_file = y.parse_file(args.file)

    if args.text:
        text(yara_file)
    if args.rules:
        rules(yara_file)
    if args.metas:
        metas(yara_file)
    if args.variables:
        variables(yara_file)
    if args.strings:
        strings(yara_file)
    if args.condition:
        condition(yara_file)
    if args.includes:
        includes(yara_file)
    if args.imports:
        imports(yara_file)
    if args.tokens:
        tokens(yara_file)
        

