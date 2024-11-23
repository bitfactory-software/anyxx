
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO bitfactory-software/virtual_void
    REF "${VERSION}"
    SHA512 0a81e526461aeddf711210dec82ebbd158f233eeadfea55c00bdd6cd5fc79fd91c04e7f9f7804d01937c7cd189cd1f28ae8c256806a5e79cb39acb5e05e4440d
)


vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(PACKAGE_NAME "virtual_void")

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
configure_file("${CMAKE_CURRENT_LIST_DIR}/usage" "${CURRENT_PACKAGES_DIR}/share/${PORT}/usage" COPYONLY)