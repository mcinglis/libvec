
##############################
### VARIABLES
##############################

DEPS_DIR ?= ./deps

LIBBASE  ?= $(DEPS_DIR)/libbase
LIBMAYBE ?= $(DEPS_DIR)/libmaybe
LIBARRAY ?= $(DEPS_DIR)/libarray

CPPFLAGS += -I$(DEPS_DIR)

CFLAGS ?= -std=c11 -g \
          -Wall -Wextra -pedantic \
          -Wcomments -Wformat=2 -Wlogical-op -Wmissing-include-dirs \
          -Wnested-externs -Wold-style-definition -Wredundant-decls \
          -Wshadow -Wstrict-prototypes -Wunused-macros -Wvla -Wwrite-strings \
          -Wno-override-init -Wno-unused-parameter

TPLRENDER ?= $(DEPS_DIR)/tplrender/tplrender


test_libvec_types   := int ptr-long
test_libbase_types  := $(test_libvec_types) size
test_libmaybe_types := size
test_libarray_types := $(test_libvec_types)

int_type         := int
int_options      := --typeclasses BOUNDED EQ ORD ENUM NUM \
                    --extra num_type=signed

ptr_long_type    := long const *
ptr_long_options := --typeclasses EQ ORD

size_type        := size_t
size_options     := --typeclasses BOUNDED EQ ORD ENUM NUM \
                    --extra num_type=unsigned

test_libbase_sources := $(foreach t,$(test_libbase_types),$(LIBBASE)/$t.c)
test_libbase_headers := $(test_libbase_sources:.c=.h)
test_libbase_objects := $(test_libbase_sources:.c=.o)

test_libmaybe_defs := $(foreach t,$(test_libmaybe_types),$(LIBMAYBE)/def/maybe-$t.h)

test_libarray_sources := $(foreach t,$(test_libarray_types),$(LIBARRAY)/array-$t.c)
test_libarray_headers := $(test_libarray_sources:.c=.h)
test_libarray_defs    := $(foreach t,$(test_libarray_types),$(LIBARRAY)/def/array-$t.h)
test_libarray_objects := $(test_libarray_sources:.c=.o)

test_libvec_sources := $(foreach t,$(test_libvec_types),vec-$t.c)
test_libvec_headers := $(test_libvec_sources:.c=.h)
test_libvec_defs    := $(addprefix def/,$(test_libvec_headers))
test_libvec_objects := $(test_libvec_sources:.c=.o)

test_gen_objects := $(test_libbase_objects) \
                    $(test_libarray_objects) \
                    $(test_libvec_objects)

test_gen := $(test_libbase_sources) \
            $(test_libbase_headers) \
            $(test_libmaybe_defs) \
            $(test_libarray_sources) \
            $(test_libarray_headers) \
            $(test_libarray_defs) \
            $(test_libarray_objects) \
            $(test_libvec_sources) \
            $(test_libvec_headers) \
            $(test_libvec_defs) \
            $(test_libvec_objects) \
            $(test_gen_objects)

test_binaries := $(basename $(wildcard tests/*.c))

mkdeps := $(test_gen_objects:.o=.dep.mk)




##############################
### BUILDING
##############################

.PHONY: all
all:

.PHONY: tests
tests: $(test_binaries)

.PHONY: test
test: tests
	./tests/test

.PHONY: clean
clean:
	rm -rf $(test_gen) $(test_binaries) $(mkdeps)


%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -MMD -MF "$(@:.o=.dep.mk)" -c $< -o $@


tests/test: $(test_gen_objects)

name_from_path = $(subst -,_,$1)

$(test_libbase_headers): $(LIBBASE)/%.h: $(LIBBASE)/header.h.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) -o $@

$(test_libbase_sources): $(LIBBASE)/%.c: $(LIBBASE)/source.c.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) -o $@

$(test_libbase_objects): $(LIBBASE)/%.o: $(LIBBASE)/%.h

$(test_libmaybe_defs): $(LIBMAYBE)/def/maybe-%.h: $(LIBMAYBE)/def.h.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) -o $@

$(test_libarray_defs): $(LIBARRAY)/def/array-%.h: $(LIBARRAY)/def.h.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) -o $@

$(test_libarray_headers): $(LIBARRAY)/array-%.h: $(LIBARRAY)/header.h.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) -o $@

$(test_libarray_sources): $(LIBARRAY)/array-%.c: $(LIBARRAY)/source.c.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) --sys-headers "libbase/$*.h" -o $@

$(test_libarray_objects): $(LIBARRAY)/array-%.o: \
    $(LIBARRAY)/array-%.h \
    $(LIBARRAY)/def/array-%.h \
    $(LIBBASE)/%.h \
    $(LIBBASE)/size.h \
    $(LIBMAYBE)/def/maybe-size.h

$(test_libvec_defs): def/vec-%.h: def.h.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) -o $@

$(test_libvec_headers): vec-%.h: header.h.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) -o $@

$(test_libvec_sources): vec-%.c: source.c.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) --sys-headers "libbase/$*.h" -o $@

$(test_libvec_objects): vec-%.o: \
    vec-%.h \
    def/vec-%.h \
    $(LIBBASE)/%.h \
    $(LIBBASE)/size.h \
    $(LIBARRAY)/def/array-%.h \
    $(LIBARRAY)/array-%.h \
    $(LIBMAYBE)/def/maybe-size.h


-include $(mkdeps)


