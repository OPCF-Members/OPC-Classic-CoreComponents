# OpcProxyStub.cmake - Reusable function for building OPC COM proxy/stub DLLs
#
# This module provides add_opc_proxy_stub() which handles:
#   1. MIDL compilation of .idl files
#   2. MC (message compiler) compilation of .mc files
#   3. Resource compilation (.rc) with generated .tlb and error resources
#   4. Linking into a self-registering COM proxy/stub DLL

function(add_opc_proxy_stub)
    cmake_parse_arguments(PS
        ""                                  # Options (boolean)
        "TARGET;IDL;DEF;RC;TLB;SOURCE_DIR"  # Single-value args
        "MC"                                # Multi-value args
        ${ARGN}
    )

    if(NOT PS_SOURCE_DIR)
        set(PS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()

    set(SRC_DIR "${PS_SOURCE_DIR}")
    # Each target gets its own binary subdirectory to avoid dlldata.c conflicts
    set(BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/${PS_TARGET}")
    file(MAKE_DIRECTORY "${BIN_DIR}")

    # Derive names from IDL file
    get_filename_component(IDL_NAME "${PS_IDL}" NAME_WE)

    # Use explicit TLB name if provided, otherwise derive from IDL name
    if(PS_TLB)
        set(TLB_NAME "${PS_TLB}")
    else()
        set(TLB_NAME "${IDL_NAME}.tlb")
    endif()

    # Determine MIDL environment flag based on target architecture
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(MIDL_ENV "x64")
        set(MIDL_ROBUST_FLAG "")
    else()
        set(MIDL_ENV "win32")
        set(MIDL_ROBUST_FLAG "/no_robust")
    endif()

    # ---------- MIDL compilation ----------
    set(MIDL_HEADER   "${BIN_DIR}/${IDL_NAME}.h")
    set(MIDL_IID      "${BIN_DIR}/${IDL_NAME}_i.c")
    set(MIDL_PROXY    "${BIN_DIR}/${IDL_NAME}_p.c")
    set(MIDL_DLLDATA  "${BIN_DIR}/dlldata.c")
    set(MIDL_TLB      "${BIN_DIR}/${TLB_NAME}")

    add_custom_command(
        OUTPUT "${MIDL_HEADER}" "${MIDL_IID}" "${MIDL_PROXY}" "${MIDL_DLLDATA}" "${MIDL_TLB}"
        COMMAND midl
            /nologo ${MIDL_ROBUST_FLAG}
            /env "${MIDL_ENV}"
            /tlb "${MIDL_TLB}"
            /h "${MIDL_HEADER}"
            /iid "${MIDL_IID}"
            /proxy "${MIDL_PROXY}"
            /dlldata "${MIDL_DLLDATA}"
            "${SRC_DIR}/${PS_IDL}"
        DEPENDS "${SRC_DIR}/${PS_IDL}"
        WORKING_DIRECTORY "${SRC_DIR}"
        COMMENT "MIDL: ${PS_IDL} -> ${IDL_NAME}_p.c, ${IDL_NAME}_i.c, ${TLB_NAME}"
        VERBATIM
    )

    set(GENERATED_SOURCES "${MIDL_IID}" "${MIDL_PROXY}" "${MIDL_DLLDATA}")
    set(ALL_DEPENDS "${MIDL_HEADER}" "${MIDL_TLB}")

    # ---------- MC (message compiler) ----------
    if(PS_MC)
        foreach(MC_FILE IN LISTS PS_MC)
            get_filename_component(MC_NAME "${MC_FILE}" NAME_WE)

            set(MC_HEADER "${BIN_DIR}/${MC_NAME}.h")
            set(MC_RC     "${BIN_DIR}/${MC_NAME}.rc")

            add_custom_command(
                OUTPUT "${MC_HEADER}" "${MC_RC}"
                COMMAND mc -h "${BIN_DIR}" -r "${BIN_DIR}" "${SRC_DIR}/${MC_FILE}"
                DEPENDS "${SRC_DIR}/${MC_FILE}"
                WORKING_DIRECTORY "${BIN_DIR}"
                COMMENT "MC: ${MC_FILE} -> ${MC_NAME}.h, ${MC_NAME}.rc"
                VERBATIM
            )

            list(APPEND ALL_DEPENDS "${MC_HEADER}" "${MC_RC}")
        endforeach()
    endif()

    # ---------- Create the DLL target ----------
    add_library(${PS_TARGET} SHARED
        ${GENERATED_SOURCES}
        "${SRC_DIR}/${PS_RC}"
    )

    # Mark generated files so CMake knows they don't exist at configure time
    set_source_files_properties(${GENERATED_SOURCES} ${ALL_DEPENDS}
        PROPERTIES GENERATED TRUE
    )

    # Ensure MIDL/MC run before compilation
    add_custom_target(${PS_TARGET}_generate DEPENDS ${GENERATED_SOURCES} ${ALL_DEPENDS})
    add_dependencies(${PS_TARGET} ${PS_TARGET}_generate)

    # Compiler settings matching the original VS2008 Release configuration
    # Target Windows 7 SP1 (NT 6.1) - restricts SDK headers to Win7-compatible APIs
    target_compile_definitions(${PS_TARGET} PRIVATE
        WIN32
        _WINDOWS
        _WIN32_DCOM
        REGISTER_PROXY_DLL
        NDEBUG
        WINVER=0x0601
        _WIN32_WINNT=0x0601
        NTDDI_VERSION=0x06010100
    )

    # Include paths: build dir (for MIDL/MC generated files), source dir, shared includes
    target_include_directories(${PS_TARGET} PRIVATE
        "${BIN_DIR}"
        "${SRC_DIR}"
        "${CMAKE_SOURCE_DIR}/Source/Include"
    )

    # Static CRT (RuntimeLibrary=0 in original vcproj = /MT)
    set_property(TARGET ${PS_TARGET} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")

    # Security hardening (compiler)
    target_compile_options(${PS_TARGET} PRIVATE
        /GS             # Buffer security check (on by default, explicit for clarity)
        /sdl            # Security Development Lifecycle checks
        /guard:cf       # Control Flow Guard (enforced on Win8.1+, ignored on XP)
    )
    # Spectre v1 mitigation - requires Spectre-mitigated libs from VS Installer
    if(OPC_ENABLE_SPECTRE)
        target_compile_options(${PS_TARGET} PRIVATE /Qspectre)
    endif()

    # Security hardening (linker)
    target_link_options(${PS_TARGET} PRIVATE
        /NXCOMPAT       # DEP (Data Execution Prevention)
        /DYNAMICBASE    # ASLR (Address Space Layout Randomization)
        /GUARD:CF       # Control Flow Guard metadata
    )
    # x86-only: Safe SEH + subsystem 6.01 (Windows 7 SP1)
    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
        target_link_options(${PS_TARGET} PRIVATE /SAFESEH)
        target_link_options(${PS_TARGET} PRIVATE /SUBSYSTEM:WINDOWS,6.01)
    endif()
    # x64-only: High-Entropy ASLR + subsystem 6.01 (Windows 7 SP1 / Server 2008 R2)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        target_link_options(${PS_TARGET} PRIVATE /HIGHENTROPYVA)
        target_link_options(${PS_TARGET} PRIVATE /SUBSYSTEM:WINDOWS,6.01)
    endif()

    target_link_libraries(${PS_TARGET} PRIVATE rpcrt4)

    set_target_properties(${PS_TARGET} PROPERTIES
        LINKER_LANGUAGE C
        OUTPUT_NAME "${PS_TARGET}"
        PREFIX ""
    )

    # Module definition file for exports (DllRegisterServer, etc.)
    target_link_options(${PS_TARGET} PRIVATE "/DEF:${SRC_DIR}/${PS_DEF}")

    # Install rule - puts DLLs in the output bin directory
    install(TARGETS ${PS_TARGET}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION bin
    )
endfunction()
