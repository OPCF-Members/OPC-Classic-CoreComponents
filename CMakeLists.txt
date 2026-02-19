cmake_minimum_required(VERSION 3.20)
project(OpcCoreComponents C CXX)

# ------------------------------------------------------------------------------
# Require MSVC CRT >= 14.44.35211 (Windows 7 SP1 compatible)
# MSVC compiler 19.44.x ships CRT 14.44.x — check compiler version.
# ---------------------------------------------------------------------------
if(MSVC)
    if(CMAKE_C_COMPILER_VERSION VERSION_LESS "19.44.35211")
        message(FATAL_ERROR
            "MSVC CRT >= 14.44.35211 required (found compiler ${CMAKE_C_COMPILER_VERSION}).\n"
            "Install the MSVC v14.44 toolset via VS Installer and/or pass -T v143,version=14.44.xxxxx to cmake.")
    endif()
    string(REGEX REPLACE "^19\\." "14." OPC_CRT_VERSION "${CMAKE_C_COMPILER_VERSION}")
    message(STATUS "MSVC compiler: ${CMAKE_C_COMPILER_VERSION} (CRT ${OPC_CRT_VERSION})")
endif()

# Enable /Qspectre if Spectre-mitigated libs are installed (VS Individual Components)
option(OPC_ENABLE_SPECTRE "Enable Spectre v1 mitigation (/Qspectre)" OFF)

# Build the OPC DA test server and client applications
option(OPC_BUILD_TESTS "Build OPC DA test server and client" ON)

# Include the reusable proxy stub build function
include(cmake/OpcProxyStub.cmake)

# ============================================================================
# OPC Common Proxy Stub
# ============================================================================
add_opc_proxy_stub(
    TARGET  opccomn_ps
    IDL     opccomn.idl
    DEF     opccomn_ps.def
    RC      opccomn_ps.rc
    TLB     opccomn.tlb
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/Source/Common/ProxyStub"
)

# ============================================================================
# OPC Data Access Proxy Stub
# ============================================================================
add_opc_proxy_stub(
    TARGET  opcproxy
    IDL     opcda.idl
    DEF     opcproxy.def
    RC      opcda_ps.rc
    TLB     opcda.tlb
    MC      opcerror.mc
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/Source/DataAccess/ProxyStub"
)

# ============================================================================
# OPC Alarms and Events Proxy Stub
# ============================================================================
add_opc_proxy_stub(
    TARGET  opc_aeps
    IDL     opc_ae.idl
    DEF     opc_aeps.def
    RC      opc_ae.rc
    TLB     opc_ae.tlb
    MC      opcae_er.mc
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/Source/AlarmsAndEvents/ProxyStub"
)

# ============================================================================
# OPC Batch Proxy Stub
# ============================================================================
add_opc_proxy_stub(
    TARGET  opcbc_ps
    IDL     opcbc.idl
    DEF     opcbc_ps.def
    RC      opcbc_ps.rc
    TLB     opcbc.tlb
    MC      OpcBatchError.mc
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/Source/Batch/ProxyStub"
)

# ============================================================================
# OPC Commands Proxy Stub
# ============================================================================
add_opc_proxy_stub(
    TARGET  OpcCmdPs
    IDL     OpcCmd.idl
    DEF     OpcCmdPs.def
    RC      OpcCmdPs.rc
    TLB     opccmd.tlb
    MC      OpcCmdError.mc
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/Source/Commands/ProxyStub"
)

# ============================================================================
# OPC Data Exchange Proxy Stub
# ============================================================================
add_opc_proxy_stub(
    TARGET  OpcDxPs
    IDL     OpcDx.idl
    DEF     OpcDxPs.def
    RC      OpcDxPs.rc
    TLB     opcdx.tlb
    MC      OpcDxError.mc
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/Source/DataExchange/ProxyStub"
)

# ============================================================================
# OPC Historical Data Access Proxy Stub
# ============================================================================
add_opc_proxy_stub(
    TARGET  opchda_ps
    IDL     opchda.idl
    DEF     opchda_ps.def
    RC      opchda_ps.rc
    TLB     opchda.tlb
    MC      OpcHda_Error.mc
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/Source/HistoricalDataAccess/ProxyStub"
)

