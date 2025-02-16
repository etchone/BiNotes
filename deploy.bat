mkdir deploy-win
copy build\Desktop_Qt_6_8_2_MSVC2022_64bit-Release\release\BiNotes.exe deploy-win\
"D:\Qt\6.8.2\msvc2022_64\bin\windeployqt.exe" deploy-win\BiNotes.exe
copy BiNotes_zh.qm deploy-win\translations\
del /q deploy\packages\com.h1msk.binotes\data\*
xcopy /s /y deploy-win\* deploy\packages\com.h1msk.binotes\data\
rmdir /s /q deploy-win
cd deploy
C:\Qt\Tools\QtInstallerFramework\4.6\bin\binarycreator.exe -c config\binotes.xml -p packages BiNotes-Setup-1.0.1.exe
cd ..
