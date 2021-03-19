project('SampleHive', 'cpp',
  version : '0.1',
  license : 'GPL v3',
  default_options : ['warning_level=1',
                     'cpp_std=c++11'])

src = [

  'src/App.cpp',
  'src/MainFrame.cpp',
  'src/Browser.cpp',
  'src/SettingsDialog.cpp',
  'src/TagEditorDialog.cpp',
  'src/Database.cpp',
  'src/Serialize.cpp',
  'src/TreeItemDialog.cpp',
  'src/Tags.cpp',

  ]

wx = dependency('wxwidgets', modules : ['media', 'std'])
wxsvg = dependency('libwxsvg')
taglib = dependency('taglib')
sqlite3 = dependency('sqlite3')
yaml = dependency('yaml-cpp')

wx_inc = include_directories('/usr/include/wx-3.0')

executable('SampleHive',
           sources : src,
           dependencies : [wx, wxsvg, taglib, sqlite3, yaml],
           include_directories : wx_inc)
