#!/usr/bin/env python
import argparse
import glob
import os
import shutil
import subprocess
import sys

from lib.util import get_electron_branding, rm_rf, scoped_cwd

PROJECT_NAME = get_electron_branding()['project_name']
PRODUCT_NAME = get_electron_branding()['product_name']
SOURCE_ROOT = os.path.abspath(os.path.dirname(os.path.dirname(__file__)))
SNAPSHOT_SOURCE = os.path.join(SOURCE_ROOT, 'spec', 'fixtures', 'testsnap.js')

def main():
  args = parse_args()

  source_root = os.path.abspath(args.source_root)
  initial_app_path = os.path.join(source_root, args.build_dir)
  app_path = create_app_copy(initial_app_path)

  returncode = 0
  try:
    with scoped_cwd(app_path):
      mkargs = [ get_binary_path('mksnapshot', app_path), \
                   SNAPSHOT_SOURCE, '--startup_blob', 'snapshot_blob.bin', \
                   '--turbo_instruction_scheduling' ]
      subprocess.check_call(mkargs)
      print 'ok mksnapshot successfully created snapshot_blob.bin.'
      context_snapshot = 'v8_context_snapshot.bin'
      context_snapshot_path = os.path.join(app_path, context_snapshot)
      gen_binary = get_binary_path('v8_context_snapshot_generator', \
                                   app_path)
      genargs = [ gen_binary, \
                 '--output_file={0}'.format(context_snapshot_path) ]
      subprocess.check_call(genargs)
      print 'ok v8_context_snapshot_generator successfully created ' \
            + context_snapshot

      test_path = os.path.join(SOURCE_ROOT, 'spec', 'fixtures', \
                               'snapshot-items-available.js')

      if sys.platform == 'darwin':
        bin_files = glob.glob(os.path.join(app_path, '*.bin'))
        app_dir = os.path.join(app_path, '{0}.app'.format(PRODUCT_NAME))
        electron = os.path.join(app_dir, 'Contents', 'MacOS', PRODUCT_NAME)
        bin_out_path = os.path.join(app_dir, 'Contents', 'Frameworks',
                  '{0} Framework.framework'.format(PROJECT_NAME),
                  'Resources')
        for bin_file in bin_files:
          shutil.copy2(bin_file, bin_out_path)
      elif sys.platform == 'win32':
        electron = os.path.join(app_path, '{0}.exe'.format(PROJECT_NAME))
      else:
        electron = os.path.join(app_path, PROJECT_NAME)

      subprocess.check_call([electron, test_path])
      print 'ok successfully used custom snapshot.'
  except subprocess.CalledProcessError as e:
    print 'not ok an error was encountered while testing mksnapshot.'
    print e
    returncode = e.returncode
  except KeyboardInterrupt:
    print 'Other error'
    returncode = 0

  return returncode


# Create copy of app to install custom snapshot
def create_app_copy(initial_app_path):
  print 'Creating copy of app for testing'
  app_path = os.path.join(os.path.dirname(initial_app_path),
                          os.path.basename(initial_app_path)
                          + '-mksnapshot-test')

  rm_rf(app_path)
  shutil.copytree(initial_app_path, app_path, symlinks=True)
  return app_path

def get_binary_path(binary_name, root_path):
  if sys.platform == 'win32':
    binary_path = os.path.join(root_path, '{0}.exe'.format(binary_name))
  else:
    binary_path = os.path.join(root_path, binary_name)
  return binary_path

def parse_args():
  parser = argparse.ArgumentParser(description='Test mksnapshot')
  parser.add_argument('-b', '--build-dir',
                      help='Path to an Electron build folder. \
                          Relative to the --source-root.',
                      default=None,
                      required=True)
  parser.add_argument('--source-root',
                      default=SOURCE_ROOT,
                      required=False)
  return parser.parse_args()

if __name__ == '__main__':
  sys.exit(main())
