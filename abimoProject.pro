TEMPLATE = subdirs

# Names of the sub projects
SUBDIRS = \
    app \
    tests

# Relative paths to the sub projects
app.subdir = src/app
tests.subdir = src/tests

# What sub projects depend on other sub projects
tests.depends = app
