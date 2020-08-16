INSTALLS       += target methods
target.path     = $$(HOME)/bin
methods.files   = methods/*.txt
methods.files  += methods/favourites
methods.path    = $$(HOME)/.towersim/methods/
CONFIG         += qt debug
TEMPLATE	= app
FORMS		= ui-towersim-mainwindow.ui ui-analysis-dialog.ui ui-select-method.ui
SOURCES		= towersim-application.cpp towersim-mainwindow.cpp \
                  methodcontroller.cpp \
                  ringingscheduler.cpp ringingserver.cpp \
                  sensorinputserver.cpp sensorparser.cpp \
                  statisticswindow.cpp select-method.cpp \
                  sensorlistwidget.cpp sensortablewidget.cpp sensortab.cpp ../common/alphanum.cpp \
                  strikingwindow.cpp outputparser.cpp analysis-dialog.cpp \
                  movingaveragegraphicsview.cpp labels.cpp movingaveragetab.cpp \
                  lineedit.cpp \
                  ../change/change.cpp ../change/touch.cpp ../change/method.cpp ../change/parsemethod.cpp
HEADERS         = towersim-mainwindow.h ringingscheduler.h methodcontroller.h \
                  ringingserver.h sensorinputserver.h sensorparser.h \
                  statisticswindow.h select-method.h \
                  sensorlistwidget.h sensortablewidget.h sensortab.h ../common/alphanum.h \
                  strikingwindow.h outputparser.h analysis-dialog.h \
                  movingaveragegraphicsview.h labels.h movingaveragetab.h lineedit.h
INCLUDEPATH    += ../
LIBS           += -lboost_date_time -lboost_system
QT             += network