# ============================================================================
# OPC Security Proxy Stub
# ============================================================================
add_opc_proxy_stub(
    TARGET  opcsec_ps
    IDL     opcSec.idl
    DEF     opcSec_ps.def
    RC      opcSec_PS.rc
    TLB     opcsec.tlb
    MC      OpcErrSec.mc
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/Source/Security/ProxyStub"
)

# ============================================================================
# OPC Server Enumerator (OpcEnum.exe) — x86 only
# ============================================================================
if(CMAKE_SIZEOF_VOID_P EQUAL 4)

set(OPCENUM_SRC_DIR "${CMAKE_SOURCE_DIR}/Source/Common/ServerEnumerator")
set(OPCENUM_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/OpcEnum")
file(MAKE_DIRECTORY "${OPCENUM_BIN_DIR}")

# MIDL: OpcEnum.idl → OpcEnum.h, OpcEnum_i.c, OpcEnum.tlb
add_custom_command(
    OUTPUT
        "${OPCENUM_BIN_DIR}/OpcEnum.h"
        "${OPCENUM_BIN_DIR}/OpcEnum_i.c"
        "${OPCENUM_BIN_DIR}/OpcEnum.tlb"
    COMMAND midl /nologo /no_robust /env win32
        /I "${CMAKE_SOURCE_DIR}/Source/Common/ProxyStub"
        /tlb "${OPCENUM_BIN_DIR}/OpcEnum.tlb"
        /h   "${OPCENUM_BIN_DIR}/OpcEnum.h"
        /iid "${OPCENUM_BIN_DIR}/OpcEnum_i.c"
        "${OPCENUM_SRC_DIR}/OpcEnum.idl"
    DEPENDS "${OPCENUM_SRC_DIR}/OpcEnum.idl"
            "${CMAKE_SOURCE_DIR}/Source/Common/ProxyStub/opccomn.idl"
    WORKING_DIRECTORY "${OPCENUM_SRC_DIR}"
    COMMENT "MIDL: OpcEnum.idl"
    VERBATIM
)

# Use MIDL output from opccomn_ps for opccomn.h / opccomn_i.c
set(OPCCOMN_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/opccomn_ps")

add_executable(OpcEnum
    "${OPCENUM_SRC_DIR}/OpcEnum.cpp"
    "${OPCENUM_SRC_DIR}/OpcSeLst.cpp"
    "${OPCENUM_SRC_DIR}/StdAfx.cpp"
    "${OPCENUM_SRC_DIR}/DCOM Config/DComPerm.cpp"
    "${OPCENUM_SRC_DIR}/DCOM Config/AclMgmt.cpp"
    "${OPCENUM_SRC_DIR}/DCOM Config/ListAcl.cpp"
    "${OPCENUM_SRC_DIR}/DCOM Config/SDMgmt.cpp"
    "${OPCENUM_SRC_DIR}/DCOM Config/SrvcMgmt.cpp"
    "${OPCENUM_SRC_DIR}/DCOM Config/UtilS.cpp"
    "${OPCENUM_SRC_DIR}/DCOM Config/Wrappers.cpp"
    "${OPCENUM_SRC_DIR}/OpcEnum.rc"
)

add_custom_target(OpcEnum_generate DEPENDS
    "${OPCENUM_BIN_DIR}/OpcEnum.h"
    "${OPCENUM_BIN_DIR}/OpcEnum_i.c"
    "${OPCENUM_BIN_DIR}/OpcEnum.tlb"
)
add_dependencies(OpcEnum OpcEnum_generate)
# OpcEnum needs opccomn.h from opccomn_ps MIDL output
add_dependencies(OpcEnum opccomn_ps_generate)

target_compile_definitions(OpcEnum PRIVATE
    WIN32 _WINDOWS _WIN32_DCOM _CRT_SECURE_NO_DEPRECATE NDEBUG
)

target_include_directories(OpcEnum PRIVATE
    "${OPCENUM_BIN_DIR}"
    "${OPCENUM_SRC_DIR}"
    "${OPCCOMN_BIN_DIR}"
    "${CMAKE_SOURCE_DIR}/Source/Include"
)

set_property(TARGET OpcEnum PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
target_compile_options(OpcEnum PRIVATE /GS /sdl /guard:cf /wd4996)
if(OPC_ENABLE_SPECTRE)
    target_compile_options(OpcEnum PRIVATE /Qspectre)
endif()

target_link_options(OpcEnum PRIVATE /NXCOMPAT /DYNAMICBASE /GUARD:CF /SAFESEH /SUBSYSTEM:WINDOWS,6.01)
target_link_libraries(OpcEnum PRIVATE rpcrt4 ole32 oleaut32 advapi32)

set_target_properties(OpcEnum PROPERTIES
    OUTPUT_NAME "OpcEnum"
    WIN32_EXECUTABLE TRUE
)

install(TARGETS OpcEnum RUNTIME DESTINATION bin)

endif() # x86 only

# ============================================================================
# OPC Category Manager (OpcCategoryManager.exe) — x64 only
# ============================================================================
if(CMAKE_SIZEOF_VOID_P EQUAL 8)

set(CATMGR_SRC_DIR "${CMAKE_SOURCE_DIR}/Source/Common/CategoryManager")
set(CATMGR_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/OpcCategoryManager")
set(OPC_UTILS_DIR  "${CMAKE_SOURCE_DIR}/Source/Shared/OpcUtilityClasses")
file(MAKE_DIRECTORY "${CATMGR_BIN_DIR}")

# MIDL: CategoryManager.idl → CategoryManager.h, CategoryManager_i.c, CategoryManager.tlb
add_custom_command(
    OUTPUT
        "${CATMGR_BIN_DIR}/CategoryManager.h"
        "${CATMGR_BIN_DIR}/CategoryManager_i.c"
        "${CATMGR_BIN_DIR}/CategoryManager.tlb"
    COMMAND midl /nologo /env x64
        /I "${CMAKE_SOURCE_DIR}/Source/Common/ProxyStub"
        /tlb "${CATMGR_BIN_DIR}/CategoryManager.tlb"
        /h   "${CATMGR_BIN_DIR}/CategoryManager.h"
        /iid "${CATMGR_BIN_DIR}/CategoryManager_i.c"
        "${CATMGR_SRC_DIR}/CategoryManager.idl"
    DEPENDS "${CATMGR_SRC_DIR}/CategoryManager.idl"
            "${CMAKE_SOURCE_DIR}/Source/Common/ProxyStub/opccomn.idl"
    WORKING_DIRECTORY "${CATMGR_SRC_DIR}"
    COMMENT "MIDL: CategoryManager.idl"
    VERBATIM
)

# Use MIDL output from opccomn_ps for opccomn.h / opccomn_i.c
set(OPCCOMN_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/opccomn_ps")
# opcsec.h comes from opcsec_ps MIDL output (needed by COpcCommon.h)
set(OPCSEC_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/opcsec_ps")
# opcda.h comes from opcproxy MIDL output (needed by COpcConnectionPoint.cpp)
set(OPCDA_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/opcproxy")

# OPC utility framework sources + DCOM Config sources needed by COpcClassFactory
set(DCOM_CFG_DIR "${CMAKE_SOURCE_DIR}/Source/Common/ServerEnumerator/DCOM Config")
file(GLOB OPC_UTILS_SOURCES "${OPC_UTILS_DIR}/*.cpp")
file(GLOB DCOM_CFG_SOURCES  "${DCOM_CFG_DIR}/*.cpp")

add_executable(OpcCategoryManager
    "${CATMGR_SRC_DIR}/CategoryManager.cpp"
    "${CATMGR_SRC_DIR}/CCategoryManager.cpp"
    "${CATMGR_SRC_DIR}/StdAfx.cpp"
    ${OPC_UTILS_SOURCES}
    ${DCOM_CFG_SOURCES}
    "${CATMGR_SRC_DIR}/CategoryManager.rc"
)

add_custom_target(OpcCategoryManager_generate DEPENDS
    "${CATMGR_BIN_DIR}/CategoryManager.h"
    "${CATMGR_BIN_DIR}/CategoryManager_i.c"
    "${CATMGR_BIN_DIR}/CategoryManager.tlb"
)
add_dependencies(OpcCategoryManager OpcCategoryManager_generate)
add_dependencies(OpcCategoryManager opccomn_ps_generate)
# COpcCommon.h includes opcsec.h, COpcConnectionPoint.cpp includes opcda.h
add_dependencies(OpcCategoryManager opcsec_ps_generate)
add_dependencies(OpcCategoryManager opcproxy_generate)

target_compile_definitions(OpcCategoryManager PRIVATE
    WIN32 _WINDOWS _WIN32_DCOM _CRT_SECURE_NO_DEPRECATE
    _CRT_NON_CONFORMING_SWPRINTFS NDEBUG UNICODE _UNICODE
    WINVER=0x0601 _WIN32_WINNT=0x0601 NTDDI_VERSION=0x06010100
)

target_include_directories(OpcCategoryManager PRIVATE
    "${CATMGR_BIN_DIR}"
    "${CATMGR_SRC_DIR}"
    "${OPC_UTILS_DIR}"
    "${OPCCOMN_BIN_DIR}"
    "${OPCSEC_BIN_DIR}"
    "${OPCDA_BIN_DIR}"
    "${CMAKE_SOURCE_DIR}/Source/Common/ServerEnumerator"
    "${CMAKE_SOURCE_DIR}/Source/Include"
)

set_property(TARGET OpcCategoryManager PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
target_compile_options(OpcCategoryManager PRIVATE /GS /sdl /guard:cf /wd4996)
if(OPC_ENABLE_SPECTRE)
    target_compile_options(OpcCategoryManager PRIVATE /Qspectre)
endif()

target_link_options(OpcCategoryManager PRIVATE
    /NXCOMPAT /DYNAMICBASE /GUARD:CF /HIGHENTROPYVA /SUBSYSTEM:WINDOWS,6.01
)
target_link_libraries(OpcCategoryManager PRIVATE
    rpcrt4 ole32 oleaut32 advapi32 version
)

set_target_properties(OpcCategoryManager PROPERTIES
    OUTPUT_NAME "OpcCategoryManager"
    WIN32_EXECUTABLE TRUE
)

install(TARGETS OpcCategoryManager RUNTIME DESTINATION bin)

endif() # x64 only

# ============================================================================
# OPC DA 2.05a Test Server — both x86 and x64
# Output: OpcTestServer_x86.exe or OpcTestServer_x64.exe
# ============================================================================
if(OPC_BUILD_TESTS)

set(TESTSVR_SRC_DIR  "${CMAKE_SOURCE_DIR}/Source/Test/TestServer")
set(TESTSVR_BIN_DIR  "${CMAKE_CURRENT_BINARY_DIR}/OpcTestServer")
set(OPC_UTILS_DIR    "${CMAKE_SOURCE_DIR}/Source/Shared/OpcUtilityClasses")
set(SAMPLE_SVR_DIR   "${CMAKE_SOURCE_DIR}/Source/Shared/SampleServerClasses")
set(SAMPLE_DEV_DIR   "${CMAKE_SOURCE_DIR}/Source/Shared/SampleDevice")
set(SAMPLE_205_DIR   "${CMAKE_SOURCE_DIR}/Source/Shared/SampleServer205")
file(MAKE_DIRECTORY "${TESTSVR_BIN_DIR}")

# Determine platform tag and MIDL environment
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(TESTSVR_PLATFORM "x64")
    set(TESTSVR_MIDL_ENV "x64")
    set(TESTSVR_MIDL_DEFINE "/D" "OPC_PLATFORM_X64")
    set(TESTSVR_MIDL_ROBUST "")
else()
    set(TESTSVR_PLATFORM "x86")
    set(TESTSVR_MIDL_ENV "win32")
    set(TESTSVR_MIDL_DEFINE "")
    set(TESTSVR_MIDL_ROBUST "/no_robust")
endif()

# MIDL: OpcTestServer.idl → OpcTestServer.h, OpcTestServer_i.c, OpcTestServer.tlb
add_custom_command(
    OUTPUT
        "${TESTSVR_BIN_DIR}/OpcTestServer.h"
        "${TESTSVR_BIN_DIR}/OpcTestServer_i.c"
        "${TESTSVR_BIN_DIR}/OpcTestServer.tlb"
    COMMAND midl /nologo ${TESTSVR_MIDL_ROBUST} /env ${TESTSVR_MIDL_ENV}
        ${TESTSVR_MIDL_DEFINE}
        /I "${CMAKE_SOURCE_DIR}/Source/Common/ProxyStub"
        /I "${CMAKE_SOURCE_DIR}/Source/DataAccess/ProxyStub"
        /tlb "${TESTSVR_BIN_DIR}/OpcTestServer.tlb"
        /h   "${TESTSVR_BIN_DIR}/OpcTestServer.h"
        /iid "${TESTSVR_BIN_DIR}/OpcTestServer_i.c"
        "${TESTSVR_SRC_DIR}/OpcTestServer.idl"
    DEPENDS "${TESTSVR_SRC_DIR}/OpcTestServer.idl"
            "${CMAKE_SOURCE_DIR}/Source/Common/ProxyStub/opccomn.idl"
            "${CMAKE_SOURCE_DIR}/Source/DataAccess/ProxyStub/opcda.idl"
    WORKING_DIRECTORY "${TESTSVR_SRC_DIR}"
    COMMENT "MIDL: OpcTestServer.idl (${TESTSVR_PLATFORM})"
    VERBATIM
)

# Reuse MIDL output directories from proxy stubs
set(OPCCOMN_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/opccomn_ps")
set(OPCSEC_BIN_DIR  "${CMAKE_CURRENT_BINARY_DIR}/opcsec_ps")
set(OPCDA_BIN_DIR   "${CMAKE_CURRENT_BINARY_DIR}/opcproxy")

# Gather reused source files
set(DCOM_CFG_DIR "${CMAKE_SOURCE_DIR}/Source/Common/ServerEnumerator/DCOM Config")
file(GLOB TESTSVR_OPC_UTILS_SOURCES "${OPC_UTILS_DIR}/*.cpp")
file(GLOB TESTSVR_DCOM_CFG_SOURCES  "${DCOM_CFG_DIR}/*.cpp")
file(GLOB TESTSVR_SAMPLE_SVR_SOURCES "${SAMPLE_SVR_DIR}/*.cpp")
list(FILTER TESTSVR_SAMPLE_SVR_SOURCES EXCLUDE REGEX "StdAfx\\.cpp$")
file(GLOB TESTSVR_SAMPLE_DEV_SOURCES "${SAMPLE_DEV_DIR}/*.cpp")
list(FILTER TESTSVR_SAMPLE_DEV_SOURCES EXCLUDE REGEX "StdAfx\\.cpp$")

add_executable(OpcTestServer
    "${TESTSVR_SRC_DIR}/OpcTestServer.cpp"
    "${TESTSVR_SRC_DIR}/COpcTestServer.cpp"
    "${TESTSVR_SRC_DIR}/StdAfx.cpp"
    "${SAMPLE_205_DIR}/COpcDa20Cache.cpp"
    ${TESTSVR_OPC_UTILS_SOURCES}
    ${TESTSVR_DCOM_CFG_SOURCES}
    ${TESTSVR_SAMPLE_SVR_SOURCES}
    ${TESTSVR_SAMPLE_DEV_SOURCES}
    "${TESTSVR_SRC_DIR}/OpcTestServer.rc"
)

add_custom_target(OpcTestServer_generate DEPENDS
    "${TESTSVR_BIN_DIR}/OpcTestServer.h"
    "${TESTSVR_BIN_DIR}/OpcTestServer_i.c"
    "${TESTSVR_BIN_DIR}/OpcTestServer.tlb"
)
add_dependencies(OpcTestServer OpcTestServer_generate)
add_dependencies(OpcTestServer opccomn_ps_generate)
add_dependencies(OpcTestServer opcsec_ps_generate)
add_dependencies(OpcTestServer opcproxy_generate)

target_compile_definitions(OpcTestServer PRIVATE
    WIN32 _WINDOWS _WIN32_DCOM _CRT_SECURE_NO_DEPRECATE
    _CRT_NON_CONFORMING_SWPRINTFS NDEBUG UNICODE _UNICODE
    WINVER=0x0601 _WIN32_WINNT=0x0601 NTDDI_VERSION=0x06010100
    $<$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>:OPC_PLATFORM_X64>
)

target_include_directories(OpcTestServer PRIVATE
    "${TESTSVR_BIN_DIR}"
    "${TESTSVR_SRC_DIR}"
    "${OPC_UTILS_DIR}"
    "${OPCCOMN_BIN_DIR}"
    "${OPCSEC_BIN_DIR}"
    "${OPCDA_BIN_DIR}"
    "${SAMPLE_SVR_DIR}"
    "${SAMPLE_DEV_DIR}"
    "${SAMPLE_205_DIR}"
    "${CMAKE_SOURCE_DIR}/Source/Common/ServerEnumerator"
    "${CMAKE_SOURCE_DIR}/Source/Include"
)

set_property(TARGET OpcTestServer PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
target_compile_options(OpcTestServer PRIVATE /GS /sdl /guard:cf /wd4996)
if(OPC_ENABLE_SPECTRE)
    target_compile_options(OpcTestServer PRIVATE /Qspectre)
endif()

target_link_options(OpcTestServer PRIVATE
    /NXCOMPAT /DYNAMICBASE /GUARD:CF /SUBSYSTEM:WINDOWS,6.01
)
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    target_link_options(OpcTestServer PRIVATE /SAFESEH)
endif()
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    target_link_options(OpcTestServer PRIVATE /HIGHENTROPYVA)
endif()
target_link_libraries(OpcTestServer PRIVATE
    rpcrt4 ole32 oleaut32 advapi32 version
)

set_target_properties(OpcTestServer PROPERTIES
    OUTPUT_NAME "OpcTestServer_${TESTSVR_PLATFORM}"
    WIN32_EXECUTABLE TRUE
)

install(TARGETS OpcTestServer RUNTIME DESTINATION bin)
install(FILES "${TESTSVR_SRC_DIR}/OpcTestServer.config.xml"
        DESTINATION bin RENAME "OpcTestServer_${TESTSVR_PLATFORM}.config.xml")

# ============================================================================
# OPC DA Test Client — both x86 and x64
# Output: OpcTestClient_x86.exe or OpcTestClient_x64.exe
# ============================================================================

add_executable(OpcTestClient
    "${CMAKE_SOURCE_DIR}/Source/Test/TestClient/OpcTestClient.cpp"
)
add_dependencies(OpcTestClient opccomn_ps_generate opcproxy_generate)

target_include_directories(OpcTestClient PRIVATE
    "${OPCCOMN_BIN_DIR}"
    "${OPCDA_BIN_DIR}"
)

target_compile_definitions(OpcTestClient PRIVATE
    WIN32 _WINDOWS _WIN32_DCOM NDEBUG UNICODE _UNICODE
    WINVER=0x0601 _WIN32_WINNT=0x0601
)

set_property(TARGET OpcTestClient PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
target_compile_options(OpcTestClient PRIVATE /GS /sdl /guard:cf /wd4996)
if(OPC_ENABLE_SPECTRE)
    target_compile_options(OpcTestClient PRIVATE /Qspectre)
endif()

target_link_options(OpcTestClient PRIVATE
    /NXCOMPAT /DYNAMICBASE /GUARD:CF /SUBSYSTEM:CONSOLE,6.01
)
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    target_link_options(OpcTestClient PRIVATE /SAFESEH)
endif()
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    target_link_options(OpcTestClient PRIVATE /HIGHENTROPYVA)
endif()
target_link_libraries(OpcTestClient PRIVATE ole32 oleaut32 rpcrt4)

set_target_properties(OpcTestClient PROPERTIES
    OUTPUT_NAME "OpcTestClient_${TESTSVR_PLATFORM}"
)

install(TARGETS OpcTestClient RUNTIME DESTINATION bin)

endif() # OPC_BUILD_TESTS

# ============================================================================
# SDK Headers — install MIDL/MC outputs + source headers/IDLs to include/
# These are packaged into the merge modules as optional SDK files.
# ============================================================================

# MIDL-generated headers (.h) and IID implementation files (_i.c)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/opccomn_ps/opccomn.h"
    "${CMAKE_CURRENT_BINARY_DIR}/opccomn_ps/opccomn_i.c"
    "${CMAKE_CURRENT_BINARY_DIR}/opcproxy/opcda.h"
    "${CMAKE_CURRENT_BINARY_DIR}/opcproxy/opcda_i.c"
    "${CMAKE_CURRENT_BINARY_DIR}/opc_aeps/opc_ae.h"
    "${CMAKE_CURRENT_BINARY_DIR}/opc_aeps/opc_ae_i.c"
    "${CMAKE_CURRENT_BINARY_DIR}/opcbc_ps/opcbc.h"
    "${CMAKE_CURRENT_BINARY_DIR}/opcbc_ps/opcbc_i.c"
    "${CMAKE_CURRENT_BINARY_DIR}/OpcCmdPs/OpcCmd.h"
    "${CMAKE_CURRENT_BINARY_DIR}/OpcCmdPs/OpcCmd_i.c"
    "${CMAKE_CURRENT_BINARY_DIR}/OpcDxPs/OpcDx.h"
    "${CMAKE_CURRENT_BINARY_DIR}/OpcDxPs/OpcDx_i.c"
    "${CMAKE_CURRENT_BINARY_DIR}/opchda_ps/opchda.h"
    "${CMAKE_CURRENT_BINARY_DIR}/opchda_ps/opchda_i.c"
    "${CMAKE_CURRENT_BINARY_DIR}/opcsec_ps/opcSec.h"
    "${CMAKE_CURRENT_BINARY_DIR}/opcsec_ps/opcSec_i.c"
    DESTINATION include
)

# MC-generated error code headers
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/opcproxy/opcerror.h"
    "${CMAKE_CURRENT_BINARY_DIR}/opc_aeps/opcae_er.h"
    "${CMAKE_CURRENT_BINARY_DIR}/opcbc_ps/OpcBatchError.h"
    "${CMAKE_CURRENT_BINARY_DIR}/OpcCmdPs/OpcCmdError.h"
    "${CMAKE_CURRENT_BINARY_DIR}/OpcDxPs/OpcDxError.h"
    "${CMAKE_CURRENT_BINARY_DIR}/opchda_ps/OpcHda_Error.h"
    "${CMAKE_CURRENT_BINARY_DIR}/opcsec_ps/OpcErrSec.h"
    DESTINATION include
)

# IDL source files
install(FILES
    "${CMAKE_SOURCE_DIR}/Source/Common/ProxyStub/opccomn.idl"
    "${CMAKE_SOURCE_DIR}/Source/DataAccess/ProxyStub/opcda.idl"
    "${CMAKE_SOURCE_DIR}/Source/AlarmsAndEvents/ProxyStub/opc_ae.idl"
    "${CMAKE_SOURCE_DIR}/Source/Batch/ProxyStub/opcbc.idl"
    "${CMAKE_SOURCE_DIR}/Source/Commands/ProxyStub/OpcCmd.idl"
    "${CMAKE_SOURCE_DIR}/Source/DataExchange/ProxyStub/OpcDx.idl"
    "${CMAKE_SOURCE_DIR}/Source/HistoricalDataAccess/ProxyStub/opchda.idl"
    "${CMAKE_SOURCE_DIR}/Source/Security/ProxyStub/opcSec.idl"
    DESTINATION include
)

# Static definition/property headers (not generated by MIDL/MC)
install(FILES
    "${CMAKE_SOURCE_DIR}/Source/AlarmsAndEvents/ProxyStub/opcaedef.h"
    "${CMAKE_SOURCE_DIR}/Source/Batch/ProxyStub/OpcBatchProps.h"
    "${CMAKE_SOURCE_DIR}/Source/Batch/ProxyStub/OpcBatchDef.h"
    DESTINATION include
)

# OpcEnum headers and IDL (x86 only — OpcEnum is an x86-only target)
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/OpcEnum/OpcEnum.h"
        "${CMAKE_CURRENT_BINARY_DIR}/OpcEnum/OpcEnum_i.c"
        "${CMAKE_SOURCE_DIR}/Source/Common/ServerEnumerator/OpcEnum.idl"
        DESTINATION include
    )
endif()

# Proxy/stub DLL copies in include/ (SDK reference copies for developers)
install(FILES
    $<TARGET_FILE:opccomn_ps>
    $<TARGET_FILE:opcproxy>
    $<TARGET_FILE:opc_aeps>
    $<TARGET_FILE:opcbc_ps>
    $<TARGET_FILE:OpcCmdPs>
    $<TARGET_FILE:OpcDxPs>
    $<TARGET_FILE:opchda_ps>
    $<TARGET_FILE:opcsec_ps>
    DESTINATION include
)
