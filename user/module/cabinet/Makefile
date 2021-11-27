MAKE_DIR := /lib/modules/$(shell uname -r)/build

obj-m += cabinet.o

CFLAGS_cabinet.o += -Wall -Werror -Wextra

KASAN_SANITIZE_cabinet.o := $(CONFIG_KASAN)
UBSAN_SANITIZE_cabinet.o := $(CONFIG_UBSAN)

all:
	$(MAKE) -C $(MAKE_DIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(MAKE_DIR) M=$(PWD) clean
