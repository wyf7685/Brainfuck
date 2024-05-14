def clean_code(code: str) -> str: ...
def parse_file(filename: str, clean: bool = True) -> str: ...
def parse_string(code: str, clean: bool = True) -> str: ...

class InStream(object):
    mode: int
    def __init__(self) -> None: ...
    def __repr__(self) -> str: ...
    def set_stdin(self) -> None: ...
    def set_file(self, filename: str) -> None: ...
    def set_string(self, value: str) -> None: ...

class Interpreter(object):
    stream: InStream
    def __init__(self, in_stream: InStream) -> None: ...
    def __repr__(self) -> str: ...
    def execute(self, code: str) -> None: ...
    def dump_memory(self) -> list[int]: ...
    def set_instream(self, in_stream: InStream) -> None: ...
    def set_memory(self, pos: int, value: int) -> None: ...
