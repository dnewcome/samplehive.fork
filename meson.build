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

project('SampleHive', 'cpp',
        version : 'v0.8.4_alpha.1',
        license : 'GPL v3',
        default_options : ['warning_level=1',
                           'cpp_std=c++11'])

meson_src_root = meson.current_source_dir()

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

# Create samplehive-config.h based on configuration
config_h = configure_file(
  output: 'SampleHiveConfig.hpp',
  configuration: config_data,
)

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

]

wxconfig = find_program(['wx-config-gtk3', 'wx-config'])
wx_modules = ['media', 'std']
wx_cxx_flags = []
wx_libs = []

foreach module : wx_modules
  wx_cxx_flags += run_command(wxconfig, '--cxxflags', module).stdout().strip().split()
  wx_libs += run_command(wxconfig, '--libs', module).stdout().strip().split()
endforeach

# Dependencies
wx = dependency('wxwidgets', version: '>=3.0.4')
taglib = dependency('taglib', version: '>=1.11')
sqlite3 = dependency('sqlite3')
yaml = dependency('yaml-cpp')
snd = dependency('sndfile')

install_subdir(
  'assets',
  install_dir: samplehive_datadir,
  exclude_directories: 'screenshots'
)

executable('SampleHive',
           sources: src,
           cpp_args: [wx_cxx_flags],
           link_args: [wx_libs],
           dependencies: [wx, taglib, sqlite3, yaml, snd],
           install: true)
