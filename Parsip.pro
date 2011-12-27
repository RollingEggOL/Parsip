######################################################################
# Automatically generated by qmake (2.01a) Fri Nov 11 09:58:56 2011
######################################################################

TEMPLATE = app
TARGET = ParsipHaptics
QT += opengl network
DEPENDPATH += . \
              Parsip100/ParsipHaptics/include \
              Parsip100/DSystem/include \              
              Parsip100/PS_BlobTree/include \
              Parsip100/PS_FrameWork/include
INCLUDEPATH += . \
               Parsip100 \
               Parsip100/ParsipHaptics/include \
               Parsip100/DSystem/include \
               Parsip100/PS_FrameWork/include \
               Parsip100/PS_BlobTree/include


# Input
HEADERS += \
           Parsip100/ParsipHaptics/include/AppSettings.h \
           Parsip100/ParsipHaptics/include/_GlobalFunctions.h \
           Parsip100/ParsipHaptics/include/_GlobalSettings.h \
           Parsip100/ParsipHaptics/include/_LookupTables.h \
           Parsip100/ParsipHaptics/include/_PolygonizerStructs.h \
           Parsip100/ParsipHaptics/include/CBlobTreeAnimation.h \
           Parsip100/ParsipHaptics/include/CBlobTreeNetwork.h \
           Parsip100/ParsipHaptics/include/CCubeTable.h \
           Parsip100/ParsipHaptics/include/CEaseInEaseOut.h \
           Parsip100/ParsipHaptics/include/CEdgeTable.h \
           Parsip100/ParsipHaptics/include/CLayerManager.h \
           Parsip100/ParsipHaptics/include/CMeshQuality.h \
           Parsip100/ParsipHaptics/include/CompactBlobTree.h \
           Parsip100/ParsipHaptics/include/ConversionToActions.h \
           Parsip100/ParsipHaptics/include/CParallelAdaptiveSubdivision.h \
           Parsip100/ParsipHaptics/include/CPolyContinuation.h \
           Parsip100/ParsipHaptics/include/CPolyHashGridEdgeTables.h \
           Parsip100/ParsipHaptics/include/CPolyInterface.h \
           Parsip100/ParsipHaptics/include/CPolyParsipOptimized.h \
           Parsip100/ParsipHaptics/include/CPolyParsipServer.h \
           Parsip100/ParsipHaptics/include/CUIWidgets.h \
           Parsip100/ParsipHaptics/include/DlgFieldFunctionEditor.h \
           Parsip100/ParsipHaptics/include/FastQuadricPointSet.h \
           Parsip100/ParsipHaptics/include/GalinMedusaGenerator.h \
           Parsip100/ParsipHaptics/include/glwidget.h \
           Parsip100/ParsipHaptics/include/mainwindow.h \
           Parsip100/ParsipHaptics/include/PS_PerfTest.h \
           Parsip100/ParsipHaptics/include/PS_SketchConfig.h \
           Parsip100/PS_BlobTree/include/_constSettings.h \
           Parsip100/PS_BlobTree/include/BlobTreeBuilder.h \
           Parsip100/PS_BlobTree/include/BlobTreeLibraryAll.h \
           Parsip100/PS_BlobTree/include/CAdaptiveUniformGrid3d.h \
           Parsip100/PS_BlobTree/include/CBlend.h \
           Parsip100/PS_BlobTree/include/CBlobTree.h \
           Parsip100/PS_BlobTree/include/CCache.h \
           Parsip100/PS_BlobTree/include/CDifference.h \
           Parsip100/PS_BlobTree/include/CFieldFunction.h \
           Parsip100/PS_BlobTree/include/CIntersection.h \
           Parsip100/PS_BlobTree/include/CPcm.h \
           Parsip100/PS_BlobTree/include/CPrimHalfPlane.h \
           Parsip100/PS_BlobTree/include/CQuadricPoint.h \
           Parsip100/PS_BlobTree/include/CRicciBlend.h \
           Parsip100/PS_BlobTree/include/CRootFinder.h \
           Parsip100/PS_BlobTree/include/CSkeleton.h \
           Parsip100/PS_BlobTree/include/CSkeletonCube.h \
           Parsip100/PS_BlobTree/include/CSkeletonCylinder.h \
           Parsip100/PS_BlobTree/include/CSkeletonDisc.h \
           Parsip100/PS_BlobTree/include/CSkeletonLine.h \
           Parsip100/PS_BlobTree/include/CSkeletonPoint.h \
           Parsip100/PS_BlobTree/include/CSkeletonPolygon.h \
           Parsip100/PS_BlobTree/include/CSkeletonPrimitive.h \
           Parsip100/PS_BlobTree/include/CSkeletonRing.h \
           Parsip100/PS_BlobTree/include/CSkeletonTriangle.h \
           Parsip100/PS_BlobTree/include/CSmoothDifference.h \
           Parsip100/PS_BlobTree/include/CUnion.h \
           Parsip100/PS_BlobTree/include/CVolume.h \
           Parsip100/PS_BlobTree/include/CVolumeBox.h \
           Parsip100/PS_BlobTree/include/CVolumeSphere.h \
           Parsip100/PS_BlobTree/include/CWarpBend.h \
           Parsip100/PS_BlobTree/include/CWarpShear.h \
           Parsip100/PS_BlobTree/include/CWarpTaper.h \
           Parsip100/PS_BlobTree/include/CWarpTwist.h \
           Parsip100/PS_FrameWork/include/_dataTypes.h \
           Parsip100/PS_FrameWork/include/_parsDebug.h \
           Parsip100/PS_FrameWork/include/_parsProfile.h \
           Parsip100/PS_FrameWork/include/common.h \
           Parsip100/PS_FrameWork/include/DX_ComputeShader11.h \
           Parsip100/PS_FrameWork/include/DX_Device11.h \
           Parsip100/PS_FrameWork/include/DX_ShaderManager.h \
           Parsip100/PS_FrameWork/include/mathHelper.h \
           Parsip100/PS_FrameWork/include/PS_AffineTransformation.h \
           Parsip100/PS_FrameWork/include/PS_AppConfig.h \
           Parsip100/PS_FrameWork/include/PS_ArcBallCamera.h \
           Parsip100/PS_FrameWork/include/PS_BoundingBox.h \
           Parsip100/PS_FrameWork/include/PS_DateTime.h \
           Parsip100/PS_FrameWork/include/PS_ErrorManager.h \
           Parsip100/PS_FrameWork/include/PS_FileDirectory.h \
           Parsip100/PS_FrameWork/include/PS_GenCylinder.h \
           Parsip100/PS_FrameWork/include/PS_GeometryFuncs.h \
           Parsip100/PS_FrameWork/include/PS_HWUtils.h \
           Parsip100/PS_FrameWork/include/PS_Interval.h \
           Parsip100/PS_FrameWork/include/PS_Lock.h \
           Parsip100/PS_FrameWork/include/PS_Material.h \
           Parsip100/PS_FrameWork/include/PS_Matrix.h \
           Parsip100/PS_FrameWork/include/PS_Mesh.h \
           Parsip100/PS_FrameWork/include/PS_MeshBase.h \
           Parsip100/PS_FrameWork/include/PS_MeshVV.h \
           Parsip100/PS_FrameWork/include/PS_MovingAverage.h \
           Parsip100/PS_FrameWork/include/PS_Octree.h \
           Parsip100/PS_FrameWork/include/PS_ParticleSystem.h \
           Parsip100/PS_FrameWork/include/PS_PerfLogger.h \
           Parsip100/PS_FrameWork/include/PS_PixelMap.h \
           Parsip100/PS_FrameWork/include/PS_Quaternion.h \
           Parsip100/PS_FrameWork/include/PS_Ray.h \
           Parsip100/PS_FrameWork/include/PS_ResourceManager.h \
           Parsip100/PS_FrameWork/include/PS_ShaderGLSL.h \
           Parsip100/PS_FrameWork/include/PS_ShaderManager.h \
           Parsip100/PS_FrameWork/include/PS_Singleton.h \
           Parsip100/PS_FrameWork/include/PS_SplineCatmullRom.h \
           Parsip100/PS_FrameWork/include/PS_String.h \
           Parsip100/PS_FrameWork/include/PS_StringBase.h \
           Parsip100/PS_FrameWork/include/PS_Vector.h \
           Parsip100/PS_FrameWork/include/TaskManager.h \
    Parsip100/PS_BlobTree/include/CGradientBlend.h \
    Parsip100/PS_FrameWork/include/PS_SketchConfig.h \
    Parsip100/PS_FrameWork/include/PS_Property.h \
    Parsip100/PS_BlobTree/include/CNullPrimitive.h \
    Parsip100/ParsipHaptics/include/SampleShapes.h \
    Parsip100/PS_BlobTree/include/PS_SketchActions.h \
    Parsip100/PS_BlobTree/include/CTurboChargedPrimitive.h \
    Parsip100/ParsipHaptics/include/PS_HighPerformanceRender.h

