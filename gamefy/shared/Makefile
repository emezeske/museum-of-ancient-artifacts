all: package

clean:
	rm -rf *.o shared.tar shared.tgz

package: clean
	tar -cvvf shared.tar ./
	gzip -c shared.tar > shared.tgz

