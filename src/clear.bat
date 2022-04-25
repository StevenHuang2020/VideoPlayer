:: clean all files no needed

:: clean debug folder
cd debug
del qrc_*.cpp/s
del moc_*.cpp/s
del moc_*.h/s
del VideoPlayer*.txt/s
rmdir /S /Q qmake
rmdir /S /Q "VideoPlayer.tlog"

:: clean release folder
cd ../release
del qrc_*.cpp/s
del moc_*.cpp/s
del moc_*.h/s
del VideoPlayer*.txt/s
rmdir /S /Q qmake
rmdir /S /Q "VideoPlayer.tlog"

:: clean source folder
cd ..
rmdir /S /Q ".vs"

del *.ncb/s
rem del *.exe/s
del *.exp/s
del *.pdb/s
del *.obj/s

del *.idb/s
del *.pch/s

del *.msi/s
del *.tmp/s
del *.ilk/s
del *.log/s
del *.recipe/s

cd ..

pause