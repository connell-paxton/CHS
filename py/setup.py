from distutils.core import setup, Extension

spammodule = Extension('spam', sources = ['main.cpp'])

setup (
	name = 'spam',
	version = '1.0',
	description = 'this is spam',
	ext_modules = [spammodule]
)
