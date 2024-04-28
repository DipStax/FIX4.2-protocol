import os
import sys
import logging as log
import logconfig
import subproc
from typing import List

ENV = os.environ["PWD"]

def main():
    log.info('Start - Build Server Container')
    if not os.path.exists(f'{ENV}/logs'):
        log.info('log forlder doesn\'t exist')
        os.mkdir(f'{ENV}/logs')
        log.info(f'Created log folder: {ENV}/logs')
    cmd = 'docker compose up'
    fd = open('./log/build.log', mode='+w')
    if not fd:
        log.fatal('Unable to open the log file: ./log/build.log')
    log.debug(f'Docker compose command:')
    cmd += ' --build --force-recreate --no-deps'
    log.debug(f'Force rebuild image')
    args: List[str] = cmd.split()
    subproc.run_subproc(args)


if __name__ == '__main__':
    logconfig.config()
    sys.exit(main())