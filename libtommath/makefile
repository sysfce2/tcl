#Makefile for GCC
#
#Tom St Denis

ifeq ($V,1)
silent=
else
silent=@
endif

#default files to install
ifndef LIBNAME
   LIBNAME=libtommath.a
endif

coverage: LIBNAME:=-Wl,--whole-archive $(LIBNAME)  -Wl,--no-whole-archive

include makefile_include.mk

%.o: %.c $(HEADERS)
ifneq ($V,1)
	@echo "   * ${CC} $@"
endif
	${silent} ${CC} -c ${LTM_CFLAGS} $< -o $@

LCOV_ARGS=--directory .

#START_INS
OBJECTS=bn_cutoffs.o bn_deprecated.o bn_mp_2expt.o bn_mp_abs.o bn_mp_add.o bn_mp_add_d.o bn_mp_addmod.o \
bn_mp_and.o bn_mp_clamp.o bn_mp_clear.o bn_mp_clear_multi.o bn_mp_cmp.o bn_mp_cmp_d.o bn_mp_cmp_mag.o \
bn_mp_cnt_lsb.o bn_mp_complement.o bn_mp_copy.o bn_mp_count_bits.o bn_mp_decr.o bn_mp_div.o bn_mp_div_2.o \
bn_mp_div_2d.o bn_mp_div_d.o bn_mp_dr_is_modulus.o bn_mp_dr_reduce.o bn_mp_dr_setup.o \
bn_mp_error_to_string.o bn_mp_exch.o bn_mp_expt_n.o bn_mp_exptmod.o bn_mp_exteuclid.o bn_mp_fread.o \
bn_mp_from_sbin.o bn_mp_from_ubin.o bn_mp_fwrite.o bn_mp_gcd.o bn_mp_get_double.o bn_mp_get_i32.o \
bn_mp_get_i64.o bn_mp_get_l.o bn_mp_get_mag_u32.o bn_mp_get_mag_u64.o bn_mp_get_mag_ul.o bn_mp_grow.o \
bn_mp_incr.o bn_mp_init.o bn_mp_init_copy.o bn_mp_init_i32.o bn_mp_init_i64.o bn_mp_init_l.o \
bn_mp_init_multi.o bn_mp_init_set.o bn_mp_init_size.o bn_mp_init_u32.o bn_mp_init_u64.o bn_mp_init_ul.o \
bn_mp_invmod.o bn_mp_is_square.o bn_mp_iseven.o bn_mp_isodd.o bn_mp_kronecker.o bn_mp_lcm.o bn_mp_log_n.o \
bn_mp_lshd.o bn_mp_mod.o bn_mp_mod_2d.o bn_mp_mod_d.o bn_mp_montgomery_calc_normalization.o \
bn_mp_montgomery_reduce.o bn_mp_montgomery_setup.o bn_mp_mul.o bn_mp_mul_2.o bn_mp_mul_2d.o bn_mp_mul_d.o \
bn_mp_mulmod.o bn_mp_neg.o bn_mp_or.o bn_mp_pack.o bn_mp_pack_count.o bn_mp_prime_fermat.o \
bn_mp_prime_frobenius_underwood.o bn_mp_prime_is_prime.o bn_mp_prime_miller_rabin.o \
bn_mp_prime_next_prime.o bn_mp_prime_rabin_miller_trials.o bn_mp_prime_rand.o \
bn_mp_prime_strong_lucas_selfridge.o bn_mp_radix_size.o bn_mp_radix_smap.o bn_mp_rand.o \
bn_mp_read_radix.o bn_mp_reduce.o bn_mp_reduce_2k.o bn_mp_reduce_2k_l.o bn_mp_reduce_2k_setup.o \
bn_mp_reduce_2k_setup_l.o bn_mp_reduce_is_2k.o bn_mp_reduce_is_2k_l.o bn_mp_reduce_setup.o \
bn_mp_root_n.o bn_mp_rshd.o bn_mp_sbin_size.o bn_mp_set.o bn_mp_set_double.o bn_mp_set_i32.o \
bn_mp_set_i64.o bn_mp_set_l.o bn_mp_set_u32.o bn_mp_set_u64.o bn_mp_set_ul.o bn_mp_shrink.o \
bn_mp_signed_rsh.o bn_mp_sqr.o bn_mp_sqrmod.o bn_mp_sqrt.o bn_mp_sqrtmod_prime.o bn_mp_sub.o bn_mp_sub_d.o \
bn_mp_submod.o bn_mp_to_radix.o bn_mp_to_sbin.o bn_mp_to_ubin.o bn_mp_ubin_size.o bn_mp_unpack.o \
bn_mp_xor.o bn_mp_zero.o bn_prime_tab.o bn_s_mp_add.o bn_s_mp_balance_mul.o bn_s_mp_div_3.o \
bn_s_mp_exptmod.o bn_s_mp_exptmod_fast.o bn_s_mp_get_bit.o bn_s_mp_invmod_fast.o bn_s_mp_invmod_slow.o \
bn_s_mp_karatsuba_mul.o bn_s_mp_karatsuba_sqr.o bn_s_mp_log.o bn_s_mp_log_2expt.o bn_s_mp_log_d.o \
bn_s_mp_montgomery_reduce_fast.o bn_s_mp_mul_digs.o bn_s_mp_mul_digs_fast.o bn_s_mp_mul_high_digs.o \
bn_s_mp_mul_high_digs_fast.o bn_s_mp_prime_is_divisible.o bn_s_mp_rand_jenkins.o \
bn_s_mp_rand_platform.o bn_s_mp_reverse.o bn_s_mp_sqr.o bn_s_mp_sqr_fast.o bn_s_mp_sub.o \
bn_s_mp_toom_mul.o bn_s_mp_toom_sqr.o

