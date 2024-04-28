import subprocess
import logging as log
from typing import List
import sys

def run_subproc(args: List[str], stdout = subprocess.PIPE):
    log.info(f'Running command: {args}')
    popen = subprocess.Popen(args, stdout=stdout)
    result: int = popen.wait()
    log.info(f'Result of the command: {result}')
    if result != 0:
        log.fatal('CMakeFile command failed')
        sys.exit(1)
