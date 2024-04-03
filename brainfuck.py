import _brainfuck as _bf


def parse_file(filename: str) -> str:
    return _bf.parse_file(filename)


def clean_code(code: str) -> str:
    return _bf.clean_code(code)


class InStream(object):
    _id: int

    def __init__(self) -> None:
        self._id = _bf.stream_create()

    def __del__(self) -> None:
        _bf.stream_destroy(self._id)

    def set_stdin(self) -> None:
        _bf.stream_set_stdin(self._id)

    def set_file(self, filename: str) -> None:
        _bf.stream_set_file(self._id, filename)

    def set_string(self, value: str) -> None:
        _bf.stream_set_string(self._id, value)


class Interpreter(object):
    _id: int

    def __init__(self, in_stream: InStream) -> None:
        self._id = _bf.interp_create(in_stream._id)

    def __del__(self) -> None:
        _bf.interp_destroy(self._id)

    def execute(self, code: str) -> None:
        _bf.interp_execute(self._id, code)

    def dump_memory(self) -> list[int]:
        return _bf.interp_dump_memory(self._id)

    def set_instream(self, in_stream: InStream) -> None:
        _bf.interp_set_instream(self._id, in_stream._id)
