#!/usr/bin/python

VERSION = '0.0.1'
APPNAME = 'llrbpp'

top = '.'

subdirs = 'lib tool'

def options(opt):
  opt.tool_options('compiler_cxx')
  opt.tool_options('unittest_gtest')
  opt.recurse(subdirs)

def configure(conf):
  conf.check_tool('compiler_cxx')
  conf.check_tool('unittest_gtest')
#  conf.check_tool('prefixsum')
#  conf.check_cfg(path = '/usr/local/lib/pkgconfig/prefixsum',
#                 args = '--cppflags --libs --ldflags',
#                 package = '',
#                 uselib_store = 'PREFIXSUM',
#                 mandatory = True)
  conf.env.CXXFLAGS += ['-O2', '-Wall', '-W', '-g']
  conf.recurse(subdirs)

def build(bld):
  bld(source = 'llrbpp.pc.in',
      prefix = bld.env['PREFIX'],
      exec_prefix = '${prefix}',
      libdir = '${exec_prefix}/lib',
      includedir = '${prefix}/include',
      PACKAGE = APPNAME,
      VERSION = VERSION)
  bld.install_files('${PREFIX}/lib/pkgconfig', 'llrbpp.pc')
  bld.recurse(subdirs)
