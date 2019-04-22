import qbs
import qbs.Environment
import "googlecommon.js" as googleCommon

CppApplication {
    consoleApplication: true

    property string googletestDir: {
        if (typeof Environment.getEnv("GOOGLETEST_DIR") === 'undefined') {
            console.warn("Using googletest src dir specified at Qt Creator wizard")
            console.log("set GOOGLETEST_DIR as environment variable or Qbs property to get rid of this message")
            return "/Users/zhujinli/googletest"
        } else {
            return Environment.getEnv("GOOGLETEST_DIR")
        }
    }

    cpp.cxxLanguageVersion: "c++11"
    cpp.defines: [ "GTEST_LANG_CXX11" ]
    cpp.dynamicLibraries: {
        if (qbs.hostOS.contains("windows")) {
            return [];
        } else {
            return [ "pthread" ];
        }
    }


    cpp.includePaths: [].concat(googleCommon.getGTestIncludes(qbs, googletestDir))
    .concat(googleCommon.getGMockIncludes(qbs, googletestDir))

    files: [
        "../contentmanager.h",
    ].concat([
        "../contentmanager.cpp",
    ].concat([
        "main.cpp",
        "tst_contentmanager.cpp",
    ].concat(googleCommon.getGTestAll(qbs, googletestDir))
    .concat(googleCommon.getGMockAll(qbs, googletestDir))))
}
