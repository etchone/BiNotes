mkdir deploy-win
copy ..\build-BiNotes-Desktop_Qt_6_2_4_MSVC2019_64bit-Release\release\BiNotes.exe deploy-win\
"C:\Qt\6.2.4\msvc2019_64\bin\windeployqt.exe" deploy-win\BiNotes.exe
copy BiNotes_zh.qm deploy-win\translations\
del /q deploy\packages\com.h1msk.binotes\data\*
xcopy /s /y deploy-win\* deploy\packages\com.h1msk.binotes\data\
rmdir /s /q deploy-win
cd deploy
pack-binotes.bat
cd ..
