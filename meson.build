project('SampleHive', 'cpp',
  version : '0.1',
  license : 'GPL v3',
  default_options : ['warning_level=1',
                     'cpp_std=c++11'])

src = [

  'src/App.cpp',
  'src/MainFrame.cpp',
  'src/SettingsDialog.cpp',
  'src/TagEditorDialog.cpp',
  'src/Database.cpp',
  'src/Serialize.cpp',
  'src/TreeItemDialog.cpp',
  'src/Tags.cpp',

  ]

wxconfig = find_program (['wx-config-gtk3', 'wx-config'])
wx_modules = ['media', 'std']
wx_cxx_flags = []
wx_libs = []

foreach module : wx_modules
  wx_cxx_flags += run_command (wxconfig, '--cxxflags', module).stdout ().strip ().split()
  wx_libs += run_command (wxconfig, '--libs', module).stdout ().strip ().split()
endforeach

wx = dependency('wxwidgets', version: '>=3.0.4')
wxsvg = dependency('libwxsvg')
taglib = dependency('taglib', version: '>=1.11')
sqlite3 = dependency('sqlite3')
yaml = dependency('yaml-cpp')

executable('SampleHive',
           sources : src,
           cpp_args: [wx_cxx_flags],
           link_args: [wx_libs],
           dependencies : [wx, wxsvg, taglib, sqlite3, yaml])
