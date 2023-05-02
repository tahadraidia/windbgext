CC=cl.exe
CFLAGS=/nologo /MT /LD /std:c++17 /EHsc

fh.dll:
	$(CC) foobar.cpp windbgext.cpp $(CFLAGS)

clean:
	del *.dll
	del *.lib
	del *.exp
	del *.obj

lint:
	cpplint ./foobar.cpp