FORMS += Parsip100/ParsipHaptics/DlgFieldFunction.ui \
         Parsip100/ParsipHaptics/DlgMtrlEditor.ui \
         Parsip100/ParsipHaptics/mainwindow.ui

SOURCES += Parsip100/ParsipHaptics/include/AppSettings.cpp \
           Parsip100/ParsipHaptics/include/_GlobalFunctions.cpp \
           Parsip100/ParsipHaptics/include/CBlobTreeAnimation.cpp \
           Parsip100/ParsipHaptics/include/CBlobTreeNetwork.cpp \
           Parsip100/ParsipHaptics/include/CCubeTable.cpp \
           Parsip100/ParsipHaptics/include/CLayerManager.cpp \
           Parsip100/ParsipHaptics/include/CMeshQuality.cpp \
           Parsip100/ParsipHaptics/include/CompactBlobTree.cpp \
           Parsip100/ParsipHaptics/include/ConversionToActions.cpp \
           Parsip100/ParsipHaptics/include/CParallelAdaptiveSubdivision.cpp \
           Parsip100/ParsipHaptics/include/CPolyContinuation.cpp \
           Parsip100/ParsipHaptics/include/CPolyParsipOptimized.cpp \
           Parsip100/ParsipHaptics/include/CPolyParsipServer.cpp \
           Parsip100/ParsipHaptics/include/DlgFieldFunctionEditor.cpp \
           Parsip100/ParsipHaptics/include/FastQuadricPointSet.cpp \
           Parsip100/ParsipHaptics/include/GalinMedusaGenerator.cpp \
           Parsip100/ParsipHaptics/include/glwidget.cpp \
           Parsip100/ParsipHaptics/include/main.cpp \
           Parsip100/ParsipHaptics/include/mainwindow.cpp \
           Parsip100/ParsipHaptics/include/PS_PerfTest.cpp \
           Parsip100/PS_BlobTree/include/BlobTreeBuilder.cpp \
           Parsip100/PS_BlobTree/include/CAdaptiveUniformGrid3d.cpp \
           Parsip100/PS_BlobTree/include/CCache.cpp \
           Parsip100/PS_BlobTree/include/CRootFinder.cpp \
           Parsip100/PS_BlobTree/include/CSkeletonPrimitive.cpp \
           Parsip100/PS_BlobTree/include/CSkeletonTriangle.cpp \
           Parsip100/PS_BlobTree/include/CVolume.cpp \
           Parsip100/PS_BlobTree/include/CVolumeBox.cpp \
           Parsip100/PS_BlobTree/include/CVolumeSphere.cpp \
           Parsip100/PS_FrameWork/include/DX_ComputeShader11.cpp \
           Parsip100/PS_FrameWork/include/DX_Device11.cpp \
           Parsip100/PS_FrameWork/include/DX_ShaderManager.cpp \
           Parsip100/PS_FrameWork/include/PS_AppConfig.cpp \
           Parsip100/PS_FrameWork/include/PS_ArcBallCamera.cpp \
           Parsip100/PS_FrameWork/include/PS_DateTime.cpp \
           Parsip100/PS_FrameWork/include/PS_ErrorManager.cpp \
           Parsip100/PS_FrameWork/include/PS_FileDirectory.cpp \
           Parsip100/PS_FrameWork/include/PS_GenCylinder.cpp \
           Parsip100/PS_FrameWork/include/PS_GeometryFuncs.cpp \
           Parsip100/PS_FrameWork/include/PS_HWUtils.c \
           Parsip100/PS_FrameWork/include/PS_Matrix.cpp \
           Parsip100/PS_FrameWork/include/PS_Mesh.cpp \
           Parsip100/PS_FrameWork/include/PS_MeshOGL.cpp \
           Parsip100/PS_FrameWork/include/PS_MeshVV.cpp \
           Parsip100/PS_FrameWork/include/PS_Octree.cpp \
           Parsip100/PS_FrameWork/include/PS_PerfLogger.cpp \
           Parsip100/PS_FrameWork/include/PS_PixelMap.cpp \
           Parsip100/PS_FrameWork/include/PS_Quaternion.cpp \
           Parsip100/PS_FrameWork/include/PS_ShaderGLSL.cpp \
           Parsip100/PS_FrameWork/include/PS_ShaderManager.cpp \
           Parsip100/PS_FrameWork/include/PS_SplineCatmullRom.cpp \
           Parsip100/PS_FrameWork/include/PS_String.cpp \
           Parsip100/PS_FrameWork/include/TaskManager.cpp \
    Parsip100/ParsipHaptics/include/CUIWidgets.cpp \
    Parsip100/PS_FrameWork/include/PS_SketchConfig.cpp \
    Parsip100/PS_BlobTree/include/CBlobTree.cpp \
    Parsip100/PS_BlobTree/include/CBlend.cpp \
    Parsip100/PS_BlobTree/include/CUnion.cpp \
    Parsip100/PS_BlobTree/include/CDifference.cpp \
    Parsip100/PS_BlobTree/include/CIntersection.cpp \
    Parsip100/PS_BlobTree/include/CRicciBlend.cpp \
    Parsip100/PS_BlobTree/include/CGradientBlend.cpp \
    Parsip100/PS_BlobTree/include/CSkeletonCube.cpp \
    Parsip100/PS_BlobTree/include/CSkeletonCylinder.cpp \
    Parsip100/PS_BlobTree/include/CSkeletonLine.cpp \
    Parsip100/PS_BlobTree/include/CSkeletonRing.cpp \
    Parsip100/PS_BlobTree/include/CSkeletonDisc.cpp \
    Parsip100/PS_BlobTree/include/CSkeletonPoint.cpp \
    Parsip100/PS_FrameWork/include/PS_Property.cpp \
    Parsip100/PS_BlobTree/include/CNullPrimitive.cpp \
    Parsip100/ParsipHaptics/include/PS_HighPerformanceRendering.cpp
