CC := cl
CFLAGS := /c
obj := so_stdio.obj utils.obj ErrorCheck.obj

build: so_stdio.dll

so_stdio.dll: $(obj)
	link /nologo /dll /out:so_stdio.dll /implib:so_stdio.lib $(obj)
	
so_stdio.obj: so_stdio.c so_stdio.h
	$(CC) $(CFLAGS) $< /Fo$@

utils.obj: utils.c utils.h
	$(CC) $(CFLAGS) $< /Fo$@

ErrorCheck.obj: ErrorCheck.c ErrorCheck.h so_stdio.h utils.h
	$(CC) $(CFLAGS) $< /Fo$@

clean:
	del *.obj