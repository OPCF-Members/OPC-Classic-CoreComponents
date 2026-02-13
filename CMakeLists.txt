cmake_minimum_required(VERSION 3.20)
project(OpcCoreComponents C)

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
