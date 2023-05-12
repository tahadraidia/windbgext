CC=cl.exe
CFLAGS=/nologo /MT /LD /std:c++17 /EHsc /W4

foobar.dll:
	$(CC) foobar.cpp windbgext.cpp $(CFLAGS)

clean:
	del *.dll
	del *.lib
	del *.exp
	del *.obj