RESOURCES += Parsip100/ParsipHaptics/ParsipIcons.qrc

# Setting Output Directory
DESTDIR = $$PWD/Parsip100/Distrib

CONFIG(debug, debug|release) {
     TARGET = $$join(TARGET,,,_Debug)
 }

CONFIG(release, debug|release) {
     TARGET = $$join(TARGET,,,_Release)
 }


##win32:CONFIG(release, debug|release):
##else:win32:CONFIG(debug, debug|release):
#CONFIG(debug, debug|release) {
#     unix: TARGET = $$join(TARGET,,,_Debug)
#     else: TARGET = $$join(TARGET,,,d)
# }

#CONFIG(release, debug|release) {
#     unix: TARGET = $$join(TARGET,,,_Release)
#     else: TARGET = $$join(TARGET,,,d)
# }

win32:CONFIG(release, debug|release): LIBS += D:\Projects\Dependencies\TBB\lib\ia32\vc10\tbb.lib \
                                              D:\Projects\Dependencies\LOKI\lib\loki.lib
else:win32:CONFIG(debug, debug|release): LIBS += D:\Projects\Dependencies\TBB\lib\ia32\vc10\tbb_debug.lib \
                                                 D:\Projects\Dependencies\LOKI\lib\loki_D.lib

win32: LIBS +=  D:\Projects\Dependencies\OPENCL\common\lib\Win32\OpenCL.lib \
                D:\Projects\Dependencies\GLEW\lib\glew32.lib \


