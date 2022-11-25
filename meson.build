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
        version : 'v0.9.0_alpha.1',
        license : 'GPL v3',
        meson_version: '>= 0.58.0',
        default_options : ['warning_level=2',
                           'buildtype=debugoptimized',
                           'b_lto=true',
                           'b_lto_threads=2',
                           'cpp_std=gnu++14'])

# Create configuration data
config_data = configuration_data()

# Save project information
meson_src_root = meson.current_source_dir()
meson_build_root = meson.current_build_dir()

project_name = meson.project_name()
project_license = meson.project_license()
project_version = meson.project_version()
project_author = 'Apoorv'
project_copyright_years = '2020-2021'
project_description = 'A simple, modern audio sample browser/manager for GNU/Linux.'
project_website = 'http://samplehive.gitlab.io/website'

build_type = get_option('buildtype')

host_sys = host_machine.system()

cpp_compiler_id = meson.get_compiler('cpp').get_id()
cpp_compiler_version = meson.get_compiler('cpp').version()

config_data.set_quoted('PROJECT_NAME', project_name)
config_data.set_quoted('PROJECT_LICENSE', project_license)
config_data.set_quoted('PROJECT_VERSION', project_version)
config_data.set_quoted('PROJECT_AUTHOR', project_author)
config_data.set_quoted('PROJECT_COPYRIGHT_YEARS', project_copyright_years)
config_data.set_quoted('PROJECT_DESCRIPTION', project_description)
config_data.set_quoted('PROJECT_WEBSITE', project_website)
config_data.set_quoted('BUILD_TYPE', build_type)
config_data.set_quoted('HOST_MACHINE', host_sys)
config_data.set_quoted('MESON_SRC_ROOT', meson_src_root)
config_data.set_quoted('MESON_BUILD_ROOT', meson_build_root)
config_data.set_quoted('CPP_COMPILER', cpp_compiler_id)
config_data.set_quoted('CPP_COMPILER_VERSION', cpp_compiler_version)

# Save important directories
prefix = get_option('prefix')
bindir = prefix / get_option('bindir')
libdir = prefix / get_option('libdir')
datadir = prefix / get_option('datadir')
samplehive_datadir = datadir / 'SampleHive'

config_data.set_quoted('PREFIX', prefix)
config_data.set_quoted('BINDIR', bindir)
config_data.set_quoted('LIBDIR', libdir)
config_data.set_quoted('DATADIR', datadir)
config_data.set_quoted('SAMPLEHIVE_DATADIR', samplehive_datadir)

if build_type == 'debug' or build_type == 'debugoptimized'
  config_data.set('SH_BUILD_DEBUG', 1)
elif build_type == 'release'
  config_data.set('SH_BUILD_RELEASE', 1)
endif

# Import CMake
cmake = import('cmake')

# Check which platform the build is for and set the appropriate GUI toolkit
wx_toolkit = ''

if host_sys == 'linux'
  wx_toolkit = 'gtk3'
elif host_sys == 'windows'
  wx_toolkit = 'msw'
elif host_sys == 'darwin'
  wx_toolkit = 'osx_cocoa'
endif

wx_opts = cmake.subproject_options()
wx_opts.add_cmake_defines({'CMAKE_POSITION_INDEPENDENT_CODE': 'ON',
                           'CMAKE_INSTALL_PREFIX': prefix,
                           'CMAKE_BUILD_TYPE': 'Release',
                           'wxBUILD_SHARED': 'ON',
                           'wxBUILD_MONOLITHIC': 'OFF',
                           'wxBUILD_BENCHMARKS': 'OFF',
                           'wxBUILD_PRECOMP': 'OFF',
                           'wxBUILD_TESTS': 'OFF',
                           'wxBUILD_SAMPLES': 'OFF',
                           'wxBUILD_DEMOS': 'OFF',
                           'wxBUILD_COMPATIBILITY': '3.0',
                           'wxBUILD_TOOLKIT': wx_toolkit,
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
                           'wxUSE_POSTSCRIPT': 'ON',
                           'wxUSE_RICHTEXT': 'OFF',
                           'wxUSE_WEBVIEW': 'OFF',
                           'wxUSE_LIBSDL': 'OFF',
                           'wxUSE_MEDIACTRL': 'ON'})

taglib_opts = cmake.subproject_options()
taglib_opts.add_cmake_defines({'CMAKE_POSITION_INDEPENDENT_CODE': 'ON',
                               'CMAKE_INSTALL_PREFIX': prefix,
                               'CMAKE_BUILD_TYPE': 'Release'})

yaml_opts = cmake.subproject_options()
yaml_opts.add_cmake_defines({'CMAKE_POSITION_INDEPENDENT_CODE': 'ON',
                             'CMAKE_INSTALL_PREFIX': prefix,
                             'CMAKE_BUILD_TYPE': 'Release',
                             'YAML_BUILD_SHARED_LIBS': 'ON',
                             'YAML_CPP_BUILD_TESTS': 'OFF',
                             'YAML_CPP_BUILD_CONTRIB': 'ON',
                             'YAML_CPP_BUILD_TOOLS': 'ON'})

snd_opts = cmake.subproject_options()

snd_shared = ''

if host_sys == 'windows'
  snd_shared = 'OFF'
else
  snd_shared = 'ON'
endif

