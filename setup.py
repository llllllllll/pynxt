import glob
from setuptools import setup, Extension
import sys

long_description = ''

if 'upload' in sys.argv:
    with open('README.rst') as f:
        long_description = f.read()

setup(
    name='pynxt',
    version='0.1.0',
    description='Bluetooth control for the lego NXT.',
    author='Joe Jevnik',
    author_email='joejev@gmail.com',
    packages=[
        'pynxt',
    ],
    long_description=long_description,
    license='GPL-2',
    classifiers=[
        'Development Status :: 3 - Alpha',
        'License :: OSI Approved :: GNU General Public License v2 (GPLv2)',
        'Natural Language :: English',
        'Programming Language :: Python :: Implementation :: CPython',
        'Operating System :: POSIX',
    ],
    url='https://github.com/llllllllll/pynxt',
    ext_modules=[
        Extension(
            'pynxt._nxt',
            ['pynxt/_nxt.c'] + glob.glob('C_NXT/src/*.c'),
            include_dirs=['C_NXT/include'],
            libraries=['bluetooth'],
        ),
    ],
)
