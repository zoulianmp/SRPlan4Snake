修改文件位置:D:\SRSnake\SRPlan4Snake.git\Modules\Loadable\PathPlanq\qSRPlanPathPlanModuleWidget.cxx
修改函数：qSRPlanPathPlanModuleWidget.cxx中的UpdateTraceMarkPosition()函数 【1447行】

目的：原来的坐标是只读 D:\NetworkShare\OpticTrace\RealtimePosition.txt文件确定位置，
      现在想读取该目录下传过来的0-100000000.raw文件，每个raw文件中只有一组数据（六个数）有效，后面的没有用，读完这个文件后就删除掉，下次QTimer触发后读取下一个raw文件。

