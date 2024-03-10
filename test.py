#!/usr/bin/env python3

from dataclasses import dataclass, field
from typing import List, BinaryIO, Tuple, Optional

import shlex
import subprocess
import os
import sys

LOX_EXT = ".lox"

@dataclass
class TestCase:
    argv: List[str]
    stdin: bytes
    returncode: int
    stdout: bytes
    stderr: bytes


@dataclass
class RunStats:
    failed: int = 0
    ignored: int = 0
    failedFiles: List[str] = field(default_factory=list)


def echoCmd(cmd, **kwargs):
    print("[CMD] %s" % ' '.join(map(shlex.quote, cmd)))
    return subprocess.run(cmd, **kwargs)


class TestData:
    @staticmethod
    def readBlobField(f: BinaryIO, name: bytes) -> bytes:
        line = f.readline()
        field = b':b ' + name + b' '

        assert line.startswith(field)
        assert line.endswith(b'\n')

        size = int(line[len(field):-1])
        blob = f.read(size)

        assert f.read(1) == b'\n'
        return blob

    @staticmethod
    def readIntField(f: BinaryIO, name: bytes) -> int:
        line = f.readline()
        field = b':i ' + name + b' '

        assert line.startswith(field)
        assert line.endswith(b'\n')

        return int(line[len(field):-1])

    @staticmethod
    def writeBlobField(f: BinaryIO, name: bytes, blob: bytes) -> None:
        f.write(b':b %s %d\n' % (name, len(blob)))
        f.write(blob)
        f.write(b'\n')

    @staticmethod
    def writeIntField(f: BinaryIO, name: bytes, value: int) -> None:
        f.write(b':i %s %d\n' % (name, value))


class Tester:
    def __init__(self):
        self.default = TestCase(
            argv = [],
            stdin = bytes(),
            returncode = 0,
            stdout = bytes(),
            stderr = bytes()
        )

    @staticmethod
    def loadCase(path: str) -> Optional[TestCase]:
        try:
            with open(path, "rb") as f:
                argv = []
                argc = TestData.readIntField(f, b'argc')
                for index in range(argc):
                    argv.append(TestData.readBlobField(f, b'arg%d' % index).decode("utf-8"))
                stdin = TestData.readBlobField(f, b'stdin')
                returncode = TestData.readIntField(f, b'returncode')
                stdout = TestData.readBlobField(f, b'stdout')
                stderr = TestData.readBlobField(f, b'stderr')
                return TestCase(argv, stdin, returncode, stdout, stderr)
        except FileNotFoundError as e:
            print(f"{e}"[10:])
            return None

    @staticmethod
    def saveCase(
        path: str, argv: List[str],
        stdin: bytes, returncode: int,
        stdout: bytes, stderr: bytes
    ) -> None:
        with open(path, "wb") as f:
            TestData.writeIntField(f, b'argc', len(argv))
            for index, arg in enumerate(argv):
                TestData.writeBlobField(f, b'arg%d' % index, arg.encode("utf-8"))
            TestData.writeBlobField(f, b'stdin', stdin)
            TestData.writeIntField(f, b'returncode', returncode)
            TestData.writeBlobField(f, b'stdout', stdout)
            TestData.writeBlobField(f, b'stderr', stderr)

    @staticmethod
    def runTestOnFile(fpath: str, stats: RunStats = RunStats()) -> None:
        assert os.path.isfile(fpath)
        assert fpath.endswith(LOX_EXT) or fpath.endswith(LAX_EXT)

        print("[INFO] Testing %s..." % fpath)

        tpath = fpath[:-len(LOX_EXT)] + ".txt"
        case = Tester.loadCase(tpath)

        error = False

        if case is not None:
            cmd = echoCmd(
                ["./clox", fpath, *case.argv],
                input = case.stdin,
                capture_output = True
            )

            if cmd.returncode != case.returncode or       \
               cmd.stdout != case.stdout or         \
               cmd.stderr != case.stderr:

                print("[ERROR] Unexpected Output", file=sys.stderr)
                print("Expected:")
                print("    returncode: %s" % case.returncode)
                print("    stdout: \n%s" % case.stdout.decode("utf-8"))
                print("    stderr: \n%s" % case.stderr.decode("utf-8"))
                print()
                print("Actual:")
                print("    returncode: %s" % cmd.returncode)
                print("    stdout: \n%s" % cmd.stdout.decode("utf-8"))
                print("    stderr: \n%s" % cmd.stderr.decode("utf-8"))

                error = True
                stats.failed += 1
        else:
            print("[WARNING] Could not find any input/output data for %s. Ignoring testing, only checking for compilation.")
            cmd = echoCmd(["./clox", fpath])
            if cmd.returncode != 0:
                error = True
                stats.failed += 1
            stats.ignored += 1

        if error:
            stats.failedFiles.append(fpath)

    @staticmethod
    def runTestOnDir(dir: str) -> None:
        stats = RunStats()
        for entry in os.scandir(dir):
            if entry.is_file() and entry.path.endswith(LOX_EXT):
                Tester.runTestOnFile(entry.path, stats)
        print()
        print("Failed %d, Ignored %d" % (stats.failed, stats.ignored))
        if stats.failed != 0:
            print("Failed files:")
            print()
            for failedFile in stats.failedFiles:
                print(f"{failedFile}")
            exit(65)

    @staticmethod
    def updateExpectedInput(fpath: str, argv: List[str]) -> None:
        assert fpath.endswith(LOX_EXT)
        tpath = fpath[:-len(LOX_EXT)] + ".txt"
        case = Tester.loadCase(tpath) or Tester().default

        print("[INFO] Provide the stdin for the test case. Press Ctrl-D when finished...")

        stdin = sys.stdin.buffer.read()

        print("[INFO] Saving input to %s" % tpath)
        Tester.saveCase(tpath, argv, stdin, case.returncode, case.stdout, case.stderr)

    @staticmethod
    def updateExpectedOutput(fpath: str) -> None:
        tpath = fpath[:-len(LOX_EXT)] + ".txt"
        case = Tester.loadCase(tpath) or Tester().default

        output = echoCmd(
            ["./clox", fpath, *case.argv],
            input=case.stdin,
            capture_output=True
        )
        print("[INFO] Saving output to %s\n" % tpath)
        Tester.saveCase(
            tpath,
            case.argv,
            case.stdin,
            output.returncode,
            output.stdout,
            output.stderr
        )

    @staticmethod
    def updateExpectedDirOutput(dir: str) -> None:
        for entry in os.scandir(dir):
            if entry.is_file() and entry.path.endswith(LOX_EXT):
                Tester.updateExpectedOutput(entry.path)


