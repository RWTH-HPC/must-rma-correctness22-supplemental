set(ABSL_PROPAGATE_CXX_STD ON)

FetchContent_Declare(
  cpp-abseil
  GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
  GIT_TAG 215105818dfde3174fe799600bb0f3cae233d0bf # LTS Nov. 2021
  GIT_SHALLOW 1
)

FetchContent_MakeAvailable(cpp-abseil)

mark_as_advanced(
  ABSL_ENABLE_INSTALL
  ABSL_GOOGLETEST_DOWNLOAD_URL
  ABSL_LOCAL_GOOGLETEST_DIR
  ABSL_RUN_TESTS
  ABSL_USE_EXTERNAL_GOOGLETEST
  ABSL_USE_GOOGLETEST_HEAD
  ABSL_PROPAGATE_CXX_STD
  ABSL_BUILD_TESTING
  BUILD_TESTING
)

mark_as_advanced(
  FETCHCONTENT_SOURCE_DIR_CPP-ABSEIL
  FETCHCONTENT_UPDATES_DISCONNECTED_CPP-ABSEIL
)
