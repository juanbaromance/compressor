QT += core gui charts
INCLUDEPATH = ./dependencies/include

SOURCES += \
    ./dependencies/common_cpp/src/CLogistic.cpp \
    QCompressor.cpp \
    main.cpp

INSTALLS += target

HEADERS += \
	./dependencies/common_cpp/inc/CLogistic.h \
	QCompressor.h \
	main.h

RESOURCES += \
	resources.qrc
