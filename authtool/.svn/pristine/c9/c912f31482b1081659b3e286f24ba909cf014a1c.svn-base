
IPVSAxAuthorps.dll: dlldata.obj IPVSAxAuthor_p.obj IPVSAxAuthor_i.obj
	link /dll /out:IPVSAxAuthorps.dll /def:IPVSAxAuthorps.def /entry:DllMain dlldata.obj IPVSAxAuthor_p.obj IPVSAxAuthor_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del IPVSAxAuthorps.dll
	@del IPVSAxAuthorps.lib
	@del IPVSAxAuthorps.exp
	@del dlldata.obj
	@del IPVSAxAuthor_p.obj
	@del IPVSAxAuthor_i.obj