snd_opts.add_cmake_defines({'CMAKE_POSITION_INDEPENDENT_CODE': 'ON',
                            'CMAKE_INSTALL_PREFIX': prefix,
                            'CMAKE_BUILD_TYPE': 'Release',
                            'BUILD_SHARED_LIBS': snd_shared,
                            'BUILD_PROGRAMS': 'OFF',
                            'BUILD_EXAMPLES': 'OFF',
                            'BUILD_TESTING': 'OFF',
                            'ENABLE_EXTERNAL_LIBS': 'ON',
                            'ENABLE_MPEG': 'ON'})

# Disable link time optimization if host is Windows
if host_sys == 'windows'
  wx_opts.set_override_option('b_lto', 'false')
  taglib_opts.set_override_option('b_lto', 'false')
  yaml_opts.set_override_option('b_lto', 'false')
  snd_opts.set_override_option('b_lto', 'false')
endif

# Source files to be compiled
src = [

  'src/App.cpp',

  'src/GUI/MainFrame.cpp',
  'src/GUI/TransportControls.cpp',
  'src/GUI/WaveformViewer.cpp',
  'src/GUI/Notebook.cpp',
  'src/GUI/DirectoryBrowser.cpp',
  'src/GUI/Hives.cpp',
  'src/GUI/Trash.cpp',
  'src/GUI/ListCtrl.cpp',
  'src/GUI/SearchBar.cpp',
  'src/GUI/InfoBar.cpp',
  'src/GUI/Library.cpp',

  'src/GUI/Dialogs/Settings.cpp',
  'src/GUI/Dialogs/TagEditor.cpp',

  'src/Database/Database.cpp',

  'src/Utility/Sample.cpp',
  'src/Utility/Serialize.cpp',
  'src/Utility/Tags.cpp',
  'src/Utility/Event.cpp',
  'src/Utility/Signal.cpp',
  'src/Utility/Log.cpp',
  'src/Utility/Utils.cpp',

]

include_dirs = include_directories('src')

# Static link C/C++ libs if platform is windows
link_args = []

if host_sys == 'windows'
  link_args += ['-static-libgcc', '-static-libstdc++']
endif

# Dependencies
wx = dependency('wxwidgets', version: '>=3.2.1', required: false)

wx_cxx_flags = []
wx_libs = []

if not wx.found()
  wx_subproject = cmake.subproject('wxwidgets', options: wx_opts)
  wx_media = wx_subproject.dependency('wxmedia')
  wx_core = wx_subproject.dependency('wxcore')
  wx_base = wx_subproject.dependency('wxbase')
  wx = [wx_media, wx_core, wx_base]

  if host_sys == 'windows'
    win = import('windows')
    wx_inc = wx_subproject.include_directories('wxbase')
    rc_file = win.compile_resources('src/GUI/Resources.rc', include_directories: wx_inc)

    src += rc_file
  endif
else
  wxconfig = find_program(['wx-config', 'wx-config-gtk3', 'wx-config-3.1'])
  wx_modules = ['media', 'std']

  foreach module : wx_modules
    wx_cxx_flags += run_command(wxconfig, '--cxxflags', module, capture: true, check: false).stdout().strip().split()
    wx_libs += run_command(wxconfig, '--libs', module, capture: true, check: false).stdout().strip().split()
  endforeach
endif

taglib = dependency('taglib', version: '>=1.12', required: false)

if not taglib.found()
  taglib_subproject = cmake.subproject('taglib', options: taglib_opts)
  taglib = taglib_subproject.dependency('tag')
else
  config_data.set('USE_SYSTEM_INCLUDE_PATH', 1)
endif

sqlite3 = dependency('sqlite3', version: '>=3.31.1', required: false)

if not sqlite3.found()
  sqlite3_subproject = subproject('sqlite3')
  libsqlite3 = sqlite3_subproject.get_variable('libsqlite3')
  sqlite3 = declare_dependency(link_with: libsqlite3)
endif

yaml = dependency('yaml-cpp', version: '>=0.6.2', required: false)

if not yaml.found()
  yaml_subproject = cmake.subproject('yaml-cpp', options: yaml_opts)
  yaml = yaml_subproject.dependency('yaml-cpp')
endif

snd_subproject = cmake.subproject('libsndfile', options: snd_opts)
snd = snd_subproject.dependency('sndfile')


spdlog_subproject = subproject('spdlog',
                               default_options: [
                                 'default_library=static',
                                 'compile_library=true',])

spdlog = spdlog_subproject.get_variable('spdlog_dep')

aubio = dependency('aubio', version: '>=0.4.9', required: false)

if not aubio.found()
  aubio_subproject = subproject('aubio')
  aubio = aubio_subproject.get_variable('aubio_dep')
endif

# Create SampleHiveConfig.hpp based on configuration
config = configure_file(output: 'SampleHiveConfig.hpp',
                        configuration: config_data,)

install_subdir('assets',
               install_dir: samplehive_datadir,
               exclude_directories: 'screenshots',
               strip_directory: true)

executable('SampleHive',
           sources: src,
           cpp_args: [wx_cxx_flags],
           link_args: [wx_libs, link_args],
           include_directories : include_dirs,
           dependencies: [wx, taglib, sqlite3, yaml, snd, spdlog, aubio],
           install: true,
           install_rpath: prefix / 'lib')

summary(
  {
    'Build type': build_type,
    'Optimization': get_option('optimization'),
    'Link time optimization': get_option('b_lto'),
    'Warning level': get_option('warning_level'),
    'Host system': host_sys,
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

summary(
  {
    'Project name': project_name,
    'Project license': project_license,
    'Project version': project_version,
    'Project author': project_author,
    'Project copyright years': project_copyright_years,
    'Project description': project_description,
    'Project website': project_website,
  },
  section: 'Project')
