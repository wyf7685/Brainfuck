import sys
from pathlib import Path

DIR = Path(".")
FILE = DIR / "cleaned.bf"
CLEAN = True
CELL_SIZE = 65536
code = "".join(line.split("#")[0] for line in FILE.read_text("utf-8").splitlines())
memory = [0 for _ in range(1024)]
pointer = 0
loop_stack = []
loop_map = {}
idx = 0
if CLEAN:
    ValidCode = "><+-[],.%"
    _code = ""
    for i in code:
        if i in ValidCode:
            _code += i
    code = _code
    del _code, ValidCode, i
for i, c in enumerate(code):
    if c == "[":
        loop_stack.append(i)
    elif c == "]":
        loop_map[loop_stack.pop()] = i
if loop_stack:
    print("循环标记不匹配")
    sys.exit(-1)

while idx < len(code):
    match code[idx]:
        case ">":
            pointer += 1
        case "<":
            pointer -= 1
        case "+":
            memory[pointer] = (memory[pointer] + 1) % CELL_SIZE
        case "-":
            memory[pointer] = (memory[pointer] - 1) % CELL_SIZE
        case ".":
            sys.stdout.write(chr(memory[pointer]))
            sys.stdout.flush()
        case ",":
            memory[pointer] = ord(input()[0])
        case "[":
            if memory[pointer]:
                loop_stack.append(idx)
            else:
                idx = loop_map[idx]
        case "]":
            if memory[pointer]:
                idx = loop_stack.pop() - 1
            else:
                loop_stack.pop()
        case "%":  # 断点标记
            pass
    idx += 1

# while memory and memory[-1] == 0:
#     memory.pop()
# print()
# print(memory)