else:unix:!symbian: LIBS += -L$$PWD/../Dependencies/TBB/lib/intel64/cc4.1.0_libc2.4_kernel2.6.16.21/ -ltbb \
                            -L$$PWD/../Dependencies/LOKI/lib/ -lloki \
                            -lGLEW \
                            -lOpenCL


#Add all special libraries used in the project
win32:INCLUDEPATH += D:\Projects\Dependencies\TBB\include \
                     D:\Projects\Dependencies\LOKI\include \
                     D:\Projects\Dependencies\GLEW\include \
                     D:\Projects\Dependencies\OPENCL\common\inc \
                     C:\Program Files\Microsoft DirectX SDK (June 2010)\Include

else:unix:INCLUDEPATH += $$PWD/../Dependencies/TBB/include \
                         /home/pourya/Desktop/Projects/Dependencies/OPENCL/common/inc \
                         /home/pourya/Desktop/Projects/Dependencies/LOKI/include

win32:DEPENDPATH += D:\Projects\Dependencies\TBB\include \
                    D:\Projects\Dependencies\LOKI\include \
                    D:\Projects\Dependencies\GLEW\include \
                    D:\Projects\Dependencies\OPENCL\common\inc \
                    C:\Program Files\Microsoft DirectX SDK (June 2010)\Include

else:unix:DEPENDPATH += $$PWD/../Dependencies/TBB/include \
                        /home/pourya/Desktop/Projects/Dependencies/OPENCL/common/inc \
                        /home/pourya/Desktop/Projects/Dependencies/LOKI/include














































