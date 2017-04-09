EXEC = \
    tests/test-matrix \
	tests/test-matrix_extend \
    tests/test-stopwatch \
	tests/test-random

GIT_HOOKS := .git/hooks/applied
OUT ?= .build
.PHONY: all
all: $(GIT_HOOKS) $(OUT) $(EXEC)

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

CC ?= gcc
CFLAGS = -Wall -std=gnu99 -g -I. -msse -mavx
LDFLAGS = -lpthread

OBJS := \
	stopwatch.o \
	matrix_naive.o\
	matrix_extend.o

deps := $(OBJS:%.o=%.o.d)
OBJS := $(addprefix $(OUT)/,$(OBJS))
deps := $(addprefix $(OUT)/,$(deps))

tests/test-%: $(OBJS) tests/test-%.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OUT)/%.o: %.c $(OUT)
	$(CC) $(CFLAGS) -c -o $@ -MMD -MF $@.d $<

$(OUT):
	@mkdir -p $@

cache:matrix_extend.o cache-test.c
	$(CC) $(CFLAGS) $(LDFLAGS) cache-test.c matrix_extend.o -o ./tests/naive
	$(CC) $(CFLAGS) $(LDFLAGS) -DSSE cache-test.c matrix_extend.o -o ./tests/sse
	$(CC) $(CFLAGS) $(LDFLAGS) -DAVX cache-test.c matrix_extend.o -o ./tests/avx
	$(CC) $(CFLAGS) $(LDFLAGS) -DTRAN cache-test.c matrix_extend.o -o ./tests/tran


check: $(EXEC)
	@for test in $^ ; \
	do \
		echo "Execute $$test..." ; $$test && echo "OK!\n" ; \
	done


clean:
	$(RM) $(EXEC) $(OBJS) $(deps) cache-test.o ./tests/avx ./tests/sse ./tests/naive ./tests/tran
	@rm -rf $(OUT) cache-test.o ./tests/avx ./tests/sse ./tests/sse ./test/tran

-include $(deps)
