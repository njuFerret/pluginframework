TEMPLATE = subdirs
SUBDIRS += \
    pluginsystem \
    coreplugin \
    helloworld \
    test

coreplugin.depends = pluginsystem
helloworld.depends = pluginsystem
test.despends = helloworld