#END_INS

$(LIBNAME):  $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)
	$(RANLIB) $@

#make a profiled library (takes a while!!!)
#
# This will build the library with profile generation
# then run the test demo and rebuild the library.
#
# So far I've seen improvements in the MP math
profiled:
	make CFLAGS="$(CFLAGS) -fprofile-arcs -DTESTING" timing
	./timing
	rm -f *.a *.o timing
	make CFLAGS="$(CFLAGS) -fbranch-probabilities"

#make a single object profiled library
profiled_single:
	perl gen.pl
	$(CC) $(LTM_CFLAGS) -fprofile-arcs -DTESTING -c mpi.c -o mpi.o
	$(CC) $(LTM_CFLAGS) -DTESTING -DTIMER demo/timing.c mpi.o -lgcov -o timing
	./timing
	rm -f *.o timing
	$(CC) $(LTM_CFLAGS) -fbranch-probabilities -DTESTING -c mpi.c -o mpi.o
	$(AR) $(ARFLAGS) $(LIBNAME) mpi.o
	ranlib $(LIBNAME)

install: $(LIBNAME)
	install -d $(DESTDIR)$(LIBPATH)
	install -d $(DESTDIR)$(INCPATH)
	install -m 644 $(LIBNAME) $(DESTDIR)$(LIBPATH)
	install -m 644 $(HEADERS_PUB) $(DESTDIR)$(INCPATH)

uninstall:
	rm $(DESTDIR)$(LIBPATH)/$(LIBNAME)
	rm $(HEADERS_PUB:%=$(DESTDIR)$(INCPATH)/%)

test_standalone: test
	@echo "test_standalone is deprecated, please use make-target 'test'"

DEMOS=test mtest_opponent

define DEMO_template
$(1): demo/$(1).o demo/shared.o $$(LIBNAME)
	$$(CC) $$(LTM_CFLAGS) $$(LTM_LFLAGS) $$^ -o $$@
endef

$(foreach demo, $(strip $(DEMOS)), $(eval $(call DEMO_template,$(demo))))

.PHONY: mtest
mtest:
	cd mtest ; $(CC) $(LTM_CFLAGS) -O0 mtest.c $(LTM_LFLAGS) -o mtest

timing: $(LIBNAME) demo/timing.c
	$(CC) $(LTM_CFLAGS) -DTIMER demo/timing.c $(LIBNAME) $(LTM_LFLAGS) -o timing

tune: $(LIBNAME)
	$(MAKE) -C etc tune CFLAGS="$(LTM_CFLAGS)"
	$(MAKE)

# You have to create a file .coveralls.yml with the content "repo_token: <the token>"
# in the base folder to be able to submit to coveralls
coveralls: lcov
	coveralls-lcov

docs manual:
	$(MAKE) -C doc/ $@ V=$(V)

.PHONY: pre_gen
pre_gen:
	mkdir -p pre_gen
	perl gen.pl
	sed -e 's/[[:blank:]]*$$//' mpi.c > pre_gen/mpi.c
	rm mpi.c

zipup:
	$(MAKE) clean
	$(MAKE) .zipup

.zipup: astyle new_file docs
	@# Update the index, so diff-index won't fail in case the pdf has been created.
	@#   As the pdf creation modifies the tex files, git sometimes detects the
	@#   modified files, but misses that it's put back to its original version.
	@git update-index --refresh
	@git diff-index --quiet HEAD -- || ( echo "FAILURE: uncommited changes or not a git" && exit 1 )
	rm -rf libtommath-$(VERSION) ltm-$(VERSION).*
	@# files/dirs excluded from "git archive" are defined in .gitattributes
	git archive --format=tar --prefix=libtommath-$(VERSION)/ HEAD | tar x
	@echo 'fixme check'
	-@(find libtommath-$(VERSION)/ -type f | xargs grep 'FIXM[E]') && echo '############## BEWARE: the "fixme" marker was found !!! ##############' || true
	mkdir -p libtommath-$(VERSION)/doc
	cp doc/bn.pdf libtommath-$(VERSION)/doc/
	$(MAKE) -C libtommath-$(VERSION)/ pre_gen
	tar -c libtommath-$(VERSION)/ | xz -6e -c - > ltm-$(VERSION).tar.xz
	zip -9rq ltm-$(VERSION).zip libtommath-$(VERSION)
	cp doc/bn.pdf bn-$(VERSION).pdf
	rm -rf libtommath-$(VERSION)
	gpg -b -a ltm-$(VERSION).tar.xz
	gpg -b -a ltm-$(VERSION).zip

new_file:
	perl helper.pl --update-files

perlcritic:
	perlcritic *.pl doc/*.pl

astyle:
	@echo "   * run astyle on all sources"
	@astyle --options=astylerc --formatted $(OBJECTS:.o=.c) tommath*.h demo/*.c etc/*.c mtest/mtest.c
