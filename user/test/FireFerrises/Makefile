PHONY += default
default: all

PHONY += all
all: mmapper

mmapper: mmapper.c
	gcc -Wall -g mmapper.c -o mmapper

mmapper.c:
	cp ../cabinet_inspector/mmapper.c .
	sed -i 's/pause()/fflush(stdout);\n\tpause()/' mmapper.c

PHONY += clean
clean:
	rm -rf $(cat .gitignore)

PHONY += test
test:
	$(MAKE) -C ../cabinet_inspector/ all
	sudo ../cabinet_inspector/cabinet_inspector
	sudo ./test.py

.PHONY: $(PHONY)
