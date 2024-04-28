import os
import sys
import logging as log
import logconfig
import subproc

ENV = os.environ["PWD"]


def cmake():
    log.info(f'CMake command: Start')
    args = "cmake .. -DServerBuild=TRUE".split()
    subproc.run_subproc(args)


def make():
    log.info(f'Make command: Start')
    args = "make".split()

    subproc.run_subproc(args)

def main() -> int:
    log.info('Start - Build Server')
    if not os.path.exists(f'{ENV}/build'):
        log.info('build forlder doesn\'t exist')
        os.mkdir(f'{ENV}/build')
        log.info(f'Created build folder: {ENV}/build')
    os.chdir('build')
    log.info(f'Moved to: {ENV}/build')

    cmake()
    make()

if __name__ == '__main__':
    logconfig.config()
    sys.exit(main())