QT += core gui widgets

DEFINES += PLUGINSYSTEM_LIBRARY

CONFIG += c++17

TARGET = PluginSystem

TEMPLATE = lib

CONFIG += shared dll

SOURCES += \
    pluginmanager.cpp \
    pluginspec.cpp

HEADERS += \
    algorithm.h \
    iplugin.h \
    pluginmanager.h \
    pluginspec.h \
    pluginsystem_global.h

DESTDIR = $${PWD}/../dist/lib


###################################################################
# 复制指定文件至目标路径
out_path = $$clean_path($${PWD}/../dist/include)         # 编译输出文件夹下的资源文件夹名称
win32:out_path ~=  s,/,\\,g            # 路径转换
RES_EXTs = "*.h"                      # 文件(扩展)名

# 复制动态链接库.
win32 {
   # 获取路径下的所有RES_EXTs文件，第二个参数如果为true：递归子目录
   #res_files = $$files($$clean_path($$PWD/$$RES_PATH/$$RES_EXTs), false)
   for(res_file, HEADERS){
      !exists($$out_path){
         mkpath($$out_path)
      }
      header =$$PWD/$$res_file
      # windows下路径为，以下替换路径里的 / 为 \
      header ~= s,/,\\,g
      # copy命令，注意必须有escape_expend
      cmd = $$QMAKE_COPY $$shell_quote($$header) $$shell_quote($$out_path) $$escape_expand(\n\t)
      QMAKE_PRE_LINK += $$cmd
      # message($$cmd)
   }

   dll = $$clean_path($${DESTDIR}/$${TARGET}.dll)
   bin = $$clean_path($${DESTDIR}/../bin)

   dll ~= s,/,\\,g
   bin ~= s,/,\\,g

   cmd2 = $$QMAKE_COPY $$shell_quote($${dll}) $$shell_quote($$bin) $$escape_expand(\n\t)
   # message($$cmd2)
   QMAKE_PRE_LINK += $$cmd2
}
####################################################################
