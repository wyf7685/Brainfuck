import _brainfuck as _bf


def clean_code(code: str) -> str:
    return _bf.clean_code(code)


def parse_file(filename: str, clean: bool = True) -> str:
    code = _bf.parse_file(filename)
    if clean:
        code = clean_code(code)
    return code


def parse_string(code: str, clean: bool = True) -> str:
    code = _bf.parse_string(code)
    if clean:
        code = clean_code(code)
    return code


class InStream(object):
    _id: int
    _mode: str

    def __init__(self) -> None:
        self._id = _bf.stream_create()
        self._mode = "stdin"

    def __del__(self) -> None:
        _bf.stream_destroy(self._id)

    def __repr__(self) -> str:
        return f"<InStream id={self._id} mode={self._mode}>"

    def set_stdin(self) -> None:
        _bf.stream_set_stdin(self._id)
        self._mode = "stdin"

    def set_file(self, filename: str) -> None:
        _bf.stream_set_file(self._id, filename)
        self._mode = "file"

    def set_string(self, value: str) -> None:
        _bf.stream_set_string(self._id, value)
        self._mode = "string"


class Interpreter(object):
    _id: int
    stream: InStream

    def __init__(self, in_stream: InStream) -> None:
        self.stream = in_stream
        self._id = _bf.interp_create(in_stream._id)

    def __del__(self) -> None:
        _bf.interp_destroy(self._id)

    def __repr__(self) -> str:
        return f"<Interpreter id={self._id} stream={self.stream}>"

    def execute(self, code: str) -> None:
        _bf.interp_execute(self._id, code)

    def dump_memory(self) -> list[int]:
        return _bf.interp_dump_memory(self._id)

    def set_instream(self, in_stream: InStream) -> None:
        self.stream = in_stream
        _bf.interp_set_instream(self._id, in_stream._id)

    def set_memory(self, pos: int, value: int) -> None:
        _bf.interp_set_memory(self._id, pos, value)
