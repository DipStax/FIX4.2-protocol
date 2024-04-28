import subprocess
import logging as log
from typing import List
import sys

def run_subproc(args: List[str]):
    log.info(f'Running command: {args}')
    popen = subprocess.Popen(args, stdout=subprocess.PIPE)
    result: int = popen.wait()
    log.info(f'Result of the command: {result}')
    if result != 0:
        log.fatal('CMakeFile command failed')
        sys.exit(1)
