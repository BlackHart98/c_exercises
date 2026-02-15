import ctypes as c
import typing as t
import sys
import platform

OS_NAME = platform.uname().system.lower()


def say_name(name : str):
    if "darwin" == OS_NAME:
        mylib = c.CDLL("./libmylib.dylib")
        mylib.say_hello_name(c.c_char_p(name.encode("utf-8")))
    else:
        print(f"hello world, {name}")
        

def main(argv : t.List[str]) -> int:
    say_name("The Weeknd")
    return 0


if __name__ == "__main__":
    main(sys.argv)
    pass