
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
          -Wno-override-init -Wno-type-limits -Wno-unused-parameter

TPLRENDER ?= $(DEPS_DIR)/tplrender/tplrender


libvec_types   := int ptr_long
libarray_types := $(libvec_types)
libmaybe_types := $(libarray_types) size
libbase_types  := $(libmaybe_types)

int_type         := int
int_options      := --typeclasses NULL BOUNDED EQ ORD ENUM NUM FROM_STR \
                    --extra num_type=signed

ptr_long_type    := long const *
ptr_long_options := --typeclasses NULL EQ ORD

size_type        := size_t
size_options     := --typeclasses NULL BOUNDED EQ ORD ENUM NUM FROM_STR \
                    --extra num_type=unsigned

libbase_sources := $(foreach t,$(libbase_types),$(LIBBASE)/$t.c)
libbase_headers := $(libbase_sources:.c=.h)
libbase_objects := $(libbase_sources:.c=.o)

libmaybe_sources := $(foreach t,$(libmaybe_types),$(LIBMAYBE)/maybe_$t.c)
libmaybe_headers := $(libmaybe_sources:.c=.h)
libmaybe_defs    := $(foreach t,$(libmaybe_types),$(LIBMAYBE)/def/maybe_$t.h)
libmaybe_objects := $(libmaybe_sources:.c=.o)

libarray_sources := $(foreach t,$(libarray_types),$(LIBARRAY)/array_$t.c)
libarray_headers := $(libarray_sources:.c=.h)
libarray_defs    := $(foreach t,$(libarray_types),$(LIBARRAY)/def/array_$t.h)
libarray_objects := $(libarray_sources:.c=.o)

libvec_sources := $(foreach t,$(libvec_types),vec_$t.c)
libvec_headers := $(libvec_sources:.c=.h)
libvec_defs    := $(addprefix def/,$(libvec_headers))
libvec_objects := $(libvec_sources:.c=.o)

gen_objects := \
    $(libbase_objects) \
    $(libmaybe_objects) \
    $(libarray_objects) \
    $(libvec_objects)

gen := \
    $(libbase_sources) \
    $(libbase_headers) \
    $(libmaybe_defs) \
    $(libarray_sources) \
    $(libarray_headers) \
    $(libarray_defs) \
    $(libarray_objects) \
    $(libvec_sources) \
    $(libvec_headers) \
    $(libvec_defs) \
    $(libvec_objects) \
    $(gen_objects)

test_binaries := $(basename $(wildcard tests/*.c))

mkdeps := $(gen_objects:.o=.dep.mk)



#########################
### BUILDING
#########################

.PHONY: all
all: tests

.PHONY: tests
tests: $(test_binaries)

.PHONY: test
test: tests
	./tests/test

.PHONY: clean
clean:
	rm -rf $(gen) $(test_binaries) $(mkdeps)


%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -MMD -MF "$(@:.o=.dep.mk)" -c $< -o $@


tests/test: $(gen_objects)


$(libbase_headers): $(LIBBASE)/%.h: $(LIBBASE)/header.h.jinja
	$(TPLRENDER) $< "$($(*)_type)" $($(*)_options) -o $@

$(libbase_sources): $(LIBBASE)/%.c: $(LIBBASE)/source.c.jinja
	$(TPLRENDER) $< "$($(*)_type)" $($(*)_options) -o $@

$(libbase_objects): $(LIBBASE)/%.o: \
    $(LIBBASE)/%.h

$(libmaybe_defs): $(LIBMAYBE)/def/maybe_%.h: $(LIBMAYBE)/def.h.jinja
	$(TPLRENDER) $< "$($(*)_type)" $($(*)_options) -o $@

$(libmaybe_headers): $(LIBMAYBE)/maybe_%.h: $(LIBMAYBE)/header.h.jinja
	$(TPLRENDER) $< "$($(*)_type)" $($(*)_options) -o $@

$(libmaybe_sources): $(LIBMAYBE)/maybe_%.c: $(LIBMAYBE)/source.c.jinja
	$(TPLRENDER) $< "$($(*)_type)" $($(*)_options) --sys-headers "libbase/$*.h" -o $@

$(libmaybe_objects): $(LIBMAYBE)/maybe_%.o: \
    $(LIBBASE)/%.h \
    $(LIBMAYBE)/def/maybe_%.h \
    $(LIBMAYBE)/maybe_%.h

$(libarray_defs): $(LIBARRAY)/def/array_%.h: $(LIBARRAY)/def.h.jinja
	$(TPLRENDER) $< "$($(*)_type)" $($(*)_options) -o $@

$(libarray_headers): $(LIBARRAY)/array_%.h: $(LIBARRAY)/header.h.jinja
	$(TPLRENDER) $< "$($(*)_type)" $($(*)_options) -o $@

$(libarray_sources): $(LIBARRAY)/array_%.c: $(LIBARRAY)/source.c.jinja
	$(TPLRENDER) $< "$($(*)_type)" $($(*)_options) --sys-headers "libbase/$*.h" -o $@

$(libarray_objects): $(LIBARRAY)/array_%.o: \
    $(LIBBASE)/size.h \
    $(LIBBASE)/%.h \
    $(LIBMAYBE)/def/maybe_size.h \
    $(LIBMAYBE)/def/maybe_%.h \
    $(LIBMAYBE)/maybe_%.h \
    $(LIBARRAY)/def/array_%.h \
    $(LIBARRAY)/array_%.h

$(libvec_defs): def/vec_%.h: def.h.jinja
	$(TPLRENDER) $< "$($(*)_type)" $($(*)_options) -o $@

$(libvec_headers): vec_%.h: header.h.jinja
	$(TPLRENDER) $< "$($(*)_type)" $($(*)_options) -o $@

$(libvec_sources): vec_%.c: source.c.jinja
	$(TPLRENDER) $< "$($(*)_type)" $($(*)_options) --sys-headers "libbase/$*.h" -o $@

$(libvec_objects): vec_%.o: \
    $(LIBBASE)/size.h \
    $(LIBBASE)/%.h \
    $(LIBMAYBE)/def/maybe_size.h \
    $(LIBMAYBE)/def/maybe_%.h \
    $(LIBMAYBE)/maybe_%.h \
    $(LIBARRAY)/def/array_%.h \
    $(LIBARRAY)/array_%.h \
    def/vec_%.h \
    vec_%.h


-include $(mkdeps)


