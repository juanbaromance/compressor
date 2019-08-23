QT += core gui charts

user = $$system(whoami)
SHARED_FOLDER = /home/$$user/Qt.5.7/Examples/Qt-5.7/widgets/painting/shared
include($$SHARED_FOLDER/shared.pri)


INCLUDEPATH = ./dependencies/include \
 $$SHARED_FOLDER

SOURCES += \
    ./dependencies/common_cpp/src/CLogistic.cpp \
    QCompressor.cpp \
    main.cpp
INSTALLS += target

HEADERS += \
	./dependencies/common_cpp/inc/CLogistic.h \
	QCompressor.h \
	QCompressor.private.h \
	main.h

RESOURCES += \
	resources.qrc
