# Sample Hive
# Copyright (C) 2021  Apoorv Singh
# A simple, modern audio sample browser/manager for GNU/Linux.
#
# This file is a part of SampleHive
#
# SampleHive is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# SampleHive is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

project('SampleHive',
        'cpp',
        version : 'v0.8.4_alpha.1',
        license : 'GPL v3',
        default_options : ['warning_level=1',
                           'cpp_std=c++11'])

meson_src_root = meson.current_source_dir()
meson_build_root = meson.current_build_dir()

# Save important directories
prefix = get_option('prefix')
bindir = prefix / get_option('bindir')
libdir = prefix / get_option('libdir')
datadir = prefix / get_option('datadir')
samplehive_datadir = datadir / 'SampleHive'

# Create configuration data
config_data = configuration_data()
config_data.set_quoted('PREFIX', prefix)
config_data.set_quoted('BINDIR', bindir)
config_data.set_quoted('LIBDIR', libdir)
config_data.set_quoted('DATADIR', datadir)
config_data.set_quoted('SAMPLEHIVE_DATADIR', samplehive_datadir)

# Import CMake
cmake = import('cmake')

wx_opts = cmake.subproject_options()
wx_opts.add_cmake_defines({'CMAKE_POSITION_INDEPENDENT_CODE': 'ON',
                           'CMAKE_INSTALL_PREFIX': prefix,
                           'CMAKE_BUILD_TYPE': 'Release',
                           'CMAKE_CXX_COMPILER': 'g++',
                           'wxBUILD_SHARED': 'ON',
                           'wxBUILD_TESTS': 'OFF',
                           'wxBUILD_SAMPLES': 'OFF',
                           'wxBUILD_DEMOS': 'OFF',
                           'wxBUILD_COMPATIBILITY': '3.0',
                           'wxUSE_UNICODE': 'ON',
                           'wxUSE_AUI': 'OFF',
                           'wxUSE_XML': 'OFF',
                           'wxUSE_XRC': 'ON',
                           'wxUSE_HTML': 'ON',
                           'wxUSE_QA': 'ON',
                           'wxUSE_PROPGRID': 'OFF',
                           'wxUSE_RIBBON': 'OFF',
                           'wxUSE_MDI': 'OFF',
                           'wxUSE_MDI_ARCHITECTURE': 'OFF',
                           'wxUSE_RICHTEXT': 'OFF',
                           'wxUSE_WEBVIEW': 'OFF',
                           'wxUSE_LIBSDL': 'OFF',
                           'wxUSE_MEDIACTRL': 'ON'})

taglib_opts = cmake.subproject_options()
taglib_opts.add_cmake_defines({'CMAKE_POSITION_INDEPENDENT_CODE': 'ON',
                               'CMAKE_INSTALL_PREFIX': prefix,
                               'CMAKE_BUILD_TYPE': 'Release',
                               'CMAKE_CXX_COMPILER': 'g++'})

# Source files to be compiled
src = [

  'src/App.cpp',
  'src/MainFrame.cpp',
  'src/SettingsDialog.cpp',
  'src/TagEditorDialog.cpp',
  'src/Database.cpp',
  'src/Sample.cpp',
  'src/Serialize.cpp',
  'src/Tags.cpp',
  'src/WaveformViewer.cpp',
  'src/SH_Event.cpp',

]

# Dependencies
wx = dependency('wxwidgets', version: '>=3.1.5', required: false)

wx_found = false

if not wx.found()
  wx_found = false

  wx_subproject = cmake.subproject('wxwidgets', options: wx_opts)
  wx_base = wx_subproject.dependency('wxbase')
  wx_core = wx_subproject.dependency('wxcore')
  wx_media = wx_subproject.dependency('wxmedia')
  wx = [wx_core, wx_base, wx_media]
else
  wx_found = true

  wxconfig = find_program(['wx-config-gtk3', 'wx-config'])
  wx_modules = ['media', 'std']
  wx_cxx_flags = []
  wx_libs = []

  foreach module : wx_modules
    wx_cxx_flags += run_command(wxconfig, '--cxxflags', module).stdout().strip().split()
    wx_libs += run_command(wxconfig, '--libs', module).stdout().strip().split()
  endforeach
endif

taglib = dependency('taglib', version: '>=1.12', required: false)

if not taglib.found()
  taglib_subproject = cmake.subproject('taglib', options: taglib_opts)
  taglib = taglib_subproject.dependency('tag')
else
  config_data.set('USE_SYSTEM_INCLUDE_PATH', 1)
endif

sqlite3 = dependency('sqlite3', required: true)
yaml = dependency('yaml-cpp', required: true)
snd = dependency('sndfile', required: true)

# Create samplehive-config.h based on configuration
config_h = configure_file(output: 'SampleHiveConfig.hpp',
                          configuration: config_data,)

install_subdir('assets',
               install_dir: samplehive_datadir,
               exclude_directories: 'screenshots')

if wx_found
  executable('SampleHive',
             sources: src,
             cpp_args: [wx_cxx_flags],
             link_args: [wx_libs],
             dependencies: [wx, taglib, sqlite3, yaml, snd],
             install: true,
             install_rpath: prefix / 'lib')
else
  executable('SampleHive',
             sources: src,
             dependencies: [wx, taglib, sqlite3, yaml, snd],
             install: true,
             install_rpath: prefix / 'lib')
endif

summary(
  {
    'Debug': get_option('debug'),
    'Optimization': get_option('optimization'),
  },
  section: 'General')

summary(
  {
    'prefix': prefix,
    'bindir': bindir,
    'libdir': libdir,
    'datadir': datadir,
    'samplehive_datadir': samplehive_datadir,
  },
  section: 'Directories')