def usage(exec: str):
    print(f"Usage: {exec} [COMMAND] [TARGET]?")
    print("  Run or update the test input/output. The default [COMMAND] is 'run'.")
    print()
    print("Commands:")
    print("    run [TARGET]?")
    print("        Runs the test case on the [TARGET]. The [TARGET] is either a .lox")
    print("        file or a directory with .lox files. The default [TARGET] is './tests/'.")
    print()
    print("    update [SUBCOMMAND]")
    print("        Update the input or output of the test cases.")
    print("        The default [SUBCOMMAND] is 'output'.")
    print()
    print("    Subcommands:")
    print("        input [TARGET]")
    print("            Update the input of the [TARGET]. The [TARGET] can only be a .lox file.")
    print()
    print("        output [TARGET]")
    print("            Update the output of the [TARGET]. The [TARGET] is either a '.lox'")
    print("            file or a directory of '.lox' files. The default [TARGET] is './tests/'.")
    print()
    print("    all")
    print("        Type check and test all cases.")
    print()
    print("    help")
    print("        Display this help message and exit with code 0.")


if __name__ == "__main__":
    exec, *argv = sys.argv
    cmd = "run"

    if len(argv) > 0:
        cmd, *argv = argv

    if cmd == "update":
        subcmd = "output"
        if len(argv) > 0:
            subcmd, *argv = argv

        if subcmd == "input":
            if len(argv) == 0:
                usage(exec)
                print("[ERROR] No file provided for '%s %s'." % (cmd, subcmd), file=sys.stderr)
                exit(64)
            fpath, *argv = argv
            Tester.updateExpectedInput(fpath, argv)
        elif subcmd == "output":
            target = "./tests"

            if len(argv) > 0:
                target, *argv = argv

            if os.path.isdir(target):
                Tester.updateExpectedDirOutput(target)
            elif os.path.isfile(target):
                Tester.updateExpectedOutput(target)
            else:
                assert False, "Unreachable"
        else:
            usage(exec)
            print("[ERROR] Unknown commands: '%s %s'." % (cmd, subcmd), file=sys.stderr)
            exit(64)
    elif cmd == "run":
        target = "./tests"

        if len(argv) > 0:
            target, *argv = argv

        if os.path.isdir(target):
            Tester.runTestOnDir(target)
        elif os.path.isfile(target):
            Tester.runTestOnFile(target)
        else:
            print("[ERROR] Cannot run tests on non-existant files!", file=sys.stderr)
            exit(65)
    elif cmd == "all":
        echoCmd(['mypy', './test.py'])
        Tester.runTestOnDir("./tests/")
    elif cmd == "help":
        usage(exec)
        exit(0)
    else:
        usage(exec)
        print("[ERROR] Unknown command: '%s'." % cmd, file=sys.stderr)
        exit(1)
