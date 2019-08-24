QT += core gui charts

user = $$system(whoami)
SHARED_FOLDER = /home/$$user/Qt.5.7/Examples/Qt-5.7/widgets/painting/shared
include($$SHARED_FOLDER/shared.pri)

QMAKE_LIBS = -lgsl -lgslcblas

INCLUDEPATH = ./dependencies/include $$SHARED_FOLDER

SOURCES += \
    ./dependencies/common_cpp/src/CLogistic.cpp \
    QCompressor.cpp \
    main.cpp \
    nurbs.cpp

INSTALLS += target

HEADERS += \
	./dependencies/common_cpp/inc/CLogistic.h \
	QCompressor.h \
	QCompressor.private.h \
	main.h \
	nurbs.h

RESOURCES += \
	resources.qrc
