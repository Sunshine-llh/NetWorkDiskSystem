######################################################################
# Automatically generated by qmake (3.1) Mon Nov 25 21:30:29 2024
######################################################################

TEMPLATE = app
TARGET = NetWorkDiskSystem
INCLUDEPATH += .

# You can make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# Please consult the documentation of the deprecated API in order to know
# how to port your code away from it.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += build-TcpClient-Desktop_Qt_5_12_12_MinGW_64_bit-Debug/ui_tcpclient.h \
           build-TcpServer-Desktop_Qt_5_12_12_MinGW_64_bit-Debug/ui_tcpserver.h \
           TcpClient/tcpclient.h \
           TcpServer/mytcpserver.h \
           TcpServer/tcpserver.h \
           /build-TcpClient-Desktop_Qt_5_12_12_MinGW_64_bit-Debug/ui_tcpclient.h \
           /build-TcpServer-Desktop_Qt_5_12_12_MinGW_64_bit-Debug/ui_tcpserver.h
FORMS += TcpClient/tcpclient.ui TcpServer/tcpserver.ui
SOURCES += TcpClient/main.cpp \
           TcpClient/tcpclient.cpp \
           TcpServer/main.cpp \
           TcpServer/mytcpserver.cpp \
           TcpServer/tcpserver.cpp \
           build-TcpClient-Desktop_Qt_5_12_12_MinGW_64_bit-Debug/debug/qrc_config.cpp \
           build-TcpServer-Desktop_Qt_5_12_12_MinGW_64_bit-Debug/debug/qrc_config.cpp
RESOURCES += TcpClient/config.qrc TcpServer/config.qrc
