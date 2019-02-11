install_path = /usr
outdir = out
outs = $(outdir)/cgic.o $(outdir)/CgiManager.o $(outdir)/SqliteManager.o $(outdir)/sqlite3secure.o

libcgi_manager.so: $(outs)
	g++ -shared -o $(outdir)/libcgi_manager.so $(outs)

out/cgic.o: CgiManager/cgic.c
	@[ -d $(outdir) ] || mkdir $(outdir)
	gcc -fpic -c CgiManager/cgic.c -o out/cgic.o

out/CgiManager.o: CgiManager/CgiManager.cpp
	@[ -d $(outdir) ] || mkdir $(outdir)
	g++ -fpic -c CgiManager/CgiManager.cpp -o out/CgiManager.o

out/SqliteManager.o: CgiManager/SqliteManager.cpp 
	@[ -d $(outdir) ] || mkdir $(outdir)
	g++ -fpic -c CgiManager/SqliteManager.cpp -o out/SqliteManager.o

out/sqlite3secure.o: CgiManager/sqlite3/sqlite3secure.c
	@[ -d $(outdir) ] || mkdir $(outdir)
	gcc -fpic -c CgiManager/sqlite3/sqlite3secure.c -o out/sqlite3secure.o

clean:
	@rm -rf $(outdir)

install:
	@echo "Installing header files to $(install_path)/include/CgiManager"
	@mkdir -p $(install_path)/include/CgiManager/sqlite3/
	@install -p -m 0755 CgiManager/*.h $(install_path)/include/CgiManager/
	@install -p -m 0755 CgiManager/sqlite3/*.h $(install_path)/include/CgiManager/sqlite3/
	@echo "Installing shared library to $(install_path)/lib"
	@mkdir -p $(install_path)/lib
	@install -p -m 0755 $(outdir)/libcgi_manager.so $(install_path)/lib/
	@ldconfig
