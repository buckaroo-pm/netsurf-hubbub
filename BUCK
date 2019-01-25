load('//:buckaroo_macros.bzl', 'buckaroo_deps')
load('//:subdir_glob.bzl', 'subdir_glob')

genrule(
  name = 'entities-inc',
  out = 'entities.inc',
  srcs = glob([
    'build/*',
  ]),
  cmd = ' && '.join([
    'cp -r $SRCDIR/. $TMP',
    'cd $TMP',
    'mkdir -p src/tokeniser',
    'perl ./build/make-entities.pl',
    'touch $OUT',
  ]),
)

cxx_library(
  name = 'hubbub',
  header_namespace = '',
  exported_headers = subdir_glob([
    ('include', '**/*.h'),
  ]),
  headers = dict(
    subdir_glob([
      ('src', '**/*.h'),
    ]).items() + [
      ('entities.inc', ':entities-inc'),
    ]
  ),
  srcs = glob([
    'src/**/*.c',
  ]),
  deps = buckaroo_deps(),
  visibility = [
    'PUBLIC',
  ],
)
