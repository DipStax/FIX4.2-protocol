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
    fd = open('./logs/build.log', mode='w')
    if not fd:
        log.fatal('Unable to open the log file: ./logs/build.log')
    log.debug('docker compose: info')
    cmd += ' --build --force-recreate --no-deps -d'
    log.debug(f'> Force rebuild image')
    log.debug(f'> detached')
    args: List[str] = cmd.split()
    subproc.run_subproc(args, stdout=fd)

if __name__ == '__main__':
    logconfig.config()
    sys.exit(main())