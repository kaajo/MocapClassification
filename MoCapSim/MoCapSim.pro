TEMPLATE = subdirs

SUBDIRS += \
    ModelBase \
    TestApp \
    DataCreator \
    Visualization

TestApp.depends = Visualization
Visualization.depends = ModelBase
