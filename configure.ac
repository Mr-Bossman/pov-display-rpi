AC_INIT([pov], [0.1])
AM_INIT_AUTOMAKE([foreign subdir-objects])

AC_PROG_CXX
AC_PROG_CC
AC_LANG(C)
AC_LANG(C++)
AC_PROG_CXX
AC_LANG_PUSH([C++])

AC_COMPILE_IFELSE([AC_LANG_SOURCE(
  [[template <typename T>
    struct check
    {
      static_assert(sizeof(int) <= sizeof(T), "not big enough");
    };

    typedef check<check<bool>> right_angle_brackets;

    int a;
    decltype(a) b;

    typedef check<int> check_type;
    check_type c;
    check_type&& cr = static_cast<check_type&&>(c);]])],,
  AC_MSG_FAILURE(['$CXX $CXXFLAGS' does not accept ISO C++11]))
AC_CONFIG_HEADERS([pov-config.h])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT